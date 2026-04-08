#include <array>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus_010
{
    bool p1a;
    bool p1b;
    bool p1c;
    bool p1d;
    bool p1e;
    bool p1f;
    bool p2a;
    bool p2b;
    bool p2c;
    bool p2d;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<Stimulus_010, 23> stimuli{{
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
        {1, 1, 1, 1, 1, 1, 0, 1, 0, 0},
        {1, 1, 1, 1, 1, 1, 0, 1, 1, 0},
        {1, 1, 1, 1, 1, 1, 0, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 0, 1, 0, 1},
        {1, 1, 1, 1, 1, 1, 0, 1, 0, 0},
        {1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
        {1, 0, 1, 1, 1, 1, 0, 0, 0, 0},
        {1, 0, 0, 1, 1, 1, 0, 0, 0, 0},
        {1, 0, 0, 0, 1, 1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    }};

    for (const auto &stim : stimuli)
    {
        sim.p1a = stim.p1a;
        sim.p1b = stim.p1b;
        sim.p1c = stim.p1c;
        sim.p1d = stim.p1d;
        sim.p1e = stim.p1e;
        sim.p1f = stim.p1f;
        sim.p2a = stim.p2a;
        sim.p2b = stim.p2b;
        sim.p2c = stim.p2c;
        sim.p2d = stim.p2d;
        sim.eval();

        const bool w1 = stim.p2a && stim.p2b;
        const bool w2 = stim.p2c && stim.p2d;
        const bool w3 = stim.p1a && stim.p1b && stim.p1c;
        const bool w4 = stim.p1d && stim.p1e && stim.p1f;
        const bool expectedP1Y = w3 || w4;
        const bool expectedP2Y = w1 || w2;

        if (sim.p1y != expectedP1Y || sim.p2y != expectedP2Y)
        {
            std::cerr << "[GrhTB] dut_010 failed: expected p1y/p2y="
                      << static_cast<int>(expectedP1Y) << "/" << static_cast<int>(expectedP2Y)
                      << ", got " << static_cast<int>(sim.p1y)
                      << "/" << static_cast<int>(sim.p2y) << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_010 passed: p1y == (p1a&p1b&p1c)|(p1d&p1e&p1f) and p2y == (p2a&p2b)|(p2c&p2d)\n";
    return EXIT_SUCCESS;
}
