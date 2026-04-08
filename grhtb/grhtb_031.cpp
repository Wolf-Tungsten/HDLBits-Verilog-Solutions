#include <array>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus_031 {
    bool a;
    bool b;
    bool s1;
    bool s2;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<Stimulus_031, 9> stimuli{{
        {false, false, false, false},
        {true, false, false, false},
        {true, true, false, false},
        {true, true, true, false},
        {true, true, true, true},
        {false, true, true, true},
        {false, true, false, true},
        {false, true, false, false},
        {false, false, false, false},
    }};

    for (const auto &stim : stimuli) {
        sim.a = stim.a;
        sim.b = stim.b;
        sim.sel_b1 = stim.s1;
        sim.sel_b2 = stim.s2;
        sim.eval();

        const bool expected = (stim.s1 && stim.s2) ? stim.b : stim.a;
        if (sim.out_assign != expected || sim.out_always != expected) {
            std::cerr << "[GrhTB] dut_031 failed: a=" << static_cast<int>(stim.a)
                      << ", b=" << static_cast<int>(stim.b)
                      << ", s1/s2=" << static_cast<int>(stim.s1)
                      << "/" << static_cast<int>(stim.s2)
                      << ", expected=" << static_cast<int>(expected)
                      << ", got out_assign=" << static_cast<int>(sim.out_assign)
                      << ", out_always=" << static_cast<int>(sim.out_always) << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_031 passed: mux behavior matches in both styles\n";
    return EXIT_SUCCESS;
}
