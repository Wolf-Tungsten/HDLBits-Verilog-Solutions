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
    sim.done_counting = false;
    sim.ack = false;
    sim.eval();

    enum State : std::uint8_t {
        IDLE = 0,
        S1 = 1,
        S11 = 2,
        S110 = 3,
        S1101 = 4,
        SHIFT1 = 5,
        SHIFT2 = 6,
        SHIFT3 = 7,
        COUNT = 8,
        DONE = 9
    };
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
        sim.done_counting = false;
        sim.ack = false;
        tick();
        sim.reset = false;
        state = IDLE;
    };

    auto step = [&](bool data, bool doneCounting, bool ack, const char *ctx) {
        sim.data = data;
        sim.done_counting = doneCounting;
        sim.ack = ack;

        State next = state;
        switch (state) {
        case IDLE: next = data ? S1 : IDLE; break;
        case S1: next = data ? S11 : IDLE; break;
        case S11: next = data ? S11 : S110; break;
        case S110: next = data ? S1101 : IDLE; break;
        case S1101: next = SHIFT1; break;
        case SHIFT1: next = SHIFT2; break;
        case SHIFT2: next = SHIFT3; break;
        case SHIFT3: next = COUNT; break;
        case COUNT: next = doneCounting ? DONE : COUNT; break;
        case DONE: next = ack ? IDLE : DONE; break;
        }

        tick();
        state = next;

        const std::uint8_t shiftExp =
            static_cast<std::uint8_t>(state == S1101 || state == SHIFT1 || state == SHIFT2 || state == SHIFT3);
        const std::uint8_t countingExp = static_cast<std::uint8_t>(state == COUNT);
        const std::uint8_t doneExp = static_cast<std::uint8_t>(state == DONE);
        if (static_cast<std::uint8_t>(sim.shift_ena) != shiftExp || static_cast<std::uint8_t>(sim.counting) != countingExp ||
            static_cast<std::uint8_t>(sim.done) != doneExp) {
            std::cerr << "[GrhTB] dut_156 failed(" << ctx << ")\n";
            return false;
        }
        return true;
    };

    apply_reset();
    if (!step(true, false, false, "seq_1") || !step(true, false, false, "seq_11") || !step(false, false, false, "seq_110") ||
        !step(true, false, false, "seq_1101") || !step(false, false, false, "SHIFT1") || !step(false, false, false, "SHIFT2") ||
        !step(false, false, false, "SHIFT3") || !step(false, false, false, "COUNT_start")) {
        return EXIT_FAILURE;
    }
    for (int i = 0; i < 3; ++i) {
        if (!step(false, false, false, "COUNT_hold")) {
            return EXIT_FAILURE;
        }
    }
    if (!step(false, true, false, "COUNT_done") || !step(false, false, false, "DONE_hold") ||
        !step(false, false, true, "DONE_ack")) {
        return EXIT_FAILURE;
    }

    apply_reset();
    const std::uint8_t seq2[] = {1u, 0u, 1u, 0u, 0u, 1u};
    for (const std::uint8_t bit : seq2) {
        if (!step(bit != 0u, false, false, "partial")) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_156 passed: S1101/shift/count/done FSM\n";
    return EXIT_SUCCESS;
}
