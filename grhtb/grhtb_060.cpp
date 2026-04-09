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

static bool check_outputs(GrhSIM_top_module &sim, std::uint64_t low64, std::uint64_t high36)
{
    set_u100(sim.in, low64, high36);
    sim.eval();

    for (int bit = 0; bit < 99; ++bit) {
        const std::uint8_t current = get_bit_u100(sim.in, bit);
        const std::uint8_t next = get_bit_u100(sim.in, bit + 1);
        if (get_bit_u100(sim.out_both, bit) != static_cast<std::uint8_t>(current & next)) {
            return false;
        }
        if (get_bit_u100(sim.out_any, bit) != static_cast<std::uint8_t>(current | next)) {
            return false;
        }
        if (get_bit_u100(sim.out_different, bit) != static_cast<std::uint8_t>(current ^ next)) {
            return false;
        }
    }

    return get_bit_u100(sim.out_different, 99) ==
           static_cast<std::uint8_t>(get_bit_u100(sim.in, 99) ^ get_bit_u100(sim.in, 0));
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    if (!check_outputs(sim, 0ull, 0ull) || !check_outputs(sim, ~UINT64_C(0), 0xFFFFFFFFFull) ||
        !check_outputs(sim, 0x0123456789ABCDEFull, 0x123456789ull)) {
        return EXIT_FAILURE;
    }

    std::uint64_t low64 = 0;
    std::uint64_t high36 = 0;
    for (int bit = 0; bit < 100; ++bit) {
        for (int rep = 0; rep < 2; ++rep) {
            set_bit(low64, high36, bit, true);
            if (!check_outputs(sim, low64, high36)) {
                return EXIT_FAILURE;
            }
            set_bit(low64, high36, bit, false);
            if (!check_outputs(sim, low64, high36)) {
                return EXIT_FAILURE;
            }
        }
    }

    std::cout << "[GrhTB] dut_060 passed: neighbor pair ops on 100-bit vector\n";
    return EXIT_SUCCESS;
}
