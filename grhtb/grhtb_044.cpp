#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    for (int rep = 0; rep < 2; ++rep) {
        sim.in = false;
        sim.eval();
        if (sim.out != false) {
            return EXIT_FAILURE;
        }
        sim.in = true;
        sim.eval();
        if (sim.out != true) {
            return EXIT_FAILURE;
        }
    }

    sim.in = false;
    sim.eval();
    if (sim.out != false) {
        return EXIT_FAILURE;
    }

    std::cout << "[GrhTB] dut_044 passed: wire-through\n";
    return EXIT_SUCCESS;
}
