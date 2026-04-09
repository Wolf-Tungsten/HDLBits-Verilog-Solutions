#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

using Packed100 = std::array<std::uint64_t, 2>;

static void set_u100(Packed100 &dst, std::uint64_t low64, std::uint64_t high36)
{
    dst[0] = low64;
    dst[1] = high36 & 0xFFFFFFFFFull;
}

static std::uint8_t get_bit_u100(const Packed100 &src, int bit)
{
    const std::size_t word = static_cast<std::size_t>(bit) >> 6;
    const std::size_t offset = static_cast<std::size_t>(bit) & 63u;
    return static_cast<std::uint8_t>((src[word] >> offset) & 1u);
}

static void set_bit(std::uint64_t &low64, std::uint64_t &high36, int bit, bool value)
{
    if (bit < 64) {
        const std::uint64_t mask = UINT64_C(1) << bit;
        low64 = value ? (low64 | mask) : (low64 & ~mask);
    } else {
        const std::uint64_t mask = UINT64_C(1) << (bit - 64);
        high36 = value ? (high36 | mask) : (high36 & ~mask);
    }
}

static bool check_outputs(GrhSIM_top_module &sim,
                          std::uint64_t aLow64,
                          std::uint64_t aHigh36,
                          std::uint64_t bLow64,
                          std::uint64_t bHigh36,
                          bool sel)
{
    set_u100(sim.a, aLow64, aHigh36);
    set_u100(sim.b, bLow64, bHigh36);
    sim.sel = sel;
    sim.eval();

    const Packed100 &expected = sel ? sim.b : sim.a;
    for (int bit = 0; bit < 100; ++bit) {
        if (get_bit_u100(sim.out, bit) != get_bit_u100(expected, bit)) {
            return false;
        }
    }
    return true;
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    if (!check_outputs(sim, 0, 0, ~UINT64_C(0), 0xFFFFFFFFFull, false) ||
        !check_outputs(sim, 0, 0, ~UINT64_C(0), 0xFFFFFFFFFull, true)) {
        return EXIT_FAILURE;
    }

    std::uint64_t aLow64 = 0;
    std::uint64_t aHigh36 = 0;
    for (int bit = 0; bit < 100; ++bit) {
        for (int rep = 0; rep < 2; ++rep) {
            set_bit(aLow64, aHigh36, bit, true);
            if (!check_outputs(sim, aLow64, aHigh36, 0, 0, false)) {
                return EXIT_FAILURE;
            }
            set_bit(aLow64, aHigh36, bit, false);
            if (!check_outputs(sim, aLow64, aHigh36, 0, 0, false)) {
                return EXIT_FAILURE;
            }
        }
    }

    std::uint64_t bLow64 = 0;
    std::uint64_t bHigh36 = 0;
    for (int bit = 0; bit < 100; ++bit) {
        for (int rep = 0; rep < 2; ++rep) {
            set_bit(bLow64, bHigh36, bit, true);
            if (!check_outputs(sim, 0, 0, bLow64, bHigh36, true)) {
                return EXIT_FAILURE;
            }
            set_bit(bLow64, bHigh36, bit, false);
            if (!check_outputs(sim, 0, 0, bLow64, bHigh36, true)) {
                return EXIT_FAILURE;
            }
        }
    }

    for (int rep = 0; rep < 2; ++rep) {
        if (!check_outputs(sim, ~UINT64_C(0), 0xFFFFFFFFFull, 0, 0, true) ||
            !check_outputs(sim, ~UINT64_C(0), 0xFFFFFFFFFull, 0, 0, false)) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_062 passed: 100-bit 2:1 mux\n";
    return EXIT_SUCCESS;
}
