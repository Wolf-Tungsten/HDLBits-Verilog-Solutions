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
    sim.x = false;
    sim.eval();

    enum State : std::uint8_t { A = 0, B = 1, C = 2, D = 3, E = 4 };
    State state = A;

    auto tick = [&]() {
        sim.clk = true;
        sim.eval();
        sim.clk = false;
        sim.eval();
    };

    auto step = [&](bool reset, bool x, const char *ctx) {
        sim.reset = reset;
        sim.x = x;
        if (reset) {
            state = A;
        } else {
            State next = A;
            switch (state) {
            case A: next = x ? B : A; break;
            case B: next = x ? E : B; break;
            case C: next = x ? B : C; break;
            case D: next = x ? C : B; break;
            case E: next = x ? E : D; break;
            }
            state = next;
        }
        tick();
        const bool z = state == D || state == E;
        if (sim.z != z) {
            std::cerr << "[GrhTB] dut_143 failed(" << ctx << ")\n";
            return false;
        }
        return true;
    };

    if (!step(true, false, "reset")) {
        return EXIT_FAILURE;
    }
    for (const auto x : {false, true, false, true, false, false, true, true, false, true, false, true}) {
        if (!step(false, x, "seq1")) {
            return EXIT_FAILURE;
        }
    }
    if (!step(true, false, "reset2") || !step(false, true, "A_B") || !step(false, true, "B_E") ||
        !step(false, false, "E_D")) {
        return EXIT_FAILURE;
    }

    std::cout << "[GrhTB] dut_143 passed: 5-state FSM with z on D/E\n";
    return EXIT_SUCCESS;
}
