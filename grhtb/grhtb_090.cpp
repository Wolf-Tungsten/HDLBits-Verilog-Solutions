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
    sim.in = false;
    sim.eval();

    const std::array<bool, 6> stimuli{{false, true, true, false, true, false}};
    bool outModel = false;

    for (const bool in : stimuli) {
        sim.in = in;
        sim.clk = true;
        sim.eval();
        outModel = static_cast<bool>(outModel ^ in);
        if (sim.out != outModel) {
            std::cerr << "[GrhTB] dut_090 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (sim.out != outModel) {
            std::cerr << "[GrhTB] dut_090 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_090 passed: XOR accumulator\n";
    return EXIT_SUCCESS;
}
