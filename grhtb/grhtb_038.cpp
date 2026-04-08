#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static std::uint8_t parity8(std::uint8_t v)
{
    v ^= static_cast<std::uint8_t>(v >> 4);
    v ^= static_cast<std::uint8_t>(v >> 2);
    v ^= static_cast<std::uint8_t>(v >> 1);
    return static_cast<std::uint8_t>(v & 1u);
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<std::uint8_t, 6> stimuli{{0x00u, 0x01u, 0x03u, 0x07u, 0xFEu, 0xFFu}};
    for (std::uint8_t v : stimuli) {
        sim.in = v;
        sim.eval();
        const std::uint8_t expected = parity8(v);
        if (sim.parity != expected) {
            std::cerr << "[GrhTB] dut_038 failed: in=0x" << std::hex << static_cast<int>(v) << std::dec
                      << ", expected parity=" << static_cast<int>(expected)
                      << ", got " << static_cast<int>(sim.parity) << '\n';
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < 8; ++i) {
        for (int rep = 0; rep < 2; ++rep) {
            sim.in = 0x00u;
            sim.eval();
            if (sim.parity != parity8(sim.in)) {
                return EXIT_FAILURE;
            }
            sim.in = static_cast<std::uint8_t>(1u << i);
            sim.eval();
            if (sim.parity != parity8(sim.in)) {
                return EXIT_FAILURE;
            }
        }
        sim.in = 0x00u;
        sim.eval();
        if (sim.parity != parity8(sim.in)) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_038 passed: parity reduction verified\n";
    return EXIT_SUCCESS;
}
