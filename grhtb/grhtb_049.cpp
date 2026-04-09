#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static bool check_outputs(GrhSIM_top_module &sim, bool a, bool b)
{
    sim.a = a;
    sim.b = b;
    sim.eval();

    const bool andValue = a & b;
    const bool orValue = a | b;
    const bool xorValue = a ^ b;
    return sim.out_and == andValue &&
           sim.out_or == orValue &&
           sim.out_xor == xorValue &&
           sim.out_nand == !andValue &&
           sim.out_nor == !orValue &&
           sim.out_xnor == !xorValue &&
           sim.out_anotb == (a & !b);
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    for (int value = 0; value < 4; ++value) {
        const bool a = (value & 1) != 0;
        const bool b = (value & 2) != 0;
        if (!check_outputs(sim, a, b)) {
            return EXIT_FAILURE;
        }
    }

    for (int rep = 0; rep < 2; ++rep) {
        if (!check_outputs(sim, false, false)) {
            return EXIT_FAILURE;
        }
        if (!check_outputs(sim, true, true)) {
            return EXIT_FAILURE;
        }
        if (!check_outputs(sim, false, false)) {
            return EXIT_FAILURE;
        }
        if (!check_outputs(sim, true, false)) {
            return EXIT_FAILURE;
        }
        if (!check_outputs(sim, false, false)) {
            return EXIT_FAILURE;
        }
        if (!check_outputs(sim, false, true)) {
            return EXIT_FAILURE;
        }
        if (!check_outputs(sim, false, false)) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_049 passed: logic reductions suite\n";
    return EXIT_SUCCESS;
}
