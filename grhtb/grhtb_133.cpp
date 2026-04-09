#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.reset = true;
    sim.in = 0u;
    sim.eval();

    enum State : std::uint8_t { BYTE1 = 0, BYTE2 = 1, BYTE3 = 2, DONE = 3 };
    State state = BYTE1;

    auto tick = [&]() {
        sim.clk = true;
        sim.eval();
        sim.clk = false;
        sim.eval();
    };

    tick();
    state = BYTE1;
    sim.reset = false;

    auto step = [&](std::uint8_t inValue) {
        const bool in3 = ((inValue >> 3) & 1u) != 0u;
        State next = BYTE1;
        switch (state) {
        case BYTE1: next = in3 ? BYTE2 : BYTE1; break;
        case BYTE2: next = BYTE3; break;
        case BYTE3: next = DONE; break;
        case DONE: next = in3 ? BYTE2 : BYTE1; break;
        }
        sim.in = inValue;
        tick();
        state = next;
        if (sim.done != (state == DONE)) {
            std::cerr << "[GrhTB] dut_133 failed(in=0x" << std::hex << static_cast<int>(inValue) << ")\n";
            return false;
        }
        return true;
    };

    for (const std::uint8_t value : {0x00u, 0x08u, 0x00u, 0x00u, 0x00u, 0x08u, 0x00u, 0x00u}) {
        if (!step(value)) {
            return EXIT_FAILURE;
        }
    }

    std::uint8_t current = 0u;
    auto toggle_bit = [&](int bit) {
        std::uint8_t v1 = static_cast<std::uint8_t>(current | (1u << bit));
        if (!step(v1)) {
            return false;
        }
        current = v1;
        std::uint8_t v2 = static_cast<std::uint8_t>(current & ~(1u << bit));
        if (!step(v2)) {
            return false;
        }
        current = v2;
        return true;
    };

    for (int bit = 0; bit < 8; ++bit) {
        if (!toggle_bit(bit)) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_133 passed: 3-byte done handshake FSM\n";
    return EXIT_SUCCESS;
}
