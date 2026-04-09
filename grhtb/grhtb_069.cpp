#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static bool check_outputs(GrhSIM_top_module &sim, std::uint8_t x, std::uint8_t y)
{
    sim.x = x & 0xFu;
    sim.y = y & 0xFu;
    sim.eval();
    return sim.sum == static_cast<std::uint8_t>((x + y) & 0x1Fu);
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    for (int x = 0; x < 16; ++x) {
        for (int y = 0; y < 16; ++y) {
            if (!check_outputs(sim, static_cast<std::uint8_t>(x), static_cast<std::uint8_t>(y))) {
                return EXIT_FAILURE;
            }
        }
    }

    for (int rep = 0; rep < 2; ++rep) {
        if (!check_outputs(sim, 0, 0) || !check_outputs(sim, 0xFu, 0) || !check_outputs(sim, 0, 0) ||
            !check_outputs(sim, 0, 0xFu) || !check_outputs(sim, 0, 0)) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_069 passed: 4-bit adder\n";
    return EXIT_SUCCESS;
}
