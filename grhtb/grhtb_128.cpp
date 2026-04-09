#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus128 {
    bool bumpLeft;
    bool bumpRight;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.areset = true;
    sim.bump_left = false;
    sim.bump_right = false;
    sim.eval();
    if (!sim.walk_left || sim.walk_right) {
        std::cerr << "[GrhTB] dut_128 failed(async reset immediate)\n";
        return EXIT_FAILURE;
    }

    enum State : std::uint8_t { LEFT = 0, RIGHT = 1 };
    State state = LEFT;
    const std::array<Stimulus128, 6> seq{{{false, false},
                                          {true, false},
                                          {false, false},
                                          {false, true},
                                          {true, false},
                                          {false, false}}};

    sim.clk = true;
    sim.eval();
    sim.clk = false;
    sim.eval();
    sim.areset = false;

    for (const auto &stim : seq) {
        sim.bump_left = stim.bumpLeft;
        sim.bump_right = stim.bumpRight;

        sim.clk = true;
        sim.eval();
        state = (state == LEFT) ? (stim.bumpLeft ? RIGHT : LEFT) : (stim.bumpRight ? LEFT : RIGHT);
        if (sim.walk_left != (state == LEFT) || sim.walk_right != (state == RIGHT)) {
            std::cerr << "[GrhTB] dut_128 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (sim.walk_left != (state == LEFT) || sim.walk_right != (state == RIGHT)) {
            std::cerr << "[GrhTB] dut_128 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_128 passed: basic Lemmings left/right FSM\n";
    return EXIT_SUCCESS;
}
