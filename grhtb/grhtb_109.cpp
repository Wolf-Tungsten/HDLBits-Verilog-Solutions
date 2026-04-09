#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus109 {
    bool load;
    bool ena;
    std::uint8_t amount;
    std::uint64_t data;
};

static std::uint64_t step_model(std::uint64_t value, const Stimulus109 &stim)
{
    if (stim.load) {
        return stim.data;
    }
    if (!stim.ena) {
        return value;
    }

    switch (stim.amount & 0x3u) {
    case 0u:
        return value << 1;
    case 1u:
        return value << 8;
    case 2u: {
        const bool msb = ((value >> 63) & 1ull) != 0u;
        value >>= 1;
        if (msb) {
            value |= (UINT64_C(1) << 63);
        }
        return value;
    }
    case 3u: {
        const bool msb = ((value >> 63) & 1ull) != 0u;
        value >>= 8;
        if (msb) {
            value |= UINT64_C(0xFF00000000000000);
        }
        return value;
    }
    default:
        return value;
    }
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.load = false;
    sim.ena = false;
    sim.amount = 0u;
    sim.data = 0u;
    sim.eval();

    const std::array<Stimulus109, 9> stimuli{{{true, false, 0u, UINT64_C(0x8000000000000001)},
                                              {false, true, 0u, 0u},
                                              {false, true, 1u, 0u},
                                              {false, true, 2u, 0u},
                                              {false, true, 3u, 0u},
                                              {true, false, 0u, UINT64_C(0x000000000000FF00)},
                                              {false, true, 2u, 0u},
                                              {false, true, 3u, 0u},
                                              {false, false, 0u, 0u}}};
    std::uint64_t qModel = 0u;

    for (const auto &stim : stimuli) {
        sim.load = stim.load;
        sim.ena = stim.ena;
        sim.amount = stim.amount;
        sim.data = stim.data;

        sim.clk = true;
        sim.eval();
        qModel = step_model(qModel, stim);
        if (static_cast<std::uint64_t>(sim.q) != qModel) {
            std::cerr << "[GrhTB] dut_109 failed(main sequence)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (static_cast<std::uint64_t>(sim.q) != qModel) {
            std::cerr << "[GrhTB] dut_109 failed(main sequence hold)\n";
            return EXIT_FAILURE;
        }
    }

    for (int bit = 0; bit < 64; ++bit) {
        const std::uint64_t mask = UINT64_C(1) << bit;

        sim.load = true;
        sim.ena = false;
        sim.amount = 0u;
        sim.data = mask;
        sim.clk = true;
        sim.eval();
        qModel = mask;
        if (static_cast<std::uint64_t>(sim.q) != qModel) {
            std::cerr << "[GrhTB] dut_109 failed(load-toggle set)\n";
            return EXIT_FAILURE;
        }
        sim.clk = false;
        sim.eval();

        sim.data = 0u;
        sim.clk = true;
        sim.eval();
        qModel = 0u;
        if (static_cast<std::uint64_t>(sim.q) != qModel) {
            std::cerr << "[GrhTB] dut_109 failed(load-toggle clear)\n";
            return EXIT_FAILURE;
        }
        sim.clk = false;
        sim.eval();
    }

    std::cout << "[GrhTB] dut_109 passed: 64-bit barrel/arithmetic shifter\n";
    return EXIT_SUCCESS;
}
