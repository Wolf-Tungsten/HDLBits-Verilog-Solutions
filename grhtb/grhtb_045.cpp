#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    for (int rep = 0; rep < 8; ++rep) {
        sim.eval();
        if (sim.out != false) {
            std::cerr << "[GrhTB] dut_045 failed: expected out=0, got " << static_cast<int>(sim.out) << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_045 passed: constant zero output verified\n";
    return EXIT_SUCCESS;
}
