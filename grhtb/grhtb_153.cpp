#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.shift_ena = false;
    sim.count_ena = false;
    sim.data = false;
    sim.eval();

    std::uint8_t regModel = 0u;

    auto tick = [&]() {
        sim.clk = true;
        sim.eval();
        sim.clk = false;
        sim.eval();
    };

    auto step = [&](bool shiftEna, bool countEna, bool data, const char *ctx) {
        sim.shift_ena = shiftEna;
        sim.count_ena = countEna;
        sim.data = data;

        std::uint8_t next = regModel;
        if (shiftEna) {
            next = static_cast<std::uint8_t>(((regModel << 1) & 0xEu) | (data ? 1u : 0u));
        } else if (countEna) {
            next = static_cast<std::uint8_t>((regModel - 1u) & 0xFu);
        }

        tick();
        regModel = next;
        if ((sim.q & 0xFu) != regModel) {
            std::cerr << "[GrhTB] dut_153 failed(" << ctx << ")\n";
            return false;
        }
        return true;
    };

    if (!step(true, false, true, "shift1") || !step(true, false, false, "shift2") || !step(true, false, true, "shift3") ||
        !step(true, false, true, "shift4") || !step(false, false, false, "idle") ) {
        return EXIT_FAILURE;
    }
    for (int i = 0; i < 5; ++i) {
        if (!step(false, true, false, "count")) {
            return EXIT_FAILURE;
        }
    }
    if (!step(true, false, false, "shift_again") || !step(false, true, false, "count_again") ||
        !step(true, false, true, "shift_again2") || !step(false, true, false, "count_again2")) {
        return EXIT_FAILURE;
    }

    std::cout << "[GrhTB] dut_153 passed: shift-or-decrement register\n";
    return EXIT_SUCCESS;
}
