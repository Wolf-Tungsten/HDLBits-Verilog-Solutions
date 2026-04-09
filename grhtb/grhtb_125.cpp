#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus125 {
    bool areset;
    bool in;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.areset = true;
    sim.in = false;
    sim.eval();
    if (sim.out) {
        std::cerr << "[GrhTB] dut_125 failed(async reset immediate)\n";
        return EXIT_FAILURE;
    }

    enum State : std::uint8_t { A = 1, B = 2, C = 3, D = 4 };
    State state = A;
    const std::array<Stimulus125, 10> pattern{{{true, false},
                                               {false, true},
                                               {false, true},
                                               {false, false},
                                               {false, true},
                                               {false, false},
                                               {true, false},
                                               {false, false},
                                               {false, true},
                                               {false, false}}};

    for (const auto &stim : pattern) {
        sim.areset = stim.areset;
        sim.in = stim.in;
        sim.eval();
        if (stim.areset && sim.out) {
            std::cerr << "[GrhTB] dut_125 failed(async level)\n";
            return EXIT_FAILURE;
        }

        sim.clk = true;
        sim.eval();
        if (stim.areset) {
            state = A;
        } else {
            switch (state) {
            case A: state = stim.in ? B : A; break;
            case B: state = stim.in ? B : C; break;
            case C: state = stim.in ? D : A; break;
            case D: state = stim.in ? B : C; break;
            default: break;
            }
        }
        if (sim.out != (state == D)) {
            std::cerr << "[GrhTB] dut_125 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (sim.out != (state == D)) {
            std::cerr << "[GrhTB] dut_125 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_125 passed: 4-state FSM with async reset\n";
    return EXIT_SUCCESS;
}
