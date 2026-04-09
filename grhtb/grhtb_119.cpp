#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus119 {
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
    if (!sim.out) {
        std::cerr << "[GrhTB] dut_119 failed(async reset immediate)\n";
        return EXIT_FAILURE;
    }

    enum State : std::uint8_t { A = 0, B = 1 };
    State state = B;
    const std::array<Stimulus119, 8> pattern{{{true, false},
                                              {false, false},
                                              {false, true},
                                              {false, false},
                                              {true, true},
                                              {false, true},
                                              {false, true},
                                              {false, false}}};

    for (const auto &stim : pattern) {
        sim.areset = stim.areset;
        sim.in = stim.in;
        sim.eval();
        if (stim.areset && !sim.out) {
            std::cerr << "[GrhTB] dut_119 failed(async level)\n";
            return EXIT_FAILURE;
        }

        sim.clk = true;
        sim.eval();
        if (stim.areset) {
            state = B;
        } else {
            state = (state == A) ? (stim.in ? A : B) : (stim.in ? B : A);
        }
        const bool outModel = state == B;
        if (sim.out != outModel) {
            std::cerr << "[GrhTB] dut_119 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (sim.out != outModel) {
            std::cerr << "[GrhTB] dut_119 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_119 passed: 2-state FSM with async reset\n";
    return EXIT_SUCCESS;
}
