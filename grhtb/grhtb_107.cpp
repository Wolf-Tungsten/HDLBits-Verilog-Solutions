#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus107 {
    bool areset;
    bool load;
    bool ena;
    std::uint8_t data;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.areset = true;
    sim.load = false;
    sim.ena = false;
    sim.data = 0u;
    sim.eval();

    const std::array<Stimulus107, 8> stimuli{{{true, false, false, 0x0u},
                                              {false, true, false, 0xAu},
                                              {false, false, true, 0x0u},
                                              {false, false, true, 0x0u},
                                              {false, false, false, 0x0u},
                                              {true, false, false, 0x0u},
                                              {false, true, false, 0x5u},
                                              {false, false, true, 0x0u}}};
    std::uint8_t qModel = 0u;

    for (const auto &stim : stimuli) {
        sim.areset = stim.areset;
        sim.load = stim.load;
        sim.ena = stim.ena;
        sim.data = stim.data;

        sim.clk = true;
        sim.eval();
        if (stim.areset) {
            qModel = 0u;
        } else if (stim.load) {
            qModel = static_cast<std::uint8_t>(stim.data & 0xFu);
        } else if (stim.ena) {
            qModel = static_cast<std::uint8_t>((qModel >> 1) & 0x7u);
        }
        if ((sim.q & 0xFu) != qModel) {
            std::cerr << "[GrhTB] dut_107 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if ((sim.q & 0xFu) != qModel) {
            std::cerr << "[GrhTB] dut_107 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_107 passed: loadable right shift register with async reset\n";
    return EXIT_SUCCESS;
}
