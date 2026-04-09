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

    std::uint16_t qModel = 0u;
    std::uint8_t enaModel = 0u;

    const int cycles = 12000;
    for (int cycle = 0; cycle < cycles; ++cycle) {
        if (cycle == 1) {
            sim.reset = false;
        }

        sim.clk = true;
        sim.eval();

        if (sim.reset) {
            qModel = 0u;
            enaModel = 0u;
        } else {
            std::uint8_t d0 = static_cast<std::uint8_t>(qModel & 0xFu);
            std::uint8_t d1 = static_cast<std::uint8_t>((qModel >> 4) & 0xFu);
            std::uint8_t d2 = static_cast<std::uint8_t>((qModel >> 8) & 0xFu);
            std::uint8_t d3 = static_cast<std::uint8_t>((qModel >> 12) & 0xFu);

            d0++;
            if (d0 == 10u) {
                d0 = 0u;
                d1++;
            }
            if (d1 == 10u) {
                d1 = 0u;
                d2++;
            }
            if (d2 == 10u) {
                d2 = 0u;
                d3++;
            }
            if (d3 == 10u) {
                d3 = d2 = d1 = d0 = 0u;
            }

            qModel = static_cast<std::uint16_t>(d0 | (d1 << 4) | (d2 << 8) | (d3 << 12));
            const std::uint8_t e1 = d0 == 9u ? 1u : 0u;
            const std::uint8_t e2 = (d0 == 9u && d1 == 9u) ? 1u : 0u;
            const std::uint8_t e3 = (d0 == 9u && d1 == 9u && d2 == 9u) ? 1u : 0u;
            enaModel = static_cast<std::uint8_t>((e3 << 2) | (e2 << 1) | e1);
        }

        if ((sim.q & 0xFFFFu) != qModel || (sim.ena & 0x7u) != enaModel) {
            std::cerr << "[GrhTB] dut_105 failed(posedge) at cycle " << cycle << '\n';
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if ((sim.q & 0xFFFFu) != qModel || (sim.ena & 0x7u) != enaModel) {
            std::cerr << "[GrhTB] dut_105 failed(negedge) at cycle " << cycle << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_105 passed: 4-digit BCD counter with enables\n";
    return EXIT_SUCCESS;
}
