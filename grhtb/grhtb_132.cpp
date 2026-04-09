#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    for (int stateBit = 0; stateBit < 10; ++stateBit) {
        const std::uint16_t state = static_cast<std::uint16_t>(1u << stateBit);
        sim.state = 0u;
        sim.in = false;
        sim.eval();
        sim.state = state;
        sim.eval();
        sim.state = 0u;
        sim.eval();

        for (int rep = 0; rep < 2; ++rep) {
            for (const bool in : {false, true, false}) {
                sim.state = state;
                sim.in = in;
                sim.eval();
                const bool out1 = (stateBit == 8) || (stateBit == 9);
                const bool out2 = (stateBit == 7) || (stateBit == 9);
                if (sim.out1 != out1 || sim.out2 != out2) {
                    std::cerr << "[GrhTB] dut_132 failed(state bit=" << stateBit
                              << ", in=" << static_cast<int>(in) << ")\n";
                    return EXIT_FAILURE;
                }
            }
        }
    }

    std::cout << "[GrhTB] dut_132 passed: next-state and outputs combinational block\n";
    return EXIT_SUCCESS;
}
