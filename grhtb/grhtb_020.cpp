#include <array>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus_020
{
    bool a;
    bool b;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<Stimulus_020, 5> stimuli{{
        {false, false},
        {true, false},
        {true, true},
        {false, true},
        {false, false},
    }};

    for (const auto &stim : stimuli)
    {
        sim.a = stim.a;
        sim.b = stim.b;
        sim.eval();

        const bool expected = stim.a != stim.b;
        if (sim.out != expected)
        {
            std::cerr << "[GrhTB] dut_020 failed: a=" << static_cast<int>(stim.a)
                      << ", b=" << static_cast<int>(stim.b)
                      << ", expected out=" << static_cast<int>(expected)
                      << ", got " << static_cast<int>(sim.out) << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_020 passed: out == a^b through mod_a\n";
    return EXIT_SUCCESS;
}
