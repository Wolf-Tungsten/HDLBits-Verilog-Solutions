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
    sim.in = 0u;
    sim.eval();

    const std::array<std::uint8_t, 7> stimuli{{0x00u, 0xFFu, 0x0Fu, 0xF0u, 0xAAu, 0x55u, 0x00u}};
    std::uint8_t prevIn = 0u;

    for (const std::uint8_t in : stimuli) {
        sim.in = in;
        sim.clk = true;
        sim.eval();
        const std::uint8_t anyedgeModel = static_cast<std::uint8_t>(prevIn ^ in);
        if ((sim.anyedge & 0xFFu) != anyedgeModel) {
            std::cerr << "[GrhTB] dut_096 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if ((sim.anyedge & 0xFFu) != anyedgeModel) {
            std::cerr << "[GrhTB] dut_096 failed(negedge)\n";
            return EXIT_FAILURE;
        }
        prevIn = in;
    }

    std::cout << "[GrhTB] dut_096 passed: per-bit any-edge detector\n";
    return EXIT_SUCCESS;
}
