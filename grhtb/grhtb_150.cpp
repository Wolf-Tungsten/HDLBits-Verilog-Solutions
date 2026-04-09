#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static std::uint8_t g_bus(const GrhSIM_top_module &sim)
{
    return static_cast<std::uint8_t>(sim.g & 0x7u);
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.resetn = true;
    sim.r = 0u;
    sim.eval();

    enum State : std::uint8_t { A = 0, B = 1, C = 2, D = 3 };
    State state = A;

    auto tick = [&]() {
        sim.clk = true;
        sim.eval();
        sim.clk = false;
        sim.eval();
    };

    auto apply_reset = [&]() {
        sim.resetn = false;
        sim.r = 0u;
        tick();
        sim.resetn = true;
        state = A;
    };

    auto step = [&](std::uint8_t r, const char *ctx) {
        const bool r1 = (r & 0x1u) != 0u;
        const bool r2 = (r & 0x2u) != 0u;
        const bool r3 = (r & 0x4u) != 0u;
        sim.r = static_cast<std::uint8_t>(r & 0x7u);

        State next = A;
        switch (state) {
        case A:
            if (r1) {
                next = B;
            } else if (r2) {
                next = C;
            } else if (r3) {
                next = D;
            } else {
                next = A;
            }
            break;
        case B: next = r1 ? B : A; break;
        case C: next = r2 ? C : A; break;
        case D: next = r3 ? D : A; break;
        }

        tick();
        state = next;
        std::uint8_t g = 0u;
        if (state == B) {
            g = 0x1u;
        } else if (state == C) {
            g = 0x2u;
        } else if (state == D) {
            g = 0x4u;
        }
        if (g_bus(sim) != g) {
            std::cerr << "[GrhTB] dut_150 failed(" << ctx << ")\n";
            return false;
        }
        return true;
    };

    apply_reset();
    if (!step(0x0u, "A_A") || !step(0x1u, "A_B") || !step(0x0u, "B_A")) {
        return EXIT_FAILURE;
    }
    apply_reset();
    if (!step(0x2u, "A_C") || !step(0x0u, "C_A")) {
        return EXIT_FAILURE;
    }
    apply_reset();
    if (!step(0x4u, "A_D") || !step(0x0u, "D_A")) {
        return EXIT_FAILURE;
    }
    apply_reset();
    if (!step(0x1u, "A_B_hold") || !step(0x1u, "B_B") || !step(0x0u, "B_A2")) {
        return EXIT_FAILURE;
    }
    apply_reset();
    if (!step(0x2u, "A_C_hold") || !step(0x2u, "C_C") || !step(0x0u, "C_A2")) {
        return EXIT_FAILURE;
    }
    apply_reset();
    if (!step(0x4u, "A_D_hold") || !step(0x4u, "D_D") || !step(0x0u, "D_A2")) {
        return EXIT_FAILURE;
    }

    std::cout << "[GrhTB] dut_150 passed: fixed-priority arbiter full coverage\n";
    return EXIT_SUCCESS;
}
