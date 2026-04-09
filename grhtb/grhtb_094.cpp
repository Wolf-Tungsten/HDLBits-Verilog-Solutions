#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus094 {
    bool j;
    bool k;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.j = false;
    sim.k = false;
    sim.eval();

    const std::array<Stimulus094, 5> stimuli{{{true, false}, {false, false}, {false, true}, {true, true}, {true, true}}};
    bool qModel = false;

    for (const auto &stim : stimuli) {
        sim.j = stim.j;
        sim.k = stim.k;

        sim.clk = true;
        sim.eval();
        const std::uint8_t code = static_cast<std::uint8_t>((static_cast<std::uint8_t>(stim.j) << 1) |
                                                            static_cast<std::uint8_t>(stim.k));
        switch (code) {
        case 0u: break;
        case 1u: qModel = false; break;
        case 2u: qModel = true; break;
        case 3u: qModel = !qModel; break;
        default: return EXIT_FAILURE;
        }
        if (sim.Q != qModel) {
            std::cerr << "[GrhTB] dut_094 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (sim.Q != qModel) {
            std::cerr << "[GrhTB] dut_094 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_094 passed: JK flip-flop behavior\n";
    return EXIT_SUCCESS;
}
