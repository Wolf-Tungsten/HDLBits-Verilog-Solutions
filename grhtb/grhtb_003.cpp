#include <array>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<bool, 3> stimuli{false, true, false};
    for (bool value : stimuli)
    {
        sim.in = value;
        sim.eval();
        if (sim.out != value)
        {
            std::cerr << "[GrhTB] dut_003 failed: expected out="
                      << static_cast<int>(value) << ", got "
                      << static_cast<int>(sim.out) << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_003 passed: in==out for all stimuli\n";
    return EXIT_SUCCESS;
}
