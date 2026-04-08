#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.eval();

    if (sim.zero)
    {
        std::cerr << "[GrhTB] dut_002 failed: expected zero=0, got "
                  << static_cast<int>(sim.zero) << '\n';
        return EXIT_FAILURE;
    }

    std::cout << "[GrhTB] dut_002 passed: zero=" << static_cast<int>(sim.zero) << '\n';
    return EXIT_SUCCESS;
}
