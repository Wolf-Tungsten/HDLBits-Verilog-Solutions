#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static std::uint8_t ref(std::uint8_t x1, std::uint8_t x2, std::uint8_t x3)
{
    return static_cast<std::uint8_t>((x1 & x3) | (static_cast<std::uint8_t>(~x3 & 1u) & x2));
}

static bool check_outputs(GrhSIM_top_module &sim, std::uint8_t x1, std::uint8_t x2, std::uint8_t x3)
{
    sim.x1 = x1;
    sim.x2 = x2;
    sim.x3 = x3;
    sim.eval();
    return sim.f == ref(x1, x2, x3);
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    for (int value = 0; value < 8; ++value) {
        if (!check_outputs(sim,
                           static_cast<std::uint8_t>((value >> 0) & 1u),
                           static_cast<std::uint8_t>((value >> 1) & 1u),
                           static_cast<std::uint8_t>((value >> 2) & 1u))) {
            return EXIT_FAILURE;
        }
    }

    for (int rep = 0; rep < 2; ++rep) {
        if (!check_outputs(sim, 0, 0, 0) || !check_outputs(sim, 1, 0, 0) || !check_outputs(sim, 0, 0, 0) ||
            !check_outputs(sim, 0, 1, 0) || !check_outputs(sim, 0, 0, 0) || !check_outputs(sim, 0, 0, 1) ||
            !check_outputs(sim, 0, 0, 0)) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_051 passed: f = x1&x3 | ~x3&x2\n";
    return EXIT_SUCCESS;
}
