#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus122 {
    bool reset;
    bool j;
    bool k;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.reset = true;
    sim.j = false;
    sim.k = false;
    sim.eval();

    enum State : std::uint8_t { OFF = 0, ON = 1 };
    State state = OFF;
    const std::array<Stimulus122, 8> pattern{{{true, false, false},
                                              {false, true, false},
                                              {false, false, false},
                                              {false, false, true},
                                              {false, true, true},
                                              {true, false, false},
                                              {false, false, true},
                                              {false, true, false}}};

    for (const auto &stim : pattern) {
        sim.reset = stim.reset;
        sim.j = stim.j;
        sim.k = stim.k;

        sim.clk = true;
        sim.eval();
        if (stim.reset) {
            state = OFF;
        } else {
            state = (state == OFF) ? (stim.j ? ON : OFF) : (stim.k ? OFF : ON);
        }
        if (sim.out != (state == ON)) {
            std::cerr << "[GrhTB] dut_122 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (sim.out != (state == ON)) {
            std::cerr << "[GrhTB] dut_122 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_122 passed: JK latch FSM with sync reset\n";
    return EXIT_SUCCESS;
}
