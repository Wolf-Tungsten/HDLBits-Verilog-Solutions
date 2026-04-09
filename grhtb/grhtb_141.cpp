#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.areset = true;
    sim.x = false;
    sim.eval();

    enum State : std::uint8_t { A = 1, B = 2 };
    State state = A;

    auto tick = [&]() {
        sim.clk = true;
        sim.eval();
        sim.clk = false;
        sim.eval();
    };

    tick();
    sim.areset = false;
    state = A;

    auto step = [&](bool x, const char *ctx) {
        sim.x = x;
        sim.eval();

        const bool z = (state == A) ? x : !x;
        State next = state;
        switch (state) {
        case A: next = x ? B : A; break;
        case B: next = B; break;
        }
        if (sim.z != z) {
            std::cerr << "[GrhTB] dut_141 failed(" << ctx << " comb)\n";
            return false;
        }
        tick();
        state = next;
        return true;
    };

    for (const auto x : {false, true, true, false}) {
        if (!step(x, "seq1")) {
            return EXIT_FAILURE;
        }
    }

    sim.areset = true;
    sim.eval();
    tick();
    sim.areset = false;
    state = A;

    for (const auto x : {true, false}) {
        if (!step(x, "seq2")) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_141 passed: simple 2-state FSM with z behavior\n";
    return EXIT_SUCCESS;
}
