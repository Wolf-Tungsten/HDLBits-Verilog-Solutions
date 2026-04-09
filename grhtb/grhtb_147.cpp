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
    sim.w = false;
    sim.eval();

    enum State : std::uint8_t { A = 0, B = 1, C = 2, D = 3, E = 4, F = 5 };
    State state = A;

    auto tick = [&]() {
        sim.clk = true;
        sim.eval();
        sim.clk = false;
        sim.eval();
    };

    auto apply_reset = [&]() {
        sim.reset = true;
        sim.w = false;
        sim.eval();
        tick();
        sim.reset = false;
        state = A;
    };

    auto step = [&](bool w, const char *ctx) {
        sim.w = w;
        State next = A;
        switch ((static_cast<std::uint8_t>(state) << 1) | (w ? 1u : 0u)) {
        case (A << 1) | 0u: next = B; break;
        case (A << 1) | 1u: next = A; break;
        case (B << 1) | 0u: next = C; break;
        case (B << 1) | 1u: next = D; break;
        case (C << 1) | 0u: next = E; break;
        case (C << 1) | 1u: next = D; break;
        case (D << 1) | 0u: next = F; break;
        case (D << 1) | 1u: next = A; break;
        case (E << 1) | 0u: next = E; break;
        case (E << 1) | 1u: next = D; break;
        case (F << 1) | 0u: next = C; break;
        case (F << 1) | 1u: next = D; break;
        default: next = A; break;
        }
        tick();
        state = next;
        const bool z = state == E || state == F;
        if (sim.z != z) {
            std::cerr << "[GrhTB] dut_147 failed(" << ctx << ")\n";
            return false;
        }
        return true;
    };

    apply_reset();
    for (const auto w : {false, false, false, false, true, false, false, true, true, true}) {
        if (!step(w, "seq1")) {
            return EXIT_FAILURE;
        }
    }

    apply_reset();
    for (const auto w : {false, true, true, false, false, false, true, false, true}) {
        if (!step(w, "seq2")) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_147 passed: 6-state FSM with z on E/F\n";
    return EXIT_SUCCESS;
}
