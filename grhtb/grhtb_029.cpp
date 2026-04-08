#include <array>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus_029 {
    bool a;
    bool b;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<Stimulus_029, 5> stimuli{{
        {false, false},
        {true, false},
        {true, true},
        {false, true},
        {false, false},
    }};

    for (const auto &stim : stimuli) {
        sim.a = stim.a;
        sim.b = stim.b;
        sim.eval();

        const bool expected = stim.a && stim.b;
        if (sim.out_assign != expected || sim.out_alwaysblock != expected) {
            std::cerr << "[GrhTB] dut_029 failed: a=" << static_cast<int>(stim.a)
                      << ", b=" << static_cast<int>(stim.b)
                      << ", expected both outputs=" << static_cast<int>(expected)
                      << ", got out_assign=" << static_cast<int>(sim.out_assign)
                      << ", out_alwaysblock=" << static_cast<int>(sim.out_alwaysblock) << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_029 passed: out_assign==out_alwaysblock==a&b\n";
    return EXIT_SUCCESS;
}
