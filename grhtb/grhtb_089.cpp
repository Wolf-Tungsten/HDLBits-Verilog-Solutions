#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus089 {
    bool r;
    bool d;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.r = false;
    sim.d = false;
    sim.eval();

    const std::array<Stimulus089, 5> stimuli{{{true, true}, {false, true}, {false, false}, {true, false}, {false, true}}};
    bool qModel = false;

    for (const auto &stim : stimuli) {
        sim.r = stim.r;
        sim.d = stim.d;

        sim.clk = true;
        sim.eval();
        qModel = stim.r ? false : stim.d;
        if (sim.q != qModel) {
            std::cerr << "[GrhTB] dut_089 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (sim.q != qModel) {
            std::cerr << "[GrhTB] dut_089 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_089 passed: sync reset DFF\n";
    return EXIT_SUCCESS;
}
