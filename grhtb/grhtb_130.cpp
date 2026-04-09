#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus130 {
    bool bumpLeft;
    bool bumpRight;
    bool ground;
    bool dig;
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
    sim.dig = false;
    sim.eval();
    if (!sim.walk_left || sim.walk_right || sim.aaah || sim.digging) {
        std::cerr << "[GrhTB] dut_130 failed(async reset immediate)\n";
        return EXIT_FAILURE;
    }

    enum State : std::uint8_t { WALK_L = 0, WALK_R = 1, FALL_L = 2, FALL_R = 3, DIG_L = 4, DIG_R = 5 };
    State state = WALK_L;
    const std::array<Stimulus130, 11> seq{{{false, false, true, false},
                                           {true, false, true, false},
                                           {false, false, true, true},
                                           {false, false, true, true},
                                           {false, false, false, true},
                                           {false, false, false, false},
                                           {false, false, true, false},
                                           {false, true, true, false},
                                           {false, false, true, true},
                                           {false, false, false, true},
                                           {false, false, true, false}}};

    sim.clk = true;
    sim.eval();
    sim.clk = false;
    sim.eval();
    sim.areset = false;

    for (const auto &stim : seq) {
        sim.bump_left = stim.bumpLeft;
        sim.bump_right = stim.bumpRight;
        sim.ground = stim.ground;
        sim.dig = stim.dig;

        sim.clk = true;
        sim.eval();
        switch (state) {
        case WALK_L:
            state = !stim.ground ? FALL_L : (stim.dig ? DIG_L : (stim.bumpLeft ? WALK_R : WALK_L));
            break;
        case WALK_R:
            state = !stim.ground ? FALL_R : (stim.dig ? DIG_R : (stim.bumpRight ? WALK_L : WALK_R));
            break;
        case FALL_L: state = stim.ground ? WALK_L : FALL_L; break;
        case FALL_R: state = stim.ground ? WALK_R : FALL_R; break;
        case DIG_L: state = stim.ground ? DIG_L : FALL_L; break;
        case DIG_R: state = stim.ground ? DIG_R : FALL_R; break;
        default: break;
        }

        const bool walkLeft = state == WALK_L;
        const bool walkRight = state == WALK_R;
        const bool aaah = state == FALL_L || state == FALL_R;
        const bool digging = state == DIG_L || state == DIG_R;
        if (sim.walk_left != walkLeft || sim.walk_right != walkRight || sim.aaah != aaah || sim.digging != digging) {
            std::cerr << "[GrhTB] dut_130 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (sim.walk_left != walkLeft || sim.walk_right != walkRight || sim.aaah != aaah || sim.digging != digging) {
            std::cerr << "[GrhTB] dut_130 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_130 passed: extended Lemmings walk/fall/dig FSM\n";
    return EXIT_SUCCESS;
}
