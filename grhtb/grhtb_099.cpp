#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.reset = true;
    sim.eval();

    std::array<bool, 26> stimuli{};
    stimuli.fill(false);
    stimuli[0] = true;
    stimuli[21] = true;

    std::uint8_t qModel = 0u;
    for (const bool reset : stimuli) {
        sim.reset = reset;
        sim.clk = true;
        sim.eval();
        qModel = reset ? 0u : static_cast<std::uint8_t>((qModel + 1u) & 0xFu);
        if ((sim.q & 0xFu) != qModel) {
            std::cerr << "[GrhTB] dut_099 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if ((sim.q & 0xFu) != qModel) {
            std::cerr << "[GrhTB] dut_099 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_099 passed: 4-bit counter with reset\n";
    return EXIT_SUCCESS;
}
