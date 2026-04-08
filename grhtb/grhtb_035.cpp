#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static std::uint8_t lsb_index8(std::uint8_t v)
{
    if (v == 0) {
        return 0;
    }
    for (std::uint8_t i = 0; i < 8; ++i) {
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

    const std::array<std::uint8_t, 9> stimuli{{0x00u, 0x01u, 0x02u, 0x04u, 0x08u, 0x10u, 0x20u, 0x40u, 0x80u}};
    for (std::uint8_t v : stimuli) {
        sim.in = v;
        sim.eval();
        const std::uint8_t expected = lsb_index8(v);
        if (sim.pos != expected) {
            std::cerr << "[GrhTB] dut_035 failed: in=0x" << std::hex << static_cast<int>(v) << std::dec
                      << ", expected pos=" << static_cast<int>(expected)
                      << ", got " << static_cast<int>(sim.pos) << '\n';
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < 8; ++i) {
        for (int rep = 0; rep < 2; ++rep) {
            sim.in = 0x00u;
            sim.eval();
            if (sim.pos != 0) {
                return EXIT_FAILURE;
            }
            sim.in = static_cast<std::uint8_t>(1u << i);
            sim.eval();
            if (sim.pos != i) {
                return EXIT_FAILURE;
            }
        }
        sim.in = 0x00u;
        sim.eval();
        if (sim.pos != 0) {
            return EXIT_FAILURE;
        }
    }

    const std::uint8_t walk[] = {1u, 2u, 4u, 8u, 16u, 32u, 64u, 128u, 64u, 32u, 16u, 8u, 4u, 2u, 1u, 0u};
    for (std::uint8_t v : walk) {
        sim.in = v;
        sim.eval();
        if (sim.pos != lsb_index8(v)) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_035 passed: casez LSB encoder behavior verified\n";
    return EXIT_SUCCESS;
}
