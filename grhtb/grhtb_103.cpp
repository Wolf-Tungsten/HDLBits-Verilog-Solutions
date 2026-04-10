#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus103 {
    bool reset;
    bool enable;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.reset = true;
    sim.enable = false;
    sim.eval();

    const std::array<Stimulus103, 17> stimuli{{{true, false},
                                               {false, true}, {false, true}, {false, true}, {false, false},
                                               {false, true}, {false, true}, {false, true}, {false, true},
                                               {false, true}, {false, true}, {false, true}, {false, true},
                                               {false, true}, {true, true},  {false, true}, {false, true}}};
    std::uint8_t qModel = 0u;

    for (const auto &stim : stimuli) {
        sim.reset = stim.reset;
        sim.enable = stim.enable;

        sim.clk = true;
        sim.eval();

        const std::uint8_t qPrev = qModel;
        if (stim.reset || ((qPrev == 12u) && stim.enable)) {
            qModel = 1u;
        } else if (stim.enable) {
            qModel = static_cast<std::uint8_t>((qPrev + 1u) & 0xFu);
        }

        const bool cEnableModel = stim.enable;
        const bool cLoadModel = stim.reset || ((qModel == 12u) && stim.enable);
        const std::uint8_t cDModel = cLoadModel ? 1u : 0u;

        if ((sim.Q & 0xFu) != qModel || sim.c_enable != cEnableModel || sim.c_load != cLoadModel ||
            (sim.c_d & 0xFu) != cDModel) {
            std::cerr << "[GrhTB] dut_103 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if ((sim.Q & 0xFu) != qModel || sim.c_enable != cEnableModel || sim.c_load != cLoadModel ||
            (sim.c_d & 0xFu) != cDModel) {
            std::cerr << "[GrhTB] dut_103 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_103 passed: counter control and outputs\n";
    return EXIT_SUCCESS;
}
