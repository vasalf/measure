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


#include <iostream>
#include <measure.h>

int main() {
    const std::uint32_t MOD32 = 1'000'000'007;
    const std::uint64_t MOD64 = 1'000'000'000'000'000'009ULL;

    std::uint32_t u32_ovf_ret = measure::measure(
        [seed = static_cast<std::uint32_t>(57)]() mutable {
            std::uint32_t a = 179;
            std::uint32_t b = 239;
            std::uint32_t sum = 0;
            for (std::uint32_t i = 0; i < 1'000'000; i++) {
                seed = (seed * a + b) % MOD32;
                sum += seed;
            }
            return sum;
        },
        measure::config{.name = L"32-bit integers, overflow"}
    );
    std::cout << u32_ovf_ret << std::endl;

    std::uint64_t u64_ovf_ret = measure::measure(
        [seed = static_cast<std::uint64_t>(57)]() mutable {
            std::uint64_t a = 179;
            std::uint64_t b = 239;
            std::uint64_t sum = 0;
            for (std::uint32_t i = 0; i < 1'000'000; i++) {
                seed = (seed * a + b) % MOD64;
                sum += seed;
            }
            return sum;
        },
        measure::config{.name = L"64-bit integers, overflow"}
    );
    std::cout << u64_ovf_ret << std::endl;

    std::uint32_t u32_mod_ret = measure::measure(
        [seed = static_cast<std::uint32_t>(57)]() mutable {
            std::uint32_t a = 179;
            std::uint32_t b = 239;
            std::uint32_t sum = 0;
            for (std::uint32_t i = 0; i < 1'000'000; i++) {
                seed = (seed * a + b) % MOD32;
                sum = (sum + seed) % MOD32;
            }
            return sum;
        },
        measure::config{.name = L"32-bit integers, division"}
    );
    std::cout << u32_mod_ret << std::endl;

    std::uint64_t u64_mod_ret = measure::measure(
        [seed = static_cast<std::uint64_t>(57)]() mutable {
            std::uint64_t a = 179;
            std::uint64_t b = 239;
            std::uint64_t sum = 0;
            for (std::uint32_t i = 0; i < 1'000'000; i++) {
                seed = (seed * a + b) % MOD64;
                sum = (sum + seed) % MOD64;
            }
            return sum;
        },
        measure::config{.name = L"64-bit integers, division"}
    );
    std::cout << u64_mod_ret << std::endl;

    std::uint32_t u32_if_ret = measure::measure(
        [seed = static_cast<std::uint32_t>(57)]() mutable {
            std::uint32_t a = 179;
            std::uint32_t b = 239;
            std::uint32_t sum = 0;
            for (std::uint32_t i = 0; i < 1'000'000; i++) {
                seed = (seed * a + b) % MOD32;
                sum = sum + seed;
                if (sum >= MOD32) {
                    sum -= MOD32;
                }
            }
            return sum;
        },
        measure::config{.name = L"32-bit integers, if"}
    );
    std::cout << u32_if_ret << std::endl;

    std::uint64_t u64_if_ret = measure::measure(
        [seed = static_cast<std::uint64_t>(57)]() mutable {
            std::uint64_t a = 179;
            std::uint64_t b = 239;
            std::uint64_t sum = 0;
            for (std::uint32_t i = 0; i < 1'000'000; i++) {
                seed = (seed * a + b) % MOD64;
                sum = sum + seed;
                if (sum >= MOD64) {
                    sum -= MOD64;
                }
            }
            return sum;
        },
        measure::config{.name = L"64-bit integers, if"}
    );
    std::cout << u64_if_ret << std::endl;

    return 0;
}
