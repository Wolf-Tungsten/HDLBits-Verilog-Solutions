#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus083 {
    bool reset;
    std::uint8_t d;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.reset = true;
    sim.d = 0u;
    sim.eval();

    const std::array<Stimulus083, 5> stimuli{{{true, 0xAAu}, {false, 0xAAu}, {false, 0x55u}, {true, 0xFFu}, {false, 0x00u}}};
    std::uint8_t qModel = 0u;

    for (const auto &stim : stimuli) {
        sim.reset = stim.reset;
        sim.d = stim.d;

        sim.clk = true;
        sim.eval();
        qModel = stim.reset ? 0u : stim.d;
        if ((sim.q & 0xFFu) != qModel) {
            std::cerr << "[GrhTB] dut_083 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if ((sim.q & 0xFFu) != qModel) {
            std::cerr << "[GrhTB] dut_083 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_083 passed: sync reset 8-bit register\n";
    return EXIT_SUCCESS;
}
