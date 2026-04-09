#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus084 {
    bool reset;
    std::uint8_t d;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = true;
    sim.reset = true;
    sim.d = 0u;
    sim.eval();

    const std::array<Stimulus084, 5> stimuli{{{true, 0x00u}, {false, 0x12u}, {false, 0xA5u}, {true, 0xFFu}, {false, 0x5Au}}};
    std::uint8_t qModel = 0x34u;

    for (const auto &stim : stimuli) {
        sim.reset = stim.reset;
        sim.d = stim.d;

        sim.clk = false;
        sim.eval();
        qModel = stim.reset ? 0x34u : stim.d;
        if ((sim.q & 0xFFu) != qModel) {
            std::cerr << "[GrhTB] dut_084 failed(negedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = true;
        sim.eval();
        if ((sim.q & 0xFFu) != qModel) {
            std::cerr << "[GrhTB] dut_084 failed(posedge_hold)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_084 passed: negedge clock with reset to 0x34\n";
    return EXIT_SUCCESS;
}
