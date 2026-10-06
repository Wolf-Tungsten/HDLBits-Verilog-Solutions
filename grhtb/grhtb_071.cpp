#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

using Wide100 = unsigned _BitInt(128);

static void set_u100(Wide100 &dst, std::uint64_t low64, std::uint64_t high36)
{
    dst = (static_cast<Wide100>(high36 & 0xFFFFFFFFFull) << 64) | static_cast<Wide100>(low64);
}

static std::uint8_t get_bit_u100(const Wide100 &src, int bit)
{
    return static_cast<std::uint8_t>((src >> bit) & 1);
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
                          bool cin,
                          const char *ctx)
{
    set_u100(sim.a, aLow64, aHigh36);
    set_u100(sim.b, bLow64, bHigh36);
    sim.cin = cin;
    sim.eval();

    std::uint8_t carry = cin ? 1u : 0u;
    for (int bit = 0; bit < 100; ++bit) {
        const std::uint8_t ai = bit < 64 ? static_cast<std::uint8_t>((aLow64 >> bit) & 1u)
                                         : static_cast<std::uint8_t>((aHigh36 >> (bit - 64)) & 1u);
        const std::uint8_t bi = bit < 64 ? static_cast<std::uint8_t>((bLow64 >> bit) & 1u)
                                         : static_cast<std::uint8_t>((bHigh36 >> (bit - 64)) & 1u);
        const std::uint8_t sum = static_cast<std::uint8_t>((ai ^ bi) ^ carry);
        const std::uint8_t cout = static_cast<std::uint8_t>((ai & bi) | (ai & carry) | (bi & carry));
        if (get_bit_u100(sim.sum, bit) != sum) {
            std::cerr << "[GrhTB] dut_071 failed(" << ctx << ") at bit " << bit << '\n';
            return false;
        }
        carry = cout;
    }

    if (sim.cout != (carry != 0u)) {
        std::cerr << "[GrhTB] dut_071 failed(" << ctx << "): cout mismatch\n";
        return false;
    }
    return true;
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    if (!check_outputs(sim, 0, 0, 0, 0, false, "zero") ||
        !check_outputs(sim, 0, 0, 0, 0, true, "zero_cin1") ||
        !check_outputs(sim, ~UINT64_C(0), 0xFFFFFFFFFull, 0, 0, false, "max_plus_zero") ||
        !check_outputs(sim, ~UINT64_C(0), 0xFFFFFFFFFull, 1, 0, false, "max_plus_one") ||
        !check_outputs(sim, ~UINT64_C(0), 0xFFFFFFFFFull, 0, 0, true, "max_plus_cin1")) {
        return EXIT_FAILURE;
    }

    for (int rep = 0; rep < 2; ++rep) {
        if (!check_outputs(sim, 0, 0, 0, 0, false, "cin0") || !check_outputs(sim, 0, 0, 0, 0, true, "cin1")) {
            return EXIT_FAILURE;
        }
    }
    if (!check_outputs(sim, 0, 0, 0, 0, false, "cin0_end")) {
        return EXIT_FAILURE;
    }

    for (int bit = 0; bit < 100; ++bit) {
        std::uint64_t aLow64 = 0;
        std::uint64_t aHigh36 = 0;
        for (int rep = 0; rep < 2; ++rep) {
            if (!check_outputs(sim, aLow64, aHigh36, 0, 0, false, "a_off")) {
                return EXIT_FAILURE;
            }
            set_bit(aLow64, aHigh36, bit, true);
            if (!check_outputs(sim, aLow64, aHigh36, 0, 0, false, "a_on")) {
                return EXIT_FAILURE;
            }
            set_bit(aLow64, aHigh36, bit, false);
            if (!check_outputs(sim, aLow64, aHigh36, 0, 0, false, "a_off2")) {
                return EXIT_FAILURE;
            }
        }
    }

    for (int bit = 0; bit < 100; ++bit) {
        std::uint64_t bLow64 = 0;
        std::uint64_t bHigh36 = 0;
        for (int rep = 0; rep < 2; ++rep) {
            if (!check_outputs(sim, 0, 0, bLow64, bHigh36, false, "b_off")) {
                return EXIT_FAILURE;
            }
            set_bit(bLow64, bHigh36, bit, true);
            if (!check_outputs(sim, 0, 0, bLow64, bHigh36, false, "b_on")) {
                return EXIT_FAILURE;
            }
            set_bit(bLow64, bHigh36, bit, false);
            if (!check_outputs(sim, 0, 0, bLow64, bHigh36, false, "b_off2")) {
                return EXIT_FAILURE;
            }
        }
    }

    std::cout << "[GrhTB] dut_071 passed: 100-bit adder with single carry-out\n";
    return EXIT_SUCCESS;
}
