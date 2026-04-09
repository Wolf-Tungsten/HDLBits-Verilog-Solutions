#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus121 {
    bool areset;
    bool j;
    bool k;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.areset = true;
    sim.j = false;
    sim.k = false;
    sim.eval();
    if (sim.out) {
        std::cerr << "[GrhTB] dut_121 failed(async reset immediate)\n";
        return EXIT_FAILURE;
    }

    enum State : std::uint8_t { OFF = 0, ON = 1 };
    State state = OFF;
    const std::array<Stimulus121, 8> pattern{{{true, false, false},
                                              {false, true, false},
                                              {false, false, false},
                                              {false, false, true},
                                              {false, true, true},
                                              {true, false, false},
                                              {false, false, true},
                                              {false, true, false}}};

    for (const auto &stim : pattern) {
        sim.areset = stim.areset;
        sim.j = stim.j;
        sim.k = stim.k;
        sim.eval();
        if (stim.areset && sim.out) {
            std::cerr << "[GrhTB] dut_121 failed(async level)\n";
            return EXIT_FAILURE;
        }

        sim.clk = true;
        sim.eval();
        if (stim.areset) {
            state = OFF;
        } else {
            state = (state == OFF) ? (stim.j ? ON : OFF) : (stim.k ? OFF : ON);
        }
        if (sim.out != (state == ON)) {
            std::cerr << "[GrhTB] dut_121 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (sim.out != (state == ON)) {
            std::cerr << "[GrhTB] dut_121 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_121 passed: JK latch FSM with async reset\n";
    return EXIT_SUCCESS;
}
