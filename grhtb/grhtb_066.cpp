#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static bool check_outputs(GrhSIM_top_module &sim, std::uint8_t a, std::uint8_t b)
{
    sim.a = a;
    sim.b = b;
    sim.eval();
    return sim.sum == static_cast<std::uint8_t>(a ^ b) && sim.cout == static_cast<std::uint8_t>(a & b);
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
            !check_outputs(sim, 0, 1) || !check_outputs(sim, 0, 0)) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_066 passed: half adder\n";
    return EXIT_SUCCESS;
}
