#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus_030 {
    bool a;
    bool b;
};

static void tick(GrhSIM_top_module &sim)
{
    sim.clk = true;
    sim.eval();
    sim.clk = false;
    sim.eval();
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<Stimulus_030, 5> stimuli{{
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

        const bool comb = stim.a ^ stim.b;
        if (sim.out_assign != comb || sim.out_always_comb != comb) {
            std::cerr << "[GrhTB] dut_030 comb mismatch: a=" << static_cast<int>(stim.a)
                      << ", b=" << static_cast<int>(stim.b)
                      << ", expected comb=" << static_cast<int>(comb)
                      << ", got out_assign=" << static_cast<int>(sim.out_assign)
                      << ", out_always_comb=" << static_cast<int>(sim.out_always_comb) << '\n';
            return EXIT_FAILURE;
        }

        tick(sim);

        if (sim.out_always_ff != comb) {
            std::cerr << "[GrhTB] dut_030 ff mismatch: a=" << static_cast<int>(stim.a)
                      << ", b=" << static_cast<int>(stim.b)
                      << ", expected ff=" << static_cast<int>(comb)
                      << ", got " << static_cast<int>(sim.out_always_ff) << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_030 passed: comb and ff XOR outputs correct across edges\n";
    return EXIT_SUCCESS;
}
