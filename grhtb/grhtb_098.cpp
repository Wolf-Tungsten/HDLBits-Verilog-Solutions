#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.d = false;
    sim.eval();

    const std::array<bool, 4> stimuliPos{{false, true, false, true}};
    const std::array<bool, 4> stimuliNeg{{true, false, true, false}};
    bool qModel = false;

    for (std::size_t i = 0; i < stimuliPos.size(); ++i) {
        sim.d = stimuliPos[i];
        sim.clk = true;
        sim.eval();
        qModel = stimuliPos[i];
        if (sim.q != qModel) {
            std::cerr << "[GrhTB] dut_098 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.d = stimuliNeg[i];
        sim.clk = false;
        sim.eval();
        qModel = stimuliNeg[i];
        if (sim.q != qModel) {
            std::cerr << "[GrhTB] dut_098 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_098 passed: both-edge triggered behavior\n";
    return EXIT_SUCCESS;
}
