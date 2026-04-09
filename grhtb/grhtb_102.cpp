#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus102 {
    bool reset;
    bool slowena;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.reset = true;
    sim.slowena = false;
    sim.eval();

    const std::array<Stimulus102, 25> stimuli{{{true, false},
                                               {false, true},  {false, true},  {false, true},  {false, true},
                                               {false, true},  {false, true},  {false, true},  {false, true},
                                               {false, true},  {false, true},  {false, false}, {false, false},
                                               {false, true},  {false, true},  {false, true},  {false, true},
                                               {false, true},  {false, true},  {false, true},  {false, true},
                                               {false, true},  {false, true},  {true, false},  {false, true}}};
    std::uint8_t qModel = 0u;

    for (const auto &stim : stimuli) {
        sim.reset = stim.reset;
        sim.slowena = stim.slowena;

        sim.clk = true;
        sim.eval();
        if (stim.reset || (stim.slowena && qModel == 9u)) {
            qModel = 0u;
        } else if (stim.slowena) {
            qModel = static_cast<std::uint8_t>((qModel + 1u) & 0xFu);
        }
        if ((sim.q & 0xFu) != qModel) {
            std::cerr << "[GrhTB] dut_102 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if ((sim.q & 0xFu) != qModel) {
            std::cerr << "[GrhTB] dut_102 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_102 passed: gated BCD counter\n";
    return EXIT_SUCCESS;
}
