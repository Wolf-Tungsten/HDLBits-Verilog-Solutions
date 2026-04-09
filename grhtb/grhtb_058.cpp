#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static std::uint8_t pop3(std::uint8_t value)
{
    return static_cast<std::uint8_t>((value & 1u) + ((value >> 1) & 1u) + ((value >> 2) & 1u));
}

static bool check_outputs(GrhSIM_top_module &sim, std::uint8_t value)
{
    sim.in = value & 7u;
    sim.eval();
    return sim.out == pop3(value);
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    for (int value = 0; value < 8; ++value) {
        if (!check_outputs(sim, static_cast<std::uint8_t>(value))) {
            return EXIT_FAILURE;
        }
    }

    for (int bit = 0; bit < 3; ++bit) {
        for (int rep = 0; rep < 2; ++rep) {
            if (!check_outputs(sim, 0) || !check_outputs(sim, static_cast<std::uint8_t>(1u << bit)) ||
                !check_outputs(sim, 0)) {
                return EXIT_FAILURE;
            }
        }
    }

    std::cout << "[GrhTB] dut_058 passed: popcount3\n";
    return EXIT_SUCCESS;
}
