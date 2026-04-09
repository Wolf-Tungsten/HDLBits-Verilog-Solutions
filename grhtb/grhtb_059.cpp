#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static std::uint8_t bit(std::uint64_t value, int index)
{
    return static_cast<std::uint8_t>((value >> index) & 1u);
}

static bool check_outputs(GrhSIM_top_module &sim, std::uint8_t value)
{
    sim.in = value & 0xFu;
    sim.eval();

    for (int i = 0; i < 3; ++i) {
        if (bit(sim.out_both, i) != static_cast<std::uint8_t>(bit(value, i) & bit(value, i + 1))) {
            return false;
        }
    }
    for (int i = 0; i < 3; ++i) {
        if (bit(sim.out_any, i) != static_cast<std::uint8_t>(bit(value, i) | bit(value, i + 1))) {
            return false;
        }
    }
    for (int i = 0; i < 3; ++i) {
        if (bit(sim.out_different, i) != static_cast<std::uint8_t>(bit(value, i) ^ bit(value, i + 1))) {
            return false;
        }
    }
    return bit(sim.out_different, 3) == static_cast<std::uint8_t>(bit(value, 3) ^ bit(value, 0));
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    for (int value = 0; value < 16; ++value) {
        if (!check_outputs(sim, static_cast<std::uint8_t>(value))) {
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < 3; ++i) {
        std::uint8_t value = 0;
        for (int rep = 0; rep < 2; ++rep) {
            value = static_cast<std::uint8_t>(1u << (i + 1));
            if (!check_outputs(sim, value) || !check_outputs(sim, static_cast<std::uint8_t>(value | (1u << i))) ||
                !check_outputs(sim, value)) {
                return EXIT_FAILURE;
            }
        }
        for (int rep = 0; rep < 2; ++rep) {
            value = static_cast<std::uint8_t>(1u << i);
            if (!check_outputs(sim, value) || !check_outputs(sim, static_cast<std::uint8_t>(value | (1u << (i + 1)))) ||
                !check_outputs(sim, value)) {
                return EXIT_FAILURE;
            }
        }
    }

    for (int rep = 0; rep < 2; ++rep) {
        if (!check_outputs(sim, 1u << 0) || !check_outputs(sim, static_cast<std::uint8_t>((1u << 0) | (1u << 3))) ||
            !check_outputs(sim, 1u << 0)) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_059 passed: neighbor pair ops on 4-bit ring\n";
    return EXIT_SUCCESS;
}
