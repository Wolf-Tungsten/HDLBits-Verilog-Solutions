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
    sim.in = 0u;
    sim.eval();

    enum State : std::uint8_t { BYTE1 = 0, BYTE2 = 1, BYTE3 = 2, DONE = 3 };
    State state = BYTE1;
    std::uint32_t dataModel = 0u;

    auto tick = [&]() {
        sim.clk = true;
        sim.eval();
        sim.clk = false;
        sim.eval();
    };

    auto apply_reset = [&]() {
        sim.reset = true;
        sim.in = 0u;
        sim.eval();
        tick();
        state = BYTE1;
        dataModel = 0u;
        sim.reset = false;
    };

    auto step = [&](std::uint8_t inValue) {
        State next = BYTE1;
        switch (state) {
        case BYTE1: next = (inValue & 0x8u) ? BYTE2 : BYTE1; break;
        case BYTE2: next = BYTE3; break;
        case BYTE3: next = DONE; break;
        case DONE: next = (inValue & 0x8u) ? BYTE2 : BYTE1; break;
        }
        sim.in = inValue;
        tick();
        dataModel = ((dataModel << 8) & 0x00FFFF00u) | inValue;
        state = next;
        const bool done = state == DONE;
        const std::uint32_t out = done ? dataModel : 0u;
        if (sim.done != done || (sim.out_bytes & 0xFFFFFFu) != out) {
            std::cerr << "[GrhTB] dut_134 failed(in=0x" << std::hex << static_cast<int>(inValue) << ")\n";
            return false;
        }
        return true;
    };

    apply_reset();
    for (int value = 0; value < 256; ++value) {
        if (!step(static_cast<std::uint8_t>(value))) {
            return EXIT_FAILURE;
        }
    }

    apply_reset();
    for (int value = 255; value >= 0; --value) {
        if (!step(static_cast<std::uint8_t>(value))) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_134 passed: exhaustive byte sweeps with coverage-focused patterns\n";
    return EXIT_SUCCESS;
}
