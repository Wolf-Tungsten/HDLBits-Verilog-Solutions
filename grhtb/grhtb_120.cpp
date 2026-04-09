#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus120 {
    bool reset;
    bool in;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.reset = true;
    sim.in = false;
    sim.eval();

    enum State : std::uint8_t { A = 0, B = 1 };
    State state = B;
    const std::array<Stimulus120, 8> pattern{{{true, false},
                                              {false, false},
                                              {false, true},
                                              {false, true},
                                              {false, false},
                                              {true, true},
                                              {false, true},
                                              {false, false}}};

    for (const auto &stim : pattern) {
        sim.reset = stim.reset;
        sim.in = stim.in;

        sim.clk = true;
        sim.eval();
        if (stim.reset) {
            state = B;
        } else {
            state = (state == B) ? (stim.in ? B : A) : (stim.in ? A : B);
        }
        const bool outModel = state == B;
        if (sim.out != outModel) {
            std::cerr << "[GrhTB] dut_120 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (sim.out != outModel) {
            std::cerr << "[GrhTB] dut_120 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_120 passed: 2-state FSM with sync reset\n";
    return EXIT_SUCCESS;
}
