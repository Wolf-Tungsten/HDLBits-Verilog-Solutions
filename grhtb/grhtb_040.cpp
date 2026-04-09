#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static void set_u100(GrhSIM_top_module &sim, std::uint64_t low64, std::uint64_t high36)
{
    sim.in[0] = low64;
    sim.in[1] = high36 & 0xFFFFFFFFFull;
}

static void reverse_u100(std::uint64_t inLo, std::uint64_t inHi36, std::uint64_t &outLo, std::uint64_t &outHi36)
{
    outLo = 0;
    outHi36 = 0;
    for (int i = 0; i < 100; ++i) {
        const int dst = 99 - i;
        const std::uint64_t bit = i < 64 ? ((inLo >> i) & 1ull) : ((inHi36 >> (i - 64)) & 1ull);
        if (dst < 64) {
            outLo |= (bit << dst);
        } else {
            outHi36 |= (bit << (dst - 64));
        }
    }
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    struct Vec100_040 {
        std::uint64_t lo;
        std::uint64_t hi36;
    };

    const std::array<Vec100_040, 3> stimuli{{
        {0x0000000000000000ull, 0x000000000ull},
        {0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFFFull},
        {0x0123456789ABCDEFull, 0x123456789ull},
    }};

    for (const auto &v : stimuli) {
        set_u100(sim, v.lo, v.hi36);
        sim.eval();
        std::uint64_t expectedLo = 0;
        std::uint64_t expectedHi = 0;
        reverse_u100(v.lo, v.hi36, expectedLo, expectedHi);
        if (sim.out[0] != expectedLo || (sim.out[1] & 0xFFFFFFFFFull) != expectedHi) {
            std::cerr << "[GrhTB] dut_040 failed: expected reversed 100-bit value\n";
            return EXIT_FAILURE;
        }
    }

    std::uint64_t lo = 0;
    std::uint64_t hi = 0;
    set_u100(sim, lo, hi);
    sim.eval();
    if (sim.out[0] != 0 || (sim.out[1] & 0xFFFFFFFFFull) != 0) {
        std::cerr << "[GrhTB] dut_040 failed: initial zero mismatch\n";
        return EXIT_FAILURE;
    }

    for (int bit = 0; bit < 100; ++bit) {
        for (int rep = 0; rep < 2; ++rep) {
            if (bit < 64) {
                lo |= (UINT64_C(1) << bit);
            } else {
                hi |= (UINT64_C(1) << (bit - 64));
            }
            set_u100(sim, lo, hi);
            sim.eval();
            std::uint64_t expectedLo = 0;
            std::uint64_t expectedHi = 0;
            reverse_u100(lo, hi, expectedLo, expectedHi);
            if (sim.out[0] != expectedLo || (sim.out[1] & 0xFFFFFFFFFull) != expectedHi) {
                return EXIT_FAILURE;
            }

            if (bit < 64) {
                lo &= ~(UINT64_C(1) << bit);
            } else {
                hi &= ~(UINT64_C(1) << (bit - 64));
            }
            set_u100(sim, lo, hi);
            sim.eval();
            reverse_u100(lo, hi, expectedLo, expectedHi);
            if (sim.out[0] != expectedLo || (sim.out[1] & 0xFFFFFFFFFull) != expectedHi) {
                return EXIT_FAILURE;
            }
        }
    }

    std::cout << "[GrhTB] dut_040 passed: bit-reverse of 100-bit vector\n";
    return EXIT_SUCCESS;
}
