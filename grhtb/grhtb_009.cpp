#include <array>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus_009
{
    bool a;
    bool b;
    bool c;
    bool d;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<Stimulus_009, 9> stimuli{{
        {false, false, false, false},
        {true, false, false, false},
        {true, true, false, false},
        {true, true, true, false},
        {true, true, true, true},
        {false, true, true, true},
        {false, false, true, true},
        {false, false, false, true},
        {false, false, false, false},
    }};

    for (const auto &stim : stimuli)
    {
        sim.a = stim.a;
        sim.b = stim.b;
        sim.c = stim.c;
        sim.d = stim.d;
        sim.eval();

        const bool expectedOut = (stim.a && stim.b) || (stim.c && stim.d);
        const bool expectedOutN = !expectedOut;
        if (sim.out != expectedOut || sim.out_n != expectedOutN)
        {
            std::cerr << "[GrhTB] dut_009 failed: a=" << static_cast<int>(stim.a)
                      << ", b=" << static_cast<int>(stim.b)
                      << ", c=" << static_cast<int>(stim.c)
                      << ", d=" << static_cast<int>(stim.d)
                      << ", expected out/out_n=" << static_cast<int>(expectedOut)
                      << "/" << static_cast<int>(expectedOutN)
                      << ", got " << static_cast<int>(sim.out)
                      << "/" << static_cast<int>(sim.out_n) << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_009 passed: out == (a&b)|(c&d) and out_n == ~out for all stimuli\n";
    return EXIT_SUCCESS;
}
