#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

namespace {

constexpr std::uint8_t SNT = 0u;
constexpr std::uint8_t WNT = 1u;
constexpr std::uint8_t WT = 2u;
constexpr std::uint8_t ST = 3u;

struct Inputs {
    bool predictValid;
    std::uint8_t predictPc;
    bool trainValid;
    bool trainTaken;
    bool trainMispredicted;
    std::uint8_t trainHistory;
    std::uint8_t trainPc;
};

struct ModelState {
    std::array<std::uint8_t, 128> pht{};
    std::uint8_t ghr = 0u;
};

std::uint8_t update_counter(std::uint8_t current, bool taken)
{
    switch (current) {
    case SNT: return taken ? WNT : SNT;
    case WNT: return taken ? WT : SNT;
    case WT: return taken ? ST : WNT;
    case ST:
    default: return taken ? ST : WT;
    }
}

void reset_model(ModelState &model)
{
    model.ghr = 0u;
    model.pht.fill(WNT);
}

} // namespace

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.areset = false;
    sim.predict_valid = false;
    sim.predict_pc = 0u;
    sim.train_valid = false;
    sim.train_taken = false;
    sim.train_mispredicted = false;
    sim.train_history = 0u;
    sim.train_pc = 0u;
    sim.eval();

    ModelState model;
    reset_model(model);

    auto apply_reset = [&]() {
        sim.areset = true;
        sim.eval();
        sim.areset = false;
        sim.eval();
        reset_model(model);
    };

    auto run_cycle = [&](const Inputs &in, const char *ctx) {
        bool expectedTaken = false;
        std::uint8_t expectedHistory = 0u;
        if (in.predictValid) {
            const std::uint8_t predictIndex = static_cast<std::uint8_t>((in.predictPc ^ model.ghr) & 0x7Fu);
            expectedTaken = model.pht[predictIndex] >= WT;
            expectedHistory = model.ghr;
        }

        sim.predict_valid = in.predictValid;
        sim.predict_pc = in.predictPc;
        sim.train_valid = in.trainValid;
        sim.train_taken = in.trainTaken;
        sim.train_mispredicted = in.trainMispredicted;
        sim.train_history = in.trainHistory;
        sim.train_pc = in.trainPc;

        sim.clk = false;
        sim.eval();
        if (static_cast<bool>(sim.predict_taken) != expectedTaken || static_cast<std::uint8_t>(sim.predict_history & 0x7Fu) != expectedHistory) {
            std::cerr << "[GrhTB] dut_162 failed(predict " << ctx << ")\n";
            return false;
        }

        sim.clk = true;
        sim.eval();

        if (in.trainValid) {
            const std::uint8_t trainIndex = static_cast<std::uint8_t>((in.trainPc ^ in.trainHistory) & 0x7Fu);
            model.pht[trainIndex] = update_counter(model.pht[trainIndex], in.trainTaken);
        }
        if (in.trainValid && in.trainMispredicted) {
            model.ghr = static_cast<std::uint8_t>(((in.trainHistory & 0x3Fu) << 1u) | (in.trainTaken ? 1u : 0u));
        } else if (in.predictValid) {
            model.ghr = static_cast<std::uint8_t>(((model.ghr & 0x3Fu) << 1u) | (expectedTaken ? 1u : 0u));
        }

        sim.clk = false;
        sim.eval();
        return true;
    };

    apply_reset();
    const Inputs quickCases[] = {
        {true, 0x7Fu, false, false, false, 0x00u, 0x00u},
        {true, 0x00u, false, false, false, 0x7Fu, 0x7Fu},
        {false, 0x00u, true, true, true, 0x3Fu, 0x40u},
        {false, 0x00u, true, false, true, 0x00u, 0x00u},
        {true, 0x10u, true, true, true, 0x2Au, 0x55u},
        {false, 0x08u, true, false, true, 0x15u, 0x12u},
        {false, 0x00u, true, true, false, 0x00u, 0x00u},
        {false, 0x00u, true, true, false, 0x00u, 0x00u},
        {false, 0x00u, true, false, false, 0x00u, 0x00u},
        {false, 0x2Au, false, false, true, 0x15u, 0x3Bu},
        {true, 0x35u, false, false, true, 0x1Eu, 0x21u},
    };
    for (const Inputs &in : quickCases) {
        if (!run_cycle(in, "quick")) {
            return EXIT_FAILURE;
        }
    }

    apply_reset();
    const std::uint8_t sampleIndices[] = {0u, 1u, 0x33u, 0x40u, 0x7Fu};
    for (const std::uint8_t idx : sampleIndices) {
        if (!run_cycle(Inputs{false, 0x00u, true, true, false, 0x00u, idx}, "pht_up")) {
            return EXIT_FAILURE;
        }
        if (!run_cycle(Inputs{false, 0x00u, true, false, false, 0x00u, idx}, "pht_down")) {
            return EXIT_FAILURE;
        }
    }

    apply_reset();
    const Inputs functionalCases[] = {
        {false, 0x00u, false, false, false, 0x00u, 0x00u},
        {true, 0x0Fu, false, false, false, 0x00u, 0x00u},
        {true, 0x20u, true, true, true, 0x55u, 0x12u},
        {true, 0x7Fu, false, false, false, 0x00u, 0x00u},
        {true, 0x01u, true, true, false, 0x0Fu, 0x3Cu},
        {false, 0x10u, true, true, false, 0x1Cu, 0x2Fu},
        {false, 0x10u, true, true, false, 0x00u, 0x33u},
    };
    for (const Inputs &in : functionalCases) {
        if (!run_cycle(in, "func")) {
            return EXIT_FAILURE;
        }
    }

    Inputs predictStrong{};
    predictStrong.predictValid = true;
    predictStrong.predictPc = static_cast<std::uint8_t>(model.ghr ^ 0x33u);
    if (!run_cycle(predictStrong, "predict_strong")) {
        return EXIT_FAILURE;
    }

    const Inputs finalCases[] = {
        {true, 0x40u, true, false, false, 0x7Fu, 0x4Cu},
        {false, 0x02u, true, false, false, 0x55u, 0x66u},
        {true, 0x00u, true, false, false, 0x01u, 0x32u},
        {false, 0x00u, true, false, false, 0x7Eu, 0x4Du},
        {true, 0x15u, true, true, false, 0x08u, 0x3Bu},
    };
    for (const Inputs &in : finalCases) {
        if (!run_cycle(in, "final")) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_162 passed all prediction and training scenarios\n";
    return EXIT_SUCCESS;
}
