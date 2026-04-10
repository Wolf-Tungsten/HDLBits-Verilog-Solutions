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
    std::uint8_t countModel = 0u;
    std::uint16_t count1000Model = 0u;

    auto tick = [&]() {
        sim.clk = true;
        sim.eval();
        sim.clk = false;
        sim.eval();
    };

    auto apply_reset = [&]() {
        sim.reset = true;
        sim.data = false;
        sim.ack = false;
        tick();
        sim.reset = false;
        state = IDLE;
        count1000Model = 0u;
    };

    auto step = [&](bool data, bool ack, const char *ctx) {
        sim.data = data;
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
        case COUNT: next = (countModel == 0u && count1000Model == 999u) ? DONE : COUNT; break;
        case DONE: next = ack ? IDLE : DONE; break;
        }

        std::uint8_t nextCount = countModel;
        std::uint16_t nextCount1000 = count1000Model;
        switch (state) {
        case S1101:
            nextCount = static_cast<std::uint8_t>((nextCount & 0x7u) | ((data ? 1u : 0u) << 3));
            break;
        case SHIFT1:
            nextCount = static_cast<std::uint8_t>((nextCount & 0xBu) | ((data ? 1u : 0u) << 2));
            break;
        case SHIFT2:
            nextCount = static_cast<std::uint8_t>((nextCount & 0xDu) | ((data ? 1u : 0u) << 1));
            break;
        case SHIFT3:
            nextCount = static_cast<std::uint8_t>((nextCount & 0xEu) | (data ? 1u : 0u));
            break;
        case COUNT:
            if (nextCount1000 < 999u) {
                ++nextCount1000;
            } else {
                nextCount = static_cast<std::uint8_t>((nextCount - 1u) & 0xFu);
                nextCount1000 = 0u;
            }
            break;
        default: nextCount1000 = 0u; break;
        }

        tick();
        state = next;
        countModel = nextCount;
        count1000Model = nextCount1000;

        const std::uint8_t countingExp = static_cast<std::uint8_t>(state == COUNT);
        const std::uint8_t doneExp = static_cast<std::uint8_t>(state == DONE);
        if ((sim.count & 0xFu) != countModel || static_cast<std::uint8_t>(sim.counting) != countingExp ||
            static_cast<std::uint8_t>(sim.done) != doneExp) {
            std::cerr << "[GrhTB] dut_157 failed(" << ctx << ")\n";
            return false;
        }
        return true;
    };

    apply_reset();
    if (!step(true, false, "S1_1") || !step(true, false, "S1_11") || !step(false, false, "S1_110") ||
        !step(true, false, "S1_1101") || !step(true, false, "shift_bit3") || !step(false, false, "shift_bit2") ||
        !step(true, false, "shift_bit1")) {
        return EXIT_FAILURE;
    }

    bool sawDone = false;
    for (int i = 0; i < 12000 && !sawDone; ++i) {
        if (!step(false, false, "COUNT_run")) {
            return EXIT_FAILURE;
        }
        sawDone = sim.done;
    }
    if (!sawDone) {
        std::cerr << "[GrhTB] dut_157 failed(never_done)\n";
        return EXIT_FAILURE;
    }
    if (!step(false, true, "DONE_ack")) {
        return EXIT_FAILURE;
    }

    apply_reset();
    const std::uint8_t seq2[] = {0u, 1u, 0u, 1u, 1u, 0u, 0u, 1u};
    for (const std::uint8_t bit : seq2) {
        if (!step(bit != 0u, false, "random")) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_157 passed: programmable countdown with 1000-cycle subcounter\n";
    return EXIT_SUCCESS;
}
