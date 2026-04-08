#include <array>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus_008
{
    bool a;
    bool b;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<Stimulus_008, 5> stimuli{{
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

        const bool expected = (stim.a == stim.b);
        if (sim.out != expected)
        {
            std::cerr << "[GrhTB] dut_008 failed: a=" << static_cast<int>(stim.a)
                      << ", b=" << static_cast<int>(stim.b)
                      << ", expected out=" << static_cast<int>(expected)
                      << ", got " << static_cast<int>(sim.out) << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_008 passed: out == ~(a ^ b) for all stimuli\n";
    return EXIT_SUCCESS;
}
