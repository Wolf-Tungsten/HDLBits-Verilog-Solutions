#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.areset = false;
    sim.train_valid = false;
    sim.train_taken = false;
    sim.eval();

    enum State : std::uint8_t { SNT = 0, WNT = 1, WT = 2, ST = 3 };
    State state = WNT;

    auto tick = [&]() {
        sim.clk = true;
        sim.eval();
        sim.clk = false;
        sim.eval();
    };

    auto apply_reset = [&]() {
        sim.areset = true;
        sim.eval();
        if ((sim.state & 0x3u) != WNT) {
            std::cerr << "[GrhTB] dut_160 failed(async_reset_immediate)\n";
            std::exit(EXIT_FAILURE);
        }
        tick();
        sim.areset = false;
        sim.eval();
        state = WNT;
    };

    auto step = [&](bool valid, bool taken, const char *ctx) {
        sim.train_valid = valid;
        sim.train_taken = taken;

        State next = state;
        if (valid) {
            switch (state) {
            case SNT: next = taken ? WNT : SNT; break;
            case WNT: next = taken ? WT : SNT; break;
            case WT: next = taken ? ST : WNT; break;
            case ST: next = taken ? ST : WT; break;
            }
        }

        tick();
        if ((sim.state & 0x3u) != static_cast<std::uint8_t>(next)) {
            std::cerr << "[GrhTB] dut_160 failed(" << ctx << ")\n";
            return false;
        }
        if (valid) {
            state = next;
        }
        return true;
    };

    apply_reset();
    if (!step(true, false, "WNT_to_SNT") || !step(true, true, "SNT_to_WNT") || !step(true, true, "WNT_to_WT") ||
        !step(true, true, "WT_to_ST") || !step(true, false, "ST_to_WT") || !step(true, false, "WT_to_WNT") ||
        !step(true, false, "WNT_to_SNT2") || !step(true, true, "SNT_to_WNT2")) {
        return EXIT_FAILURE;
    }
    for (int i = 0; i < 4; ++i) {
        if (!step(false, false, "hold")) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_160 passed: 2-bit saturating branch predictor FSM\n";
    return EXIT_SUCCESS;
}
