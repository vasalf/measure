#include <iostream>
#include <measure.h>

int main() {
    auto ret = measure::measure(
        [](){
            std::uint64_t a = 179;
            std::uint64_t b = 239;
            std::uint64_t seed = 57;
            for (int i = 0; i < 1'000'000; i++) {
                seed = (seed * a + b);
            }
            return seed;
        }
    );
    std::cout << ret << std::endl;
    return 0;
}
