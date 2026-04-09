#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus092 {
    bool L;
    bool R;
    bool E;
    bool w;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.L = false;
    sim.R = false;
    sim.E = false;
    sim.w = false;
    sim.eval();

    const std::array<Stimulus092, 6> stimuli{{{true, true, false, false},
                                              {true, false, true, true},
                                              {false, false, true, true},
                                              {false, false, true, false},
                                              {false, false, false, true},
                                              {false, false, false, false}}};
    bool qModel = false;

    for (const auto &stim : stimuli) {
        sim.L = stim.L;
        sim.R = stim.R;
        sim.E = stim.E;
        sim.w = stim.w;

        sim.clk = true;
        sim.eval();
        if (stim.L) {
            qModel = stim.R;
        } else if (stim.E) {
            qModel = stim.w;
        }
        if (sim.Q != qModel) {
            std::cerr << "[GrhTB] dut_092 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (sim.Q != qModel) {
            std::cerr << "[GrhTB] dut_092 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_092 passed: load/enable DFF\n";
    return EXIT_SUCCESS;
}
