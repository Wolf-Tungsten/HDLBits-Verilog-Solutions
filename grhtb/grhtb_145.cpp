#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static std::uint8_t expected_y2(std::uint8_t y, bool w)
{
    switch (((y & 0x7u) << 1) | (w ? 1u : 0u)) {
    case 0x0: return 0u;
    case 0x1: return 0u;
    case 0x2: return 1u;
    case 0x3: return 1u;
    case 0x4: return 0u;
    case 0x5: return 1u;
    case 0x6: return 0u;
    case 0x7: return 0u;
    case 0x8: return 0u;
    case 0x9: return 1u;
    case 0xA: return 1u;
    case 0xB: return 1u;
    default: return 0u;
    }
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    for (std::uint8_t y = 0; y < 8; ++y) {
        for (const bool w : {false, true}) {
            sim.y = y;
            sim.w = w;
            sim.eval();
            if (sim.Y2 != expected_y2(y, w)) {
                std::cerr << "[GrhTB] dut_145 failed(y=" << static_cast<int>(y)
                          << ", w=" << static_cast<int>(w) << ")\n";
                return EXIT_FAILURE;
            }
        }
    }

    for (std::uint8_t bit = 0; bit < 3; ++bit) {
        const std::uint8_t mask = static_cast<std::uint8_t>(1u << bit);
        for (const auto y : {std::uint8_t{0}, mask, std::uint8_t{0}}) {
            sim.y = y;
            sim.w = false;
            sim.eval();
        }
    }
    for (const auto w : {false, true, false}) {
        sim.y = 0x2u;
        sim.w = w;
        sim.eval();
    }

    std::cout << "[GrhTB] dut_145 passed: combinational next-state decoder\n";
    return EXIT_SUCCESS;
}
