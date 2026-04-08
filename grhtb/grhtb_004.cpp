#include <array>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus
{
    bool a;
    bool b;
    bool c;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<Stimulus, 9> stimuli{{
        {false, false, false},
        {true, false, false},
        {true, true, false},
        {false, true, false},
        {false, true, true},
        {false, false, true},
        {true, false, false},
        {true, true, false},
        {false, false, false},
    }};

    for (const auto &stim : stimuli)
    {
        sim.a = stim.a;
        sim.b = stim.b;
        sim.c = stim.c;
        sim.eval();

        if (sim.w != stim.a || sim.x != stim.b || sim.y != stim.b ||
            sim.z != stim.c)
        {
            std::cerr << "[GrhTB] dut_004 failed: "
                      << "a=" << static_cast<int>(stim.a) << ", "
                      << "b=" << static_cast<int>(stim.b) << ", "
                      << "c=" << static_cast<int>(stim.c) << " => wxyz="
                      << static_cast<int>(sim.w)
                      << static_cast<int>(sim.x)
                      << static_cast<int>(sim.y)
                      << static_cast<int>(sim.z) << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_004 passed: outputs mirror inputs for all stimuli\n";
    return EXIT_SUCCESS;
}
