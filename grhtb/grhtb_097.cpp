#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus097 {
    bool reset;
    std::uint32_t in;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.reset = true;
    sim.in = 0u;
    sim.eval();

    const std::array<Stimulus097, 10> stimuli{{{true, 0x00000000u},
                                               {false, 0xFFFFFFFFu},
                                               {false, 0x00000000u},
                                               {true, 0x00000000u},
                                               {false, 0xAAAAAAAAu},
                                               {false, 0x55555555u},
                                               {false, 0x00000000u},
                                               {true, 0x00000000u},
                                               {false, 0x0F0F0F0Fu},
                                               {false, 0x00000000u}}};
    std::uint32_t prevIn = 0u;
    std::uint32_t outModel = 0u;

    for (const auto &stim : stimuli) {
        sim.reset = stim.reset;
        sim.in = stim.in;

        sim.clk = true;
        sim.eval();
        outModel = stim.reset ? 0u : static_cast<std::uint32_t>(outModel | (prevIn & ~stim.in));
        if (static_cast<std::uint32_t>(sim.out) != outModel) {
            std::cerr << "[GrhTB] dut_097 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (static_cast<std::uint32_t>(sim.out) != outModel) {
            std::cerr << "[GrhTB] dut_097 failed(negedge)\n";
            return EXIT_FAILURE;
        }
        prevIn = stim.in;
    }

    std::cout << "[GrhTB] dut_097 passed: 1->0 event latch with reset\n";
    return EXIT_SUCCESS;
}
