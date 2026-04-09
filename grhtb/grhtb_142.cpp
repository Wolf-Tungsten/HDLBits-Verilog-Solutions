#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.reset = false;
    sim.s = false;
    sim.w = false;
    sim.eval();

    enum State : std::uint8_t { A = 0, B = 1 };
    State state = A;
    std::uint8_t count = 0u;
    std::uint8_t count1 = 0u;

    auto tick = [&]() {
        sim.clk = true;
        sim.eval();
        sim.clk = false;
        sim.eval();
    };

    auto step = [&](bool reset, bool s, bool w, const char *ctx) {
        sim.reset = reset;
        sim.s = s;
        sim.w = w;

        if (reset) {
            state = A;
            count = 0u;
            count1 = 0u;
        } else {
            State next = (state == A) ? (s ? B : A) : B;
            if (state == B) {
                if (count1 == 3u) {
                    count = 0u;
                    count1 = 0u;
                }
                if (w) {
                    count = static_cast<std::uint8_t>((count + 1u) & 0x3u);
                }
                count1 = static_cast<std::uint8_t>((count1 + 1u) & 0x3u);
            }
            state = next;
        }

        tick();
        const bool z = count == 2u && count1 == 3u;
        if (sim.z != z) {
            std::cerr << "[GrhTB] dut_142 failed(" << ctx << ")\n";
            return false;
        }
        return true;
    };

    if (!step(true, false, false, "reset")) {
        return EXIT_FAILURE;
    }
    for (int i = 0; i < 3; ++i) {
        if (!step(false, false, false, "stay_A")) {
            return EXIT_FAILURE;
        }
    }
    for (int i = 0; i < 12; ++i) {
        if (!step(false, true, (i % 3) != 0, "run_B")) {
            return EXIT_FAILURE;
        }
    }
    if (!step(true, false, false, "reset2")) {
        return EXIT_FAILURE;
    }
    for (int i = 0; i < 8; ++i) {
        if (!step(false, true, (i & 1) != 0, "run_B_alt")) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_142 passed: counter-based FSM\n";
    return EXIT_SUCCESS;
}
