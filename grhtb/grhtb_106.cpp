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
    sim.ena = false;
    sim.eval();

    const int cycles = 60 * 60 * 13;
    for (int cycle = 0; cycle < cycles; ++cycle) {
        if (cycle == 1) {
            sim.reset = false;
        }

        sim.ena = true;
        sim.clk = true;
        sim.eval();

        const std::uint8_t hh = static_cast<std::uint8_t>(sim.hh & 0xFFu);
        const std::uint8_t pm = static_cast<std::uint8_t>(sim.pm & 1u);
        if (!(hh >= 0x01u && hh <= 0x12u) || pm > 1u) {
            std::cerr << "[GrhTB] dut_106 failed(posedge) at cycle " << cycle << '\n';
            return EXIT_FAILURE;
        }
        if (cycle < 2 && (hh != 0x12u || pm != 0u)) {
            std::cerr << "[GrhTB] dut_106 failed(reset state)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        const std::uint8_t hhHold = static_cast<std::uint8_t>(sim.hh & 0xFFu);
        const std::uint8_t pmHold = static_cast<std::uint8_t>(sim.pm & 1u);
        if (hhHold != hh || pmHold != pm) {
            std::cerr << "[GrhTB] dut_106 failed(negedge hold) at cycle " << cycle << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_106 passed: basic 12-hour clock sanity checks\n";
    return EXIT_SUCCESS;
}
