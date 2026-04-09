#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static void set_u100(std::array<std::uint64_t, 2> &dst, std::uint64_t low64, std::uint64_t high36)
{
    dst[0] = low64;
    dst[1] = high36 & 0xFFFFFFFFFull;
}

static std::uint8_t get_bit_u100(const std::array<std::uint64_t, 2> &src, int bit)
{
    const std::size_t word = static_cast<std::size_t>(bit) >> 6;
    const std::size_t offset = static_cast<std::size_t>(bit) & 63u;
    return static_cast<std::uint8_t>((src[word] >> offset) & 1u);
}

static void set_bit_pair(std::uint64_t &low64, std::uint64_t &high36, int bit, bool value)
{
    if (bit < 64) {
        const std::uint64_t mask = UINT64_C(1) << bit;
        low64 = value ? (low64 | mask) : (low64 & ~mask);
    } else {
        const std::uint64_t mask = UINT64_C(1) << (bit - 64);
        high36 = value ? (high36 | mask) : (high36 & ~mask);
    }
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const auto check = [&](std::uint64_t aLo,
                           std::uint64_t aHi,
                           std::uint64_t bLo,
                           std::uint64_t bHi,
                           bool cin,
                           const char *ctx)
    {
        set_u100(sim.a, aLo, aHi);
        set_u100(sim.b, bLo, bHi);
        sim.cin = cin;
        sim.eval();

        std::uint8_t carry = cin ? 1u : 0u;
        for (int bit = 0; bit < 100; ++bit) {
            const std::uint8_t ai = bit < 64 ? static_cast<std::uint8_t>((aLo >> bit) & 1u)
                                             : static_cast<std::uint8_t>((aHi >> (bit - 64)) & 1u);
            const std::uint8_t bi = bit < 64 ? static_cast<std::uint8_t>((bLo >> bit) & 1u)
                                             : static_cast<std::uint8_t>((bHi >> (bit - 64)) & 1u);
            const std::uint8_t sum = static_cast<std::uint8_t>((ai ^ bi) ^ carry);
            const std::uint8_t cout = static_cast<std::uint8_t>((ai & bi) | (ai & carry) | (bi & carry));
            if (get_bit_u100(sim.sum, bit) != sum || get_bit_u100(sim.cout, bit) != cout) {
                std::cerr << "[GrhTB] dut_042 failed(" << ctx << ") at bit " << bit << '\n';
                std::exit(EXIT_FAILURE);
            }
            carry = cout;
        }
    };

    check(0, 0, 0, 0, false, "zero");
    check(~UINT64_C(0), 0xFFFFFFFFFull, 0, 0, false, "allones_plus_zero");
    check(~UINT64_C(0), 0xFFFFFFFFFull, 0, 0, true, "allones_plus_zero_cin1");
    check(0xFFFFFFFFFFFFFFFEull, 0, 1, 0, false, "low_carry");
    check(0, 1, 0, 0, true, "high36_carry");

    for (int rep = 0; rep < 2; ++rep) {
        check(0, 0, 0, 0, false, "cin0");
        check(0, 0, 0, 0, true, "cin1");
    }
    check(0, 0, 0, 0, false, "cin0_end");

    for (int bit = 0; bit < 100; ++bit) {
        std::uint64_t aLo = 0;
        std::uint64_t aHi = 0;
        std::uint64_t bLo = 0;
        std::uint64_t bHi = 0;
        for (int rep = 0; rep < 2; ++rep) {
            check(aLo, aHi, bLo, bHi, false, "a_bit_off");
            set_bit_pair(aLo, aHi, bit, true);
            check(aLo, aHi, bLo, bHi, false, "a_bit_on");
            set_bit_pair(aLo, aHi, bit, false);
            check(aLo, aHi, bLo, bHi, false, "a_bit_off2");
        }
    }

    for (int bit = 0; bit < 100; ++bit) {
        std::uint64_t aLo = 0;
        std::uint64_t aHi = 0;
        std::uint64_t bLo = 0;
        std::uint64_t bHi = 0;
        for (int rep = 0; rep < 2; ++rep) {
            check(aLo, aHi, bLo, bHi, false, "ab_off");
            set_bit_pair(aLo, aHi, bit, true);
            set_bit_pair(bLo, bHi, bit, true);
            check(aLo, aHi, bLo, bHi, false, "ab_on");
            set_bit_pair(bLo, bHi, bit, false);
            check(aLo, aHi, bLo, bHi, false, "ab_b_off");
            set_bit_pair(aLo, aHi, bit, false);
            check(aLo, aHi, bLo, bHi, false, "ab_a_off");
        }
    }

    std::cout << "[GrhTB] dut_042 passed: 100-bit ripple-carry adder\n";
    return EXIT_SUCCESS;
}
