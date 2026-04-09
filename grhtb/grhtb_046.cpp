#include <array>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<std::array<bool, 3>, 4> truthTable{{
        std::array<bool, 3>{false, false, true},
        std::array<bool, 3>{false, true, false},
        std::array<bool, 3>{true, false, false},
        std::array<bool, 3>{true, true, false},
    }};

    for (const auto &row : truthTable) {
        sim.in1 = row[0];
        sim.in2 = row[1];
        sim.eval();
        if (sim.out != row[2]) {
            return EXIT_FAILURE;
        }
    }

    for (int rep = 0; rep < 2; ++rep) {
        sim.in1 = false;
        sim.in2 = false;
        sim.eval();
        if (sim.out != true) {
            return EXIT_FAILURE;
        }
        sim.in1 = true;
        sim.eval();
        if (sim.out != false) {
            return EXIT_FAILURE;
        }
        sim.in1 = false;
        sim.eval();
        if (sim.out != true) {
            return EXIT_FAILURE;
        }
        sim.in2 = true;
        sim.eval();
        if (sim.out != false) {
            return EXIT_FAILURE;
        }
        sim.in2 = false;
        sim.eval();
        if (sim.out != true) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_046 passed: NOR gate\n";
    return EXIT_SUCCESS;
}
