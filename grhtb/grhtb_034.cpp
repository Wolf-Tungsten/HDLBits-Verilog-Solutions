#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static std::uint8_t lsb_index(std::uint8_t v)
{
    if (v == 0) {
        return 0;
    }
    for (std::uint8_t i = 0; i < 4; ++i) {
        if (v & (static_cast<std::uint8_t>(1u) << i)) {
            return i;
        }
    }
    return 0;
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    for (std::uint8_t v = 0; v < 16; ++v) {
        sim.in = v & 0xFu;
        sim.eval();
        const std::uint8_t expected = lsb_index(v);
        if (sim.pos != expected) {
            std::cerr << "[GrhTB] dut_034 failed: in=" << static_cast<int>(v)
                      << ", expected pos=" << static_cast<int>(expected)
                      << ", got " << static_cast<int>(sim.pos) << '\n';
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < 4; ++i) {
        sim.in = 0u;
        sim.eval();
        if (sim.pos != 0) {
            std::cerr << "[GrhTB] dut_034 failed: pos should be 0 at in=0\n";
            return EXIT_FAILURE;
        }
        sim.in = static_cast<std::uint8_t>(1u << i);
        sim.eval();
        if (sim.pos != i) {
            std::cerr << "[GrhTB] dut_034 failed: pos mismatch when in=1<<i\n";
            return EXIT_FAILURE;
        }
        sim.in = 0u;
        sim.eval();
        if (sim.pos != 0) {
            std::cerr << "[GrhTB] dut_034 failed: pos should be 0 after clearing bit\n";
            return EXIT_FAILURE;
        }
    }

    const std::uint8_t walk[] = {1u, 2u, 4u, 8u, 4u, 2u, 1u, 0u};
    for (std::uint8_t v : walk) {
        sim.in = v;
        sim.eval();
        const std::uint8_t expected = lsb_index(v);
        if (sim.pos != expected) {
            std::cerr << "[GrhTB] dut_034 failed on pos walk: in=" << static_cast<int>(v)
                      << ", expected pos=" << static_cast<int>(expected)
                      << ", got " << static_cast<int>(sim.pos) << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_034 passed: pos equals index of LSB set (or 0)\n";
    return EXIT_SUCCESS;
}
