#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.resetn = true;
    sim.x = false;
    sim.y = false;
    sim.eval();

    enum State : std::uint8_t { A = 0, F1 = 1, TMP0 = 2, TMP1 = 3, TMP2 = 4, G1 = 5, G1P = 6, TMP3 = 7, G0P = 8 };
    State state = A;

    auto tick = [&]() {
        sim.clk = true;
        sim.eval();
        sim.clk = false;
        sim.eval();
    };

    auto apply_reset = [&]() {
        sim.resetn = false;
        sim.x = false;
        sim.y = false;
        tick();
        sim.resetn = true;
        state = A;
    };

    auto step = [&](bool x, bool y, const char *ctx) {
        sim.x = x;
        sim.y = y;

        State next = state;
        switch (state) {
        case A: next = sim.resetn ? F1 : A; break;
        case F1: next = TMP0; break;
        case TMP0: next = x ? TMP1 : TMP0; break;
        case TMP1: next = x ? TMP1 : TMP2; break;
        case TMP2: next = x ? G1 : TMP0; break;
        case G1: next = y ? G1P : TMP3; break;
        case TMP3: next = y ? G1P : G0P; break;
        case G1P: next = sim.resetn ? G1P : A; break;
        case G0P: next = sim.resetn ? G0P : A; break;
        }

        tick();
        state = next;

        std::uint8_t fExp = 0u;
        std::uint8_t gExp = 0u;
        switch (state) {
        case F1: fExp = 1u; break;
        case G1:
        case TMP3:
        case G1P: gExp = 1u; break;
        default: break;
        }
        if (static_cast<std::uint8_t>(sim.f) != fExp || static_cast<std::uint8_t>(sim.g) != gExp) {
            std::cerr << "[GrhTB] dut_151 failed(" << ctx << ")\n";
            return false;
        }
        return true;
    };

    apply_reset();
    if (!step(false, false, "S1.A_to_F1") || !step(false, false, "S1.F1_to_TMP0") || !step(true, false, "S1.TMP0_to_TMP1") ||
        !step(false, false, "S1.TMP1_to_TMP2") || !step(true, false, "S1.TMP2_to_G1") || !step(false, true, "S1.G1_to_G1P") ||
        !step(false, true, "S1.G1P_hold")) {
        return EXIT_FAILURE;
    }

    apply_reset();
    if (!step(false, false, "S2.A_to_F1") || !step(false, false, "S2.F1_to_TMP0") || !step(true, false, "S2.TMP0_to_TMP1") ||
        !step(false, false, "S2.TMP1_to_TMP2") || !step(true, false, "S2.TMP2_to_G1") || !step(false, false, "S2.G1_to_TMP3") ||
        !step(false, true, "S2.TMP3_to_G1P") || !step(false, true, "S2.G1P_hold")) {
        return EXIT_FAILURE;
    }

    apply_reset();
    if (!step(false, false, "S3.A_to_F1") || !step(false, false, "S3.F1_to_TMP0") || !step(true, false, "S3.TMP0_to_TMP1") ||
        !step(false, false, "S3.TMP1_to_TMP2") || !step(true, false, "S3.TMP2_to_G1") || !step(false, true, "S3.G1_to_G1P")) {
        return EXIT_FAILURE;
    }
    apply_reset();
    if (!step(false, false, "S3b.A_to_F1") || !step(false, false, "S3b.F1_to_TMP0") || !step(true, false, "S3b.TMP0_to_TMP1") ||
        !step(false, false, "S3b.TMP1_to_TMP2") || !step(true, false, "S3b.TMP2_to_G1") || !step(false, false, "S3b.G1_to_TMP3") ||
        !step(false, false, "S3b.TMP3_to_G0P")) {
        return EXIT_FAILURE;
    }
    apply_reset();

    std::cout << "[GrhTB] dut_151 passed: FSM with f/g outputs\n";
    return EXIT_SUCCESS;
}
