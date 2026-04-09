#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static void clear_u255(GrhSIM_top_module &sim)
{
    sim.in[0] = 0;
    sim.in[1] = 0;
    sim.in[2] = 0;
    sim.in[3] = 0;
}

static void set_bit_u255(GrhSIM_top_module &sim, int bit, bool value)
{
    const std::size_t word = static_cast<std::size_t>(bit) >> 6;
    const std::size_t offset = static_cast<std::size_t>(bit) & 63u;
    const std::uint64_t mask = UINT64_C(1) << offset;
    if (value) {
        sim.in[word] |= mask;
    } else {
        sim.in[word] &= ~mask;
    }
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    clear_u255(sim);
    sim.eval();
    if (sim.out != 0u) {
        std::cerr << "[GrhTB] dut_041 failed: out!=0 at zero\n";
        return EXIT_FAILURE;
    }

    std::uint16_t expected = 0;
    for (int bit = 0; bit < 255; ++bit) {
        set_bit_u255(sim, bit, true);
        sim.eval();
        ++expected;
        if (sim.out != expected) {
            std::cerr << "[GrhTB] dut_041 failed: after set bit " << bit << '\n';
            return EXIT_FAILURE;
        }

        set_bit_u255(sim, bit, false);
        sim.eval();
        --expected;
        if (sim.out != expected) {
            std::cerr << "[GrhTB] dut_041 failed: after clear bit " << bit << '\n';
            return EXIT_FAILURE;
        }
    }

    for (int bit = 0; bit < 255; ++bit) {
        set_bit_u255(sim, bit, true);
    }
    sim.eval();
    if (sim.out != 255u) {
        std::cerr << "[GrhTB] dut_041 failed: all ones expected 255\n";
        return EXIT_FAILURE;
    }

    clear_u255(sim);
    sim.eval();
    if (sim.out != 0u) {
        return EXIT_FAILURE;
    }

    std::cout << "[GrhTB] dut_041 passed: popcount over 255 bits\n";
    return EXIT_SUCCESS;
}
