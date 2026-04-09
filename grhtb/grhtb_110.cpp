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

    std::array<bool, 17> stimuli{};
    stimuli.fill(false);
    stimuli[0] = true;

    std::uint8_t qModel = 0x1u;
    for (const bool reset : stimuli) {
        sim.reset = reset;

        sim.clk = true;
        sim.eval();
        if (reset) {
            qModel = 0x1u;
        } else {
            const std::uint8_t q0 = qModel & 1u;
            const std::uint8_t q1 = (qModel >> 1) & 1u;
            const std::uint8_t q2 = (qModel >> 2) & 1u;
            const std::uint8_t q3 = (qModel >> 3) & 1u;
            const std::uint8_t q4 = (qModel >> 4) & 1u;
            qModel = static_cast<std::uint8_t>((q0 << 4) | (q4 << 3) | ((q3 ^ q0) << 2) | (q2 << 1) | q1);
        }
        if ((sim.q & 0x1Fu) != qModel) {
            std::cerr << "[GrhTB] dut_110 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if ((sim.q & 0x1Fu) != qModel) {
            std::cerr << "[GrhTB] dut_110 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_110 passed: 5-bit LFSR sequence with reset\n";
    return EXIT_SUCCESS;
}
