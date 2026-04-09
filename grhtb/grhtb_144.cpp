#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static std::uint8_t expected_y0(std::uint8_t y, bool x)
{
    switch (((y & 0x7u) << 1) | (x ? 1u : 0u)) {
    case 0x0: return 0u;
    case 0x1: return 1u;
    case 0x2: return 1u;
    case 0x3: return 0u;
    case 0x4: return 0u;
    case 0x5: return 1u;
    case 0x6: return 1u;
    case 0x7: return 0u;
    case 0x8: return 1u;
    case 0x9: return 0u;
    default: return 0u;
    }
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    for (std::uint8_t y = 0; y < 8; ++y) {
        for (const bool x : {false, true}) {
            sim.y = y;
            sim.x = x;
            sim.eval();
            const bool z = y == 0x3u || y == 0x4u;
            const std::uint8_t y0 = expected_y0(y, x);
            if (sim.z != z || sim.Y0 != y0) {
                std::cerr << "[GrhTB] dut_144 failed(y=" << static_cast<int>(y)
                          << ", x=" << static_cast<int>(x) << ")\n";
                return EXIT_FAILURE;
            }
        }
    }

    std::cout << "[GrhTB] dut_144 passed: Mealy-style combinational block\n";
    return EXIT_SUCCESS;
}
