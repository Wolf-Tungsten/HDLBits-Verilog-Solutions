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
    sim.d = 0u;
    sim.eval();

    const std::array<std::uint8_t, 6> stimuli{{0x00u, 0xFFu, 0x00u, 0xAAu, 0x55u, 0x00u}};
    std::uint8_t qModel = 0u;

    for (const std::uint8_t d : stimuli) {
        sim.d = d;
        sim.clk = true;
        sim.eval();
        qModel = d;
        if ((sim.q & 0xFFu) != qModel) {
            std::cerr << "[GrhTB] dut_082 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if ((sim.q & 0xFFu) != qModel) {
            std::cerr << "[GrhTB] dut_082 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_082 passed: 8-bit register\n";
    return EXIT_SUCCESS;
}
