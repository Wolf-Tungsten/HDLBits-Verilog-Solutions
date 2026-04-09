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

    const std::array<bool, 7> stimuli{{false, true, true, false, false, true, false}};
    bool qModel = false;

    for (const bool d : stimuli) {
        sim.d = d;
        sim.clk = true;
        sim.eval();
        qModel = d;
        if (sim.q != qModel) {
            std::cerr << "[GrhTB] dut_081 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (sim.q != qModel) {
            std::cerr << "[GrhTB] dut_081 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_081 passed: simple D flip-flop\n";
    return EXIT_SUCCESS;
}
