#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus087 {
    bool ena;
    bool d;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.ena = false;
    sim.d = false;
    sim.eval();

    const std::array<Stimulus087, 6> stimuli{{{false, false}, {true, true}, {false, false},
                                              {true, false}, {false, true}, {true, true}}};
    bool qModel = false;

    for (const auto &stim : stimuli) {
        sim.ena = stim.ena;
        sim.d = stim.d;
        sim.eval();
        if (stim.ena) {
            qModel = stim.d;
        }

        if (sim.q != qModel) {
            std::cerr << "[GrhTB] dut_087 failed\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_087 passed: level-sensitive latch with enable\n";
    return EXIT_SUCCESS;
}
