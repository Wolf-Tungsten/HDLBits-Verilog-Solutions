#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus091 {
    bool L;
    bool r_in;
    bool q_in;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.L = false;
    sim.r_in = false;
    sim.q_in = false;
    sim.eval();

    const std::array<Stimulus091, 6> stimuli{{{true, true, false},
                                              {false, false, false},
                                              {false, false, true},
                                              {true, false, true},
                                              {false, true, true},
                                              {false, true, false}}};
    bool qModel = false;

    for (const auto &stim : stimuli) {
        sim.L = stim.L;
        sim.r_in = stim.r_in;
        sim.q_in = stim.q_in;

        sim.clk = true;
        sim.eval();
        qModel = stim.L ? stim.r_in : stim.q_in;
        if (sim.Q != qModel) {
            std::cerr << "[GrhTB] dut_091 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (sim.Q != qModel) {
            std::cerr << "[GrhTB] dut_091 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_091 passed: muxed DFF with load\n";
    return EXIT_SUCCESS;
}
