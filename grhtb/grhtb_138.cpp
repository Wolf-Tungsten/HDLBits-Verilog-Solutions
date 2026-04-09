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
    sim.in = false;
    sim.eval();

    enum State : std::uint8_t {
        ST_NONE = 0,
        ST_ONE = 1,
        ST_TWO = 2,
        ST_THREE = 3,
        ST_FOUR = 4,
        ST_FIVE = 5,
        ST_SIX = 6,
        ST_DISC = 7,
        ST_FLAG = 8,
        ST_ERR = 9,
    };

    State state = ST_NONE;

    auto tick = [&]() {
        sim.clk = true;
        sim.eval();
        sim.clk = false;
        sim.eval();
    };

    auto apply_reset = [&]() {
        sim.reset = true;
        sim.in = false;
        sim.eval();
        tick();
        sim.reset = false;
        state = ST_NONE;
    };

    auto check_outputs = [&](const char *ctx) {
        const bool disc = state == ST_DISC;
        const bool flag = state == ST_FLAG;
        const bool err = state == ST_ERR;
        if (sim.disc != disc || sim.flag != flag || sim.err != err) {
            std::cerr << "[GrhTB] dut_138 failed(" << ctx << ")\n";
            return false;
        }
        return true;
    };

    auto step = [&](bool bit, const char *ctx) {
        sim.in = bit;
        State next = ST_NONE;
        switch (state) {
        case ST_NONE: next = bit ? ST_ONE : ST_NONE; break;
        case ST_ONE: next = bit ? ST_TWO : ST_NONE; break;
        case ST_TWO: next = bit ? ST_THREE : ST_NONE; break;
        case ST_THREE: next = bit ? ST_FOUR : ST_NONE; break;
        case ST_FOUR: next = bit ? ST_FIVE : ST_NONE; break;
        case ST_FIVE: next = bit ? ST_SIX : ST_DISC; break;
        case ST_SIX: next = bit ? ST_ERR : ST_FLAG; break;
        case ST_DISC: next = bit ? ST_ONE : ST_NONE; break;
        case ST_FLAG: next = bit ? ST_ONE : ST_NONE; break;
        case ST_ERR: next = bit ? ST_ERR : ST_NONE; break;
        }
        tick();
        state = next;
        return check_outputs(ctx);
    };

    apply_reset();
    if (!step(false, "none0") || !step(false, "none0_again")) {
        return EXIT_FAILURE;
    }

    apply_reset();
    for (const auto bit : {false, true, true, true, true, true, false, false}) {
        if (!step(bit, "disc_path")) {
            return EXIT_FAILURE;
        }
    }

    apply_reset();
    for (const auto bit : {false, true, true, true, true, true, false, true}) {
        if (!step(bit, "disc_to_one")) {
            return EXIT_FAILURE;
        }
    }

    apply_reset();
    for (const auto bit : {false, true, true, true, true, true, true, false, true}) {
        if (!step(bit, "flag_to_one")) {
            return EXIT_FAILURE;
        }
    }

    apply_reset();
    for (const auto bit : {false, true, true, true, true, true, true, false, false}) {
        if (!step(bit, "flag_to_none")) {
            return EXIT_FAILURE;
        }
    }

    apply_reset();
    for (const auto bit : {true, true, true, true, true, true, true, true, false}) {
        if (!step(bit, "err_path")) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_138 passed: HDLC flag/disc/err pattern FSM with full coverage\n";
    return EXIT_SUCCESS;
}
