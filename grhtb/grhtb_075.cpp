#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static std::uint8_t ref(std::uint8_t a, std::uint8_t b, std::uint8_t c)
{
    return static_cast<std::uint8_t>((a | (c & static_cast<std::uint8_t>(~b & 1u))) & 1u);
}

static bool check_outputs(GrhSIM_top_module &sim, std::uint8_t a, std::uint8_t b, std::uint8_t c, std::uint8_t d)
{
    sim.a = a;
    sim.b = b;
    sim.c = c;
    sim.d = d;
    sim.eval();
    return sim.out == ref(a, b, c);
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    for (int value = 0; value < 16; ++value) {
        if (!check_outputs(sim,
                           static_cast<std::uint8_t>(value & 1u),
                           static_cast<std::uint8_t>((value >> 1) & 1u),
                           static_cast<std::uint8_t>((value >> 2) & 1u),
                           static_cast<std::uint8_t>((value >> 3) & 1u))) {
            return EXIT_FAILURE;
        }
    }

    for (int rep = 0; rep < 2; ++rep) {
        if (!check_outputs(sim, 0, 0, 0, 0) || !check_outputs(sim, 1, 0, 0, 0) || !check_outputs(sim, 0, 0, 0, 0) ||
            !check_outputs(sim, 0, 1, 0, 0) || !check_outputs(sim, 0, 0, 0, 0) || !check_outputs(sim, 0, 0, 1, 0) ||
            !check_outputs(sim, 0, 0, 0, 0) || !check_outputs(sim, 0, 0, 0, 1) || !check_outputs(sim, 0, 0, 0, 0)) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_075 passed: logic with unused input d\n";
    return EXIT_SUCCESS;
}
