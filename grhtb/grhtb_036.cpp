#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus_036 {
    std::uint16_t sc;
    bool l;
    bool d;
    bool r;
    bool u;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<Stimulus_036, 6> stimuli{{
        {0x0000u, false, false, false, false},
        {0xE06Bu, true,  false, false, false},
        {0xE072u, false, true,  false, false},
        {0xE074u, false, false, true,  false},
        {0xE075u, false, false, false, true},
        {0x1234u, false, false, false, false},
    }};

    auto check = [&](std::uint16_t sc, bool l, bool d, bool r, bool u, const char *ctx) {
        sim.scancode = sc;
        sim.eval();
        if (sim.left != l || sim.down != d || sim.right != r || sim.up != u) {
            std::cerr << "[GrhTB] dut_036 failed(" << ctx << "): sc=0x" << std::hex << sc << std::dec
                      << ", expected l/d/r/u=" << static_cast<int>(l)
                      << "/" << static_cast<int>(d)
                      << "/" << static_cast<int>(r)
                      << "/" << static_cast<int>(u)
                      << ", got " << static_cast<int>(sim.left)
                      << "/" << static_cast<int>(sim.down)
                      << "/" << static_cast<int>(sim.right)
                      << "/" << static_cast<int>(sim.up) << '\n';
            std::exit(EXIT_FAILURE);
        }
    };

    for (const auto &stim : stimuli) {
        check(stim.sc, stim.l, stim.d, stim.r, stim.u, "sanity");
    }

    for (int i = 0; i < 16; ++i) {
        for (int rep = 0; rep < 2; ++rep) {
            check(0x0000u, false, false, false, false, "toggle_zero");
            check(static_cast<std::uint16_t>(1u << i), false, false, false, false, "toggle_bit_on");
        }
        check(0x0000u, false, false, false, false, "toggle_bit_off");
    }

    for (int rep = 0; rep < 2; ++rep) {
        check(0x0000u, false, false, false, false, "out_all_zero");
        check(0xE06Bu, true,  false, false, false, "left_on");
        check(0x0000u, false, false, false, false, "left_off");
        check(0xE072u, false, true,  false, false, "down_on");
        check(0x0000u, false, false, false, false, "down_off");
        check(0xE074u, false, false, true,  false, "right_on");
        check(0x0000u, false, false, false, false, "right_off");
        check(0xE075u, false, false, false, true,  "up_on");
        check(0x0000u, false, false, false, false, "up_off");
    }

    std::cout << "[GrhTB] dut_036 passed: scancode mapping ok\n";
    return EXIT_SUCCESS;
}
