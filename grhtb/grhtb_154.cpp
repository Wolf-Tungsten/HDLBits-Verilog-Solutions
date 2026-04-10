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
    sim.data = false;
    sim.eval();

    enum State : std::uint8_t { IDLE = 0, S1 = 1, S11 = 2, S110 = 3, S1101 = 4 };
    State state = IDLE;

    auto tick = [&]() {
        sim.clk = true;
        sim.eval();
        sim.clk = false;
        sim.eval();
    };

    auto apply_reset = [&]() {
        sim.reset = true;
        sim.data = false;
        tick();
        sim.reset = false;
        state = IDLE;
    };

    auto step = [&](bool data, const char *ctx) {
        sim.data = data;

        State next = state;
        switch (state) {
        case IDLE: next = data ? S1 : IDLE; break;
        case S1: next = data ? S11 : IDLE; break;
        case S11: next = data ? S11 : S110; break;
        case S110: next = data ? S1101 : IDLE; break;
        case S1101: next = S1101; break;
        }

        tick();
        state = next;
        if (static_cast<std::uint8_t>(sim.start_shifting) != static_cast<std::uint8_t>(state == S1101)) {
            std::cerr << "[GrhTB] dut_154 failed(" << ctx << ")\n";
            return false;
        }
        return true;
    };

    apply_reset();
    if (!step(true, "1") || !step(true, "11") || !step(false, "110") || !step(true, "1101")) {
        return EXIT_FAILURE;
    }
    for (int i = 0; i < 3; ++i) {
        if (!step(true, "hold")) {
            return EXIT_FAILURE;
        }
    }

    apply_reset();
    const std::uint8_t seq[] = {0u, 1u, 1u, 0u, 1u, 1u, 1u, 0u, 1u, 0u, 0u, 1u};
    for (const std::uint8_t bit : seq) {
        if (!step(bit != 0u, "mixed")) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_154 passed: 1101 sequence detector with latch\n";
    return EXIT_SUCCESS;
}
