// MIT License
//
// Copyright (c) 2022 Vasily Alferov
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef MEASURE_H_
#define MEASURE_H_

#include <iostream>
#include <cassert>
#include <cstring>
#include <locale>
#include <optional>
#include <sstream>
#include <string>
#include <utility>

#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace measure {
namespace detail {

inline long check_syscall_result(const char* name, long result) {
    if (result == -1) {
        std::cerr << name << "(2) failed: " << std::strerror(errno) << std::endl;
        std::abort();
    }
    return result;
}

struct fd_holder {
    int fd = -1;

    fd_holder(int file_descriptor) : fd(file_descriptor) {}

    fd_holder(const fd_holder&) = delete;
    fd_holder& operator=(const fd_holder&) = delete;

    fd_holder(fd_holder&& other) : fd(std::exchange(other.fd, -1)) {}

    fd_holder& operator=(fd_holder&& rhs) {
        std::swap(fd, rhs.fd);
        return *this;
    }

    ~fd_holder() {
        if (fd != -1) {
            check_syscall_result("close", ::close(fd));
        }
    }
};

namespace {
struct locale_setter {
    locale_setter() {
        std::locale::global(std::locale(""));
    }
} locale_setter_instance;
}

template<typename T>
inline std::wstring format_time(T ns) {
    std::wostringstream ss;
    ss.precision(3);
    ss << std::fixed;
    ss.imbue(std::locale(""));
    if (ns < 1'000) {
        ss << static_cast<double>(ns) << L" ??s";
    } else if (ns < 1'000'000) {
        ss << static_cast<double>(ns) / 1'000 << L" ??s";
    } else if (ns < 1'000'000'000) {
        ss << static_cast<double>(ns) / 1'000'000 << " ms";
    } else {
        ss << static_cast<double>(ns) / 1'000'000'000 << " s";
    }
    return ss.str();
}

}

struct config {
    std::optional<std::wstring_view> name{};
    std::uint64_t warmups = 0;
};

template <typename F>
auto measure(F &&f, const config& cfg = {}) {
    perf_event_attr instrs_attr{
        .type = PERF_TYPE_HARDWARE,
        .size = sizeof(perf_event_attr),
        .config = PERF_COUNT_HW_CPU_CYCLES,
        .read_format = PERF_FORMAT_TOTAL_TIME_RUNNING | PERF_FORMAT_GROUP,
        .disabled = 1,
        .exclude_kernel = 1,
        .exclude_hv = 1,
    };

    detail::fd_holder grp_fd {
        static_cast<int>(
            detail::check_syscall_result(
                "perf_event_open",
                ::syscall(SYS_perf_event_open, &instrs_attr, static_cast<pid_t>(0), -1, -1, 0ul)
            )
        )
    };

    perf_event_attr cycles_attr{
        .type = PERF_TYPE_HARDWARE,
        .size = sizeof(perf_event_attr),
        .config = PERF_COUNT_HW_CPU_CYCLES,
        .exclude_kernel = 1,
        .exclude_hv = 1,
    };

    detail::fd_holder cycles_fd{
        static_cast<int>(
            detail::check_syscall_result(
                "perf_event_open",
                ::syscall(SYS_perf_event_open, &cycles_attr, static_cast<pid_t>(0), -1, grp_fd.fd, 0ul)
            )
        )
    };

    if (cfg.warmups) {
        for (std::uint64_t i = 0; i < cfg.warmups; i++) {
            f();
        }
    }

    detail::check_syscall_result("ioctl", ::ioctl(grp_fd.fd, PERF_EVENT_IOC_ENABLE, 0));
    auto ret = f();
    detail::check_syscall_result("ioctl", ::ioctl(grp_fd.fd, PERF_EVENT_IOC_DISABLE, 0));

    struct {
        std::uint64_t nr;
        std::uint64_t time_running;
        std::uint64_t counters[2];
    } result;
    std::size_t read = detail::check_syscall_result("read", ::read(grp_fd.fd, &result, sizeof(result)));
    assert(read == sizeof(result));

    assert(result.nr == 2);
    auto time_running = result.time_running;
    auto cpu_instrs = result.counters[0];
    auto cpu_cycles = result.counters[1];
    double avg_instr_time = static_cast<double>(time_running) / cpu_instrs;
    double avg_cycle_time = static_cast<double>(time_running) / cpu_cycles;
    double avg_instrs_per_cycle = static_cast<double>(cpu_instrs) / cpu_cycles;

    std::wcerr << "=========== MEASURE REPORT ===========" << std::endl;
    if (cfg.name) {
        std::wcerr << "Name: " << *cfg.name << std::endl;
    }
    std::wcerr << "Time running: " << detail::format_time(time_running) << std::endl;
    std::wcerr << "CPU instructions: " << cpu_instrs << std::endl;
    std::wcerr << "CPU cycles: " << cpu_cycles << std::endl;
    std::wcerr << "Avg instruction time: " << detail::format_time(avg_instr_time) << std::endl;
    std::wcerr << "Avg cycle time: " << detail::format_time(avg_cycle_time) << std::endl;
    std::wcerr << "Avg intsructions per cycle: " << avg_instrs_per_cycle << std::endl;
    std::wcerr << "======================================" << std::endl << std::endl;

    return ret;
}
}

#endif
