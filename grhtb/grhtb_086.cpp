#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus086 {
    bool resetn;
    std::uint16_t d;
    std::uint8_t byteena;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.resetn = false;
    sim.byteena = 0u;
    sim.d = 0u;
    sim.eval();

    const std::array<Stimulus086, 8> stimuli{{{false, 0xAAAAu, 0x3u},
                                              {true, 0x1234u, 0x1u},
                                              {true, 0xDEADu, 0x2u},
                                              {true, 0xBEEFu, 0x3u},
                                              {true, 0x0000u, 0x0u},
                                              {false, 0xFFFFu, 0x3u},
                                              {true, 0x00FFu, 0x1u},
                                              {true, 0xFF00u, 0x2u}}};
    std::uint16_t qModel = 0u;

    for (const auto &stim : stimuli) {
        sim.resetn = stim.resetn;
        sim.byteena = stim.byteena & 0x3u;
        sim.d = stim.d;

        sim.clk = true;
        sim.eval();
        if (!stim.resetn) {
            qModel = 0u;
        } else {
            if (stim.byteena & 0x1u) {
                qModel = static_cast<std::uint16_t>((qModel & 0xFF00u) | (stim.d & 0x00FFu));
            }
            if (stim.byteena & 0x2u) {
                qModel = static_cast<std::uint16_t>((qModel & 0x00FFu) | (stim.d & 0xFF00u));
            }
        }

        if ((sim.q & 0xFFFFu) != qModel) {
            std::cerr << "[GrhTB] dut_086 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if ((sim.q & 0xFFFFu) != qModel) {
            std::cerr << "[GrhTB] dut_086 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_086 passed: byte-enable 16-bit register\n";
    return EXIT_SUCCESS;
}
