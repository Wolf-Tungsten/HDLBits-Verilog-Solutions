#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.load = false;
    sim.data = 0u;
    sim.eval();

    std::uint16_t counter = 0u;

    auto tick = [&]() {
        sim.clk = true;
        sim.eval();
        sim.clk = false;
        sim.eval();
    };

    auto loadAndCheck = [&](std::uint16_t value, const char *ctx) {
        sim.load = true;
        sim.data = static_cast<std::uint16_t>(value & 0x3FFu);
        counter = static_cast<std::uint16_t>(value & 0x3FFu);
        tick();
        const std::uint8_t tcExp = static_cast<std::uint8_t>(counter == 0u);
        if (static_cast<std::uint8_t>(sim.tc) != tcExp) {
            std::cerr << "[GrhTB] dut_159 failed(load " << ctx << ")\n";
            return false;
        }
        sim.load = false;
        return true;
    };

    auto runDown = [&](int cycles, const char *ctx) {
        for (int i = 0; i < cycles; ++i) {
            if (counter > 1u) {
                --counter;
            } else {
                counter = 0u;
            }
            tick();
            const std::uint8_t tcExp = static_cast<std::uint8_t>(counter == 0u);
            if (static_cast<std::uint8_t>(sim.tc) != tcExp) {
                std::cerr << "[GrhTB] dut_159 failed(run " << ctx << ")\n";
                return false;
            }
        }
        return true;
    };

    const std::uint16_t patterns[] = {0x000u, 0x001u, 0x3FFu, 0x155u, 0x2AAu};
    for (const std::uint16_t pattern : patterns) {
        if (!loadAndCheck(pattern, "pattern")) {
            return EXIT_FAILURE;
        }
        const int cycles = (pattern > 0u) ? (static_cast<int>(pattern) + 2) : 5;
        if (!runDown(cycles, "down")) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_159 passed: down-counter with terminal count tc\n";
    return EXIT_SUCCESS;
}
