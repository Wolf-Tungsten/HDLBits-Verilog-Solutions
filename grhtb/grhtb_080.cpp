#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static bool check_outputs(GrhSIM_top_module &sim, std::uint8_t c, std::uint8_t d)
{
    sim.c = c;
    sim.d = d;
    sim.eval();

    const std::uint8_t expected0 = static_cast<std::uint8_t>((c | d) & 1u);
    const std::uint8_t expected1 = 0u;
    const std::uint8_t expected2 = static_cast<std::uint8_t>(~d & 1u);
    const std::uint8_t expected3 = static_cast<std::uint8_t>((c & d) & 1u);
    const std::uint8_t actual = static_cast<std::uint8_t>(sim.mux_in & 0xFu);
    return ((actual >> 0) & 1u) == expected0 && ((actual >> 1) & 1u) == expected1 &&
           ((actual >> 2) & 1u) == expected2 && ((actual >> 3) & 1u) == expected3;
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    for (int value = 0; value < 4; ++value) {
        if (!check_outputs(sim,
                           static_cast<std::uint8_t>(value & 1u),
                           static_cast<std::uint8_t>((value >> 1) & 1u))) {
            return EXIT_FAILURE;
        }
    }

    for (int rep = 0; rep < 2; ++rep) {
        if (!check_outputs(sim, 0, 0) || !check_outputs(sim, 1, 0) || !check_outputs(sim, 0, 0) ||
            !check_outputs(sim, 0, 1) || !check_outputs(sim, 0, 0) || !check_outputs(sim, 1, 1) ||
            !check_outputs(sim, 0, 0)) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_080 passed: 2-input mux_in pattern generator\n";
    return EXIT_SUCCESS;
}
