#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus129 {
    bool bumpLeft;
    bool bumpRight;
    bool ground;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.areset = true;
    sim.bump_left = false;
    sim.bump_right = false;
    sim.ground = true;
    sim.eval();
    if (!sim.walk_left || sim.walk_right || sim.aaah) {
        std::cerr << "[GrhTB] dut_129 failed(async reset immediate)\n";
        return EXIT_FAILURE;
    }

    enum State : std::uint8_t { WALK_L = 0, WALK_R = 1, FALL_L = 2, FALL_R = 3 };
    State state = WALK_L;
    const std::array<Stimulus129, 9> seq{{{false, false, true},
                                          {true, false, true},
                                          {false, false, false},
                                          {false, false, false},
                                          {false, false, true},
                                          {false, true, true},
                                          {false, false, false},
                                          {false, false, false},
                                          {false, false, true}}};

    sim.clk = true;
    sim.eval();
    sim.clk = false;
    sim.eval();
    sim.areset = false;

    for (const auto &stim : seq) {
        sim.bump_left = stim.bumpLeft;
        sim.bump_right = stim.bumpRight;
        sim.ground = stim.ground;

        sim.clk = true;
        sim.eval();
        switch (state) {
        case WALK_L: state = stim.ground ? (stim.bumpLeft ? WALK_R : WALK_L) : FALL_L; break;
        case WALK_R: state = stim.ground ? (stim.bumpRight ? WALK_L : WALK_R) : FALL_R; break;
        case FALL_L: state = stim.ground ? WALK_L : FALL_L; break;
        case FALL_R: state = stim.ground ? WALK_R : FALL_R; break;
        default: break;
        }

        const bool walkLeft = state == WALK_L;
        const bool walkRight = state == WALK_R;
        const bool aaah = state == FALL_L || state == FALL_R;
        if (sim.walk_left != walkLeft || sim.walk_right != walkRight || sim.aaah != aaah) {
            std::cerr << "[GrhTB] dut_129 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (sim.walk_left != walkLeft || sim.walk_right != walkRight || sim.aaah != aaah) {
            std::cerr << "[GrhTB] dut_129 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_129 passed: Lemmings walk/fall FSM\n";
    return EXIT_SUCCESS;
}
