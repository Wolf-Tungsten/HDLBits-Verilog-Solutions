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

    std::uint8_t d0 = 0u;
    std::uint8_t d1 = 0u;
    std::uint8_t d2 = 0u;

    const int cycles = 1100;
    for (int cycle = 0; cycle < cycles; ++cycle) {
        if (cycle == 1) {
            sim.reset = false;
        }

        sim.clk = true;
        sim.eval();
        if (sim.reset) {
            d0 = d1 = d2 = 0u;
        } else {
            d0 = static_cast<std::uint8_t>((d0 + 1u) % 10u);
            if (d0 == 0u) {
                d1 = static_cast<std::uint8_t>((d1 + 1u) % 10u);
                if (d1 == 0u) {
                    d2 = static_cast<std::uint8_t>((d2 + 1u) % 10u);
                }
            }
        }

        const std::uint8_t cEnableModel = static_cast<std::uint8_t>(((d1 == 9u && d0 == 9u) << 2) | ((d0 == 9u) << 1) | 1u);
        const bool oneHertzModel = (d2 == 9u && d1 == 9u && d0 == 9u);
        if ((sim.c_enable & 0x7u) != cEnableModel || sim.OneHertz != oneHertzModel) {
            std::cerr << "[GrhTB] dut_104 failed(posedge) at cycle " << cycle << '\n';
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if ((sim.c_enable & 0x7u) != cEnableModel || sim.OneHertz != oneHertzModel) {
            std::cerr << "[GrhTB] dut_104 failed(negedge) at cycle " << cycle << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_104 passed: cascaded BCD enables and OneHertz pulse\n";
    return EXIT_SUCCESS;
}
