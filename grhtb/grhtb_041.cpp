#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static void clear_u255(GrhSIM_top_module &sim)
{
    sim.in = 0;
}

static void set_bit_u255(GrhSIM_top_module &sim, int bit, bool value)
{
    if (value) {
        sim.in |= (unsigned _BitInt(256))1 << bit;
    } else {
        sim.in &= ~((unsigned _BitInt(256))1 << bit);
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
