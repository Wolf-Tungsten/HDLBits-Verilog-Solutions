#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static void zero_u256(GrhSIM_top_module &sim)
{
    for (int word = 0; word < 4; ++word) {
        sim.in[word] = 0;
    }
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    for (int sel = 0; sel < 256; ++sel) {
        const int word = sel >> 6;
        const int bit = sel & 63;
        for (int rep = 0; rep < 2; ++rep) {
            zero_u256(sim);
            sim.sel = static_cast<std::uint8_t>(sel);
            sim.eval();
            if (sim.out != 0) {
                return EXIT_FAILURE;
            }

            sim.in[word] |= (UINT64_C(1) << bit);
            sim.eval();
            if (sim.out != 1) {
                return EXIT_FAILURE;
            }

            sim.in[word] &= ~(UINT64_C(1) << bit);
            sim.eval();
            if (sim.out != 0) {
                return EXIT_FAILURE;
            }
        }
    }

    for (int rep = 0; rep < 2; ++rep) {
        for (int bit = 0; bit < 8; ++bit) {
            zero_u256(sim);
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

    std::cout << "[GrhTB] dut_064 passed: bit-select via sel\n";
    return EXIT_SUCCESS;
}
