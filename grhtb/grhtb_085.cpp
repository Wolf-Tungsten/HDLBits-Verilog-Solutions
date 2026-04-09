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
    sim.areset = false;
    sim.d = 0u;
    sim.eval();

    std::uint8_t qModel = 0u;

    sim.areset = true;
    sim.eval();
    qModel = 0u;
    if ((sim.q & 0xFFu) != qModel) {
        std::cerr << "[GrhTB] dut_085 failed(initial async reset)\n";
        return EXIT_FAILURE;
    }
    sim.areset = false;
    sim.eval();

    const std::array<std::uint8_t, 4> stimuli{{0x00u, 0xFFu, 0xAAu, 0x55u}};
    for (const std::uint8_t d : stimuli) {
        sim.d = d;
        sim.clk = true;
        sim.eval();
        qModel = d;
        if ((sim.q & 0xFFu) != qModel) {
            std::cerr << "[GrhTB] dut_085 failed(normal posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if ((sim.q & 0xFFu) != qModel) {
            std::cerr << "[GrhTB] dut_085 failed(normal negedge)\n";
            return EXIT_FAILURE;
        }

        sim.areset = true;
        sim.eval();
        qModel = 0u;
        if ((sim.q & 0xFFu) != qModel) {
            std::cerr << "[GrhTB] dut_085 failed(async reset)\n";
            return EXIT_FAILURE;
        }
        sim.areset = false;
        sim.eval();
    }

    sim.d = 0x3Cu;
    sim.areset = true;
    sim.clk = true;
    sim.eval();
    qModel = 0u;
    if ((sim.q & 0xFFu) != qModel) {
        std::cerr << "[GrhTB] dut_085 failed(coincident reset)\n";
        return EXIT_FAILURE;
    }

    std::cout << "[GrhTB] dut_085 passed: async reset 8-bit register\n";
    return EXIT_SUCCESS;
}
