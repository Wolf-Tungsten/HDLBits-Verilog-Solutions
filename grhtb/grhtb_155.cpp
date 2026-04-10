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
    sim.eval();

    enum State : std::uint8_t { S0 = 0, S1 = 1, S2 = 2, S3 = 3, S4 = 4 };
    State state = S0;

    auto tick = [&]() {
        sim.clk = true;
        sim.eval();
        sim.clk = false;
        sim.eval();
    };

    auto step = [&](bool reset, const char *ctx) {
        sim.reset = reset;

        State next = state;
        switch (state) {
        case S0: next = reset ? S1 : S0; break;
        case S1: next = S2; break;
        case S2: next = S3; break;
        case S3: next = S4; break;
        case S4: next = S0; break;
        }

        tick();
        state = next;
        if (static_cast<std::uint8_t>(sim.shift_ena) != static_cast<std::uint8_t>(state != S0)) {
            std::cerr << "[GrhTB] dut_155 failed(" << ctx << ")\n";
            return false;
        }
        return true;
    };

    if (!step(true, "S0_to_S1") || !step(false, "S1_to_S2") || !step(false, "S2_to_S3") || !step(false, "S3_to_S4") ||
        !step(false, "S4_to_S0")) {
        return EXIT_FAILURE;
    }
    for (int i = 0; i < 8; ++i) {
        if (!step(false, "cycle")) {
            return EXIT_FAILURE;
        }
    }
    if (!step(true, "pulse_reset") || !step(false, "after_reset")) {
        return EXIT_FAILURE;
    }

    std::cout << "[GrhTB] dut_155 passed: 5-state enable sequencer\n";
    return EXIT_SUCCESS;
}
