#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.eval();

    if (!sim.one)
    {
        std::cerr << "[GrhTB] dut_001 failed: expected one=1, got "
                  << static_cast<int>(sim.one) << '\n';
        return EXIT_FAILURE;
    }

    std::cout << "[GrhTB] dut_001 passed: one=" << static_cast<int>(sim.one) << '\n';
    return EXIT_SUCCESS;
}
