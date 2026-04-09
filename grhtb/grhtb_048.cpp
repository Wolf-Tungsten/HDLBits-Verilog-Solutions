#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static bool reference(bool in1, bool in2, bool in3)
{
    return static_cast<bool>(in3 ^ !(in1 ^ in2));
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    for (int value = 0; value < 8; ++value) {
        const bool in1 = (value & 1) != 0;
        const bool in2 = (value & 2) != 0;
        const bool in3 = (value & 4) != 0;
        sim.in1 = in1;
        sim.in2 = in2;
        sim.in3 = in3;
        sim.eval();
        if (sim.out != reference(in1, in2, in3)) {
            return EXIT_FAILURE;
        }
    }

    for (int rep = 0; rep < 2; ++rep) {
        sim.in1 = false;
        sim.in2 = false;
        sim.in3 = false;
        sim.eval();
        if (sim.out != reference(false, false, false)) {
            return EXIT_FAILURE;
        }
        sim.in1 = true;
        sim.eval();
        if (sim.out != reference(true, false, false)) {
            return EXIT_FAILURE;
        }
        sim.in1 = false;
        sim.eval();
        if (sim.out != reference(false, false, false)) {
            return EXIT_FAILURE;
        }
        sim.in2 = true;
        sim.eval();
        if (sim.out != reference(false, true, false)) {
            return EXIT_FAILURE;
        }
        sim.in2 = false;
        sim.eval();
        if (sim.out != reference(false, false, false)) {
            return EXIT_FAILURE;
        }
        sim.in3 = true;
        sim.eval();
        if (sim.out != reference(false, false, true)) {
            return EXIT_FAILURE;
        }
        sim.in3 = false;
        sim.eval();
        if (sim.out != reference(false, false, false)) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_048 passed: in3 ^ ~(in1 ^ in2)\n";
    return EXIT_SUCCESS;
}
