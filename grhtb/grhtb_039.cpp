#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static void set_u100(GrhSIM_top_module &sim, std::uint64_t low64, std::uint64_t high36)
{
    sim.in[0] = low64;
    sim.in[1] = (high36 & 0xFFFFFFFFFull);
}

static unsigned popcnt64(std::uint64_t x)
{
    x = x - ((x >> 1) & 0x5555555555555555ull);
    x = (x & 0x3333333333333333ull) + ((x >> 2) & 0x3333333333333333ull);
    return static_cast<unsigned>((((x + (x >> 4)) & 0x0F0F0F0F0F0F0F0Full) * 0x0101010101010101ull) >> 56);
}

struct Vec100_039 {
    std::uint64_t lo;
    std::uint64_t hi36;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<Vec100_039, 5> stimuli{{
        {0x0000000000000000ull, 0x000000000ull},
        {0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFFFull},
        {0xAAAAAAAAAAAAAAAAull, 0xAAAAAAAAAull},
        {0x5555555555555555ull, 0x555555555ull},
        {0x0000000000000001ull, 0x000000000ull},
    }};

    for (const auto &v : stimuli) {
        set_u100(sim, v.lo, v.hi36);
        sim.eval();

        const bool all_ones = (v.lo == 0xFFFFFFFFFFFFFFFFull) && ((v.hi36 & 0xFFFFFFFFFull) == 0xFFFFFFFFFull);
        const bool any_one = (v.lo != 0ull) || ((v.hi36 & 0xFFFFFFFFFull) != 0ull);
        const bool parity = ((popcnt64(v.lo) + popcnt64(v.hi36 & 0xFFFFFFFFFull)) & 1u) != 0u;

        if (sim.out_and != all_ones || sim.out_or != any_one || sim.out_xor != parity) {
            std::cerr << "[GrhTB] dut_039 failed: expected and/or/xor="
                      << static_cast<int>(all_ones) << "/" << static_cast<int>(any_one)
                      << "/" << static_cast<int>(parity)
                      << ", got " << static_cast<int>(sim.out_and)
                      << "/" << static_cast<int>(sim.out_or)
                      << "/" << static_cast<int>(sim.out_xor) << '\n';
            return EXIT_FAILURE;
        }
    }

    set_u100(sim, 0ull, 0ull);
    sim.eval();
    if (sim.out_and || sim.out_or || sim.out_xor) {
        return EXIT_FAILURE;
    }
    set_u100(sim, 0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFFFull);
    sim.eval();
    if (!sim.out_and || !sim.out_or) {
        return EXIT_FAILURE;
    }
    set_u100(sim, 0ull, 0ull);
    sim.eval();
    if (sim.out_and || sim.out_or) {
        return EXIT_FAILURE;
    }

    std::uint64_t lo = 0ull;
    std::uint64_t hi = 0ull;
    set_u100(sim, lo, hi);
    sim.eval();
    for (int bit = 0; bit < 100; ++bit) {
        for (int rep = 0; rep < 2; ++rep) {
            if (bit < 64) {
                lo |= (UINT64_C(1) << bit);
            } else {
                hi |= (UINT64_C(1) << (bit - 64));
            }
            set_u100(sim, lo, hi);
            sim.eval();
            if (!sim.out_or || !sim.out_xor || sim.out_and) {
                return EXIT_FAILURE;
            }
            if (bit < 64) {
                lo &= ~(UINT64_C(1) << bit);
            } else {
                hi &= ~(UINT64_C(1) << (bit - 64));
            }
            set_u100(sim, lo, hi);
            sim.eval();
            if (sim.out_or || sim.out_xor || sim.out_and) {
                return EXIT_FAILURE;
            }
        }
    }

    std::cout << "[GrhTB] dut_039 passed: reductions on 100-bit vector verified\n";
    return EXIT_SUCCESS;
}
