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

        const bool expected = !value;
        if (sim.out != expected)
        {
            std::cerr << "[GrhTB] dut_005 failed: in=" << static_cast<int>(value)
                      << ", expected out=" << static_cast<int>(expected)
                      << ", got " << static_cast<int>(sim.out) << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_005 passed: out is bitwise inversion of in\n";
    return EXIT_SUCCESS;
}
