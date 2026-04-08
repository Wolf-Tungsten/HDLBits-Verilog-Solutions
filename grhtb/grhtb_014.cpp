#include <array>
#include <cstdlib>
#include <cstdint>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus_014
{
    uint8_t a;
    uint8_t b;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<Stimulus_014, 5> stimuli{{
        {0u, 0u},
        {7u, 0u},
        {7u, 7u},
        {0u, 7u},
        {0u, 0u},
    }};

    for (const auto &stim : stimuli)
    {
        sim.a = stim.a & 0x7u;
        sim.b = stim.b & 0x7u;
        sim.eval();

        const uint8_t expectedOrBit = static_cast<uint8_t>((stim.a | stim.b) & 0x7u);
        const bool expectedOrLogical = (stim.a != 0u) || (stim.b != 0u);
        const uint8_t expectedNotHi = static_cast<uint8_t>((~stim.b) & 0x7u);
        const uint8_t expectedNotLo = static_cast<uint8_t>((~stim.a) & 0x7u);
        if ((sim.out_or_bitwise & 0x7u) != expectedOrBit ||
            sim.out_or_logical != expectedOrLogical ||
            ((sim.out_not >> 3) & 0x7u) != expectedNotHi ||
            (sim.out_not & 0x7u) != expectedNotLo)
        {
            std::cerr << "[GrhTB] dut_014 failed: a=" << static_cast<int>(stim.a)
                      << ", b=" << static_cast<int>(stim.b)
                      << ", expected or_bit=" << static_cast<int>(expectedOrBit)
                      << ", or_log=" << static_cast<int>(expectedOrLogical)
                      << ", not_hi/lo=" << static_cast<int>(expectedNotHi)
                      << "/" << static_cast<int>(expectedNotLo)
                      << ", got or_bit=" << static_cast<int>(sim.out_or_bitwise & 0x7u)
                      << ", or_log=" << static_cast<int>(sim.out_or_logical)
                      << ", out_not=" << static_cast<int>(sim.out_not & 0x3Fu) << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_014 passed: bitwise/logical or and inversion checks\n";
    return EXIT_SUCCESS;
}
