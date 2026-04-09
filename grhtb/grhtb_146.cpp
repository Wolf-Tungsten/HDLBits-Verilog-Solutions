#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    auto apply = [&](std::uint8_t y, bool w) {
        sim.y = static_cast<std::uint8_t>(y & 0x3Fu);
        sim.w = w;
        sim.eval();
        const bool y2 = ((y & 0x01u) != 0u) && !w;
        const bool y4 = ((((y >> 1) & 1u) != 0u) || (((y >> 2) & 1u) != 0u) || (((y >> 4) & 1u) != 0u) ||
                         (((y >> 5) & 1u) != 0u)) &&
                        w;
        if (sim.Y2 != y2 || sim.Y4 != y4) {
            std::cerr << "[GrhTB] dut_146 failed(y=0x" << std::hex << static_cast<int>(y)
                      << ", w=" << std::dec << static_cast<int>(w) << ")\n";
            return false;
        }
        return true;
    };

    for (const auto y : {0x00u, 0x3Fu}) {
        for (const auto w : {false, true}) {
            if (!apply(static_cast<std::uint8_t>(y), w)) {
                return EXIT_FAILURE;
            }
        }
    }
    for (std::uint8_t bit = 0; bit < 6; ++bit) {
        const std::uint8_t y = static_cast<std::uint8_t>(1u << bit);
        if (!apply(y, false) || !apply(y, true)) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_146 passed: simple output decode\n";
    return EXIT_SUCCESS;
}
