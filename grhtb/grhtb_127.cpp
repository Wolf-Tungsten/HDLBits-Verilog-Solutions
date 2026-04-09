#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static std::uint8_t next_state(std::uint8_t state, std::uint8_t s)
{
    const bool s1 = (s & 0x1u) != 0u;
    const bool s2 = (s & 0x2u) != 0u;
    const bool s3 = (s & 0x4u) != 0u;
    switch (state) {
    case 0: return s1 ? 2u : 0u;
    case 1: return s2 ? 4u : (s1 ? 1u : 0u);
    case 2: return s2 ? 4u : (s1 ? 2u : 0u);
    case 3: return s3 ? 5u : (s2 ? 3u : 1u);
    case 4: return s3 ? 5u : (s2 ? 4u : 1u);
    case 5: return s3 ? 5u : 3u;
    default: return 0u;
    }
}

static std::uint8_t output_mask(std::uint8_t state)
{
    switch (state) {
    case 0: return 0xFu;
    case 1: return 0x7u;
    case 2: return 0x6u;
    case 3: return 0x3u;
    case 4: return 0x2u;
    case 5: return 0x0u;
    default: return 0u;
    }
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.reset = true;
    sim.s = 0u;
    sim.eval();

    std::uint8_t state = 0u;
    sim.clk = true;
    sim.eval();
    sim.clk = false;
    sim.eval();
    sim.reset = false;

    for (int cycle = 0; cycle < 32; ++cycle) {
        const std::uint8_t level = static_cast<std::uint8_t>(cycle & 0x7);
        sim.s = level;
        sim.clk = true;
        sim.eval();
        state = next_state(state, level);

        const std::uint8_t expected = output_mask(state);
        const std::uint8_t actual = static_cast<std::uint8_t>((sim.fr3 << 3) | (sim.fr2 << 2) | (sim.fr1 << 1) | sim.dfr);
        if (actual != expected) {
            std::cerr << "[GrhTB] dut_127 failed(posedge cycle=" << cycle << ")\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        const std::uint8_t held = static_cast<std::uint8_t>((sim.fr3 << 3) | (sim.fr2 << 2) | (sim.fr1 << 1) | sim.dfr);
        if (held != expected) {
            std::cerr << "[GrhTB] dut_127 failed(negedge cycle=" << cycle << ")\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_127 passed: water level controller outputs valid patterns\n";
    return EXIT_SUCCESS;
}
