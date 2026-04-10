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
    sim.predict_valid = false;
    sim.predict_taken = false;
    sim.train_mispredicted = false;
    sim.train_taken = false;
    sim.train_history = 0u;
    sim.eval();

    std::uint32_t histModel = 0u;

    auto tick = [&]() {
        sim.clk = true;
        sim.eval();
        sim.clk = false;
        sim.eval();
    };

    auto apply_reset = [&]() {
        sim.areset = true;
        sim.eval();
        if (sim.predict_history != 0u) {
            std::cerr << "[GrhTB] dut_161 failed(async_reset_immediate)\n";
            std::exit(EXIT_FAILURE);
        }
        tick();
        sim.areset = false;
        sim.eval();
        histModel = 0u;
    };

    auto step = [&](bool trainMispredicted, bool predictValid, bool predictTaken, std::uint32_t trainHistory, bool trainTaken,
                    const char *ctx) {
        sim.train_mispredicted = trainMispredicted;
        sim.predict_valid = predictValid;
        sim.predict_taken = predictTaken;
        sim.train_history = trainHistory;
        sim.train_taken = trainTaken;

        if (trainMispredicted) {
            histModel = (trainHistory << 1) | (trainTaken ? 1u : 0u);
        } else if (predictValid) {
            histModel = (histModel << 1) | (predictTaken ? 1u : 0u);
        }

        tick();
        if (sim.predict_history != histModel) {
            std::cerr << "[GrhTB] dut_161 failed(" << ctx << ")\n";
            return false;
        }
        return true;
    };

    apply_reset();
    if (!step(true, false, false, 0x00000000u, true, "mispred_0") || !step(true, true, true, 0xAAAAAAAAu, false, "mispred_A") ||
        !step(true, false, true, 0x55555555u, true, "mispred_5")) {
        return EXIT_FAILURE;
    }
    for (int i = 0; i < 32; ++i) {
        if (!step(false, true, true, 0u, false, "predict_ones")) {
            return EXIT_FAILURE;
        }
    }
    for (int i = 0; i < 32; ++i) {
        if (!step(false, true, false, 0u, false, "predict_zeros")) {
            return EXIT_FAILURE;
        }
    }
    for (int i = 0; i < 4; ++i) {
        if (!step(false, false, false, 0u, false, "idle")) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_161 passed: global history register\n";
    return EXIT_SUCCESS;
}
