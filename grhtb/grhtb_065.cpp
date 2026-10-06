#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static void zero_u1024(GrhSIM_top_module &sim)
{
    sim.in = 0;
}

static void set_nibble(GrhSIM_top_module &sim, int index, std::uint8_t value)
{
    const int bit = 4 * index;
    using Wide = unsigned _BitInt(1024);
    const Wide mask = static_cast<Wide>(0xF) << bit;
    sim.in = (sim.in & ~mask) | (static_cast<Wide>(value & 0xFu) << bit);
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    for (int index = 0; index < 256; ++index) {
        for (int rep = 0; rep < 2; ++rep) {
            zero_u1024(sim);
            sim.sel = static_cast<std::uint8_t>(index);
            sim.eval();
            if (sim.out != 0) {
                return EXIT_FAILURE;
            }

            set_nibble(sim, index, 0xFu);
            sim.eval();
            if (sim.out != 0xFu) {
                return EXIT_FAILURE;
            }

            set_nibble(sim, index, 0x0u);
            sim.eval();
            if (sim.out != 0x0u) {
                return EXIT_FAILURE;
            }
        }
    }

    for (int rep = 0; rep < 2; ++rep) {
        for (int bit = 0; bit < 8; ++bit) {
            zero_u1024(sim);
            sim.sel = 0;
            sim.eval();
            if (sim.out != 0) {
                return EXIT_FAILURE;
            }
            sim.sel = static_cast<std::uint8_t>(1u << bit);
            sim.eval();
            if (sim.out != 0) {
                return EXIT_FAILURE;
            }
            sim.sel = 0;
            sim.eval();
            if (sim.out != 0) {
                return EXIT_FAILURE;
            }
        }
    }

    std::cout << "[GrhTB] dut_065 passed: 4-bit slice select\n";
    return EXIT_SUCCESS;
}
