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
    sim.ar = false;
    sim.d = false;
    sim.eval();

    bool qModel = false;

    sim.ar = true;
    sim.eval();
    if (sim.q != qModel) {
        std::cerr << "[GrhTB] dut_088 failed(initial async reset)\n";
        return EXIT_FAILURE;
    }
    sim.ar = false;
    sim.eval();

    const std::array<bool, 6> stimuli{{false, true, false, true, true, false}};
    for (const bool d : stimuli) {
        sim.d = d;
        sim.clk = true;
        sim.eval();
        qModel = d;
        if (sim.q != qModel) {
            std::cerr << "[GrhTB] dut_088 failed(normal posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (sim.q != qModel) {
            std::cerr << "[GrhTB] dut_088 failed(normal negedge)\n";
            return EXIT_FAILURE;
        }

        sim.ar = true;
        sim.eval();
        qModel = false;
        if (sim.q != qModel) {
            std::cerr << "[GrhTB] dut_088 failed(async reset)\n";
            return EXIT_FAILURE;
        }
        sim.ar = false;
        sim.eval();
    }

    sim.d = true;
    sim.ar = true;
    sim.clk = true;
    sim.eval();
    if (sim.q != false) {
        std::cerr << "[GrhTB] dut_088 failed(coincident reset)\n";
        return EXIT_FAILURE;
    }

    std::cout << "[GrhTB] dut_088 passed: async reset DFF\n";
    return EXIT_SUCCESS;
}
