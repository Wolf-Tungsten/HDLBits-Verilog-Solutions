#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

using Packed400 = std::array<std::uint64_t, 7>;

static void clear_u400(Packed400 &value)
{
    value.fill(0);
}

static void set_bcd_digit(Packed400 &value, int digit, std::uint8_t nibble)
{
    const std::size_t bit = static_cast<std::size_t>(digit) * 4u;
    const std::size_t word = bit >> 6;
    const std::size_t shift = bit & 63u;
    const std::uint64_t mask = UINT64_C(0xF) << shift;
    value[word] = (value[word] & ~mask) | ((static_cast<std::uint64_t>(nibble & 0xFu)) << shift);
}

static std::uint8_t get_bcd_digit(const Packed400 &value, int digit)
{
    const std::size_t bit = static_cast<std::size_t>(digit) * 4u;
    const std::size_t word = bit >> 6;
    const std::size_t shift = bit & 63u;
    return static_cast<std::uint8_t>((value[word] >> shift) & 0xFu);
}

static void bcd_add_100(const Packed400 &a,
                        const Packed400 &b,
                        bool cin,
                        Packed400 &sum,
                        bool &cout)
{
    clear_u400(sum);
    std::uint8_t carry = cin ? 1u : 0u;
    for (int digit = 0; digit < 100; ++digit) {
        std::uint8_t t = static_cast<std::uint8_t>(get_bcd_digit(a, digit) + get_bcd_digit(b, digit) + carry);
        carry = t > 9 ? 1u : 0u;
        if (carry) {
            t = static_cast<std::uint8_t>(t + 6u);
        }
        set_bcd_digit(sum, digit, static_cast<std::uint8_t>(t & 0xFu));
    }
    cout = carry != 0u;
}

static bool packed400_equal(const Packed400 &lhs, const Packed400 &rhs)
{
    for (std::size_t i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const auto check = [&](const Packed400 &a, const Packed400 &b, bool cin, const char *ctx)
    {
        sim.a = a;
        sim.b = b;
        sim.cin = cin;
        sim.eval();

        Packed400 expectedSum{};
        bool expectedCout = false;
        bcd_add_100(a, b, cin, expectedSum, expectedCout);
        if (!packed400_equal(sim.sum, expectedSum) || sim.cout != expectedCout) {
            std::cerr << "[GrhTB] dut_043 failed(" << ctx << "): cout=" << static_cast<int>(sim.cout)
                      << ", expected " << static_cast<int>(expectedCout) << '\n';
            return false;
        }
        return true;
    };

    Packed400 zero{};
    clear_u400(zero);
    if (!check(zero, zero, false, "zero")) {
        return EXIT_FAILURE;
    }

    Packed400 a1234{};
    Packed400 b5678{};
    clear_u400(a1234);
    clear_u400(b5678);
    set_bcd_digit(a1234, 0, 4);
    set_bcd_digit(a1234, 1, 3);
    set_bcd_digit(a1234, 2, 2);
    set_bcd_digit(a1234, 3, 1);
    set_bcd_digit(b5678, 0, 8);
    set_bcd_digit(b5678, 1, 7);
    set_bcd_digit(b5678, 2, 6);
    set_bcd_digit(b5678, 3, 5);
    if (!check(a1234, b5678, false, "1234+5678")) {
        return EXIT_FAILURE;
    }

    Packed400 a9999{};
    Packed400 b0001{};
    clear_u400(a9999);
    clear_u400(b0001);
    for (int digit = 0; digit < 4; ++digit) {
        set_bcd_digit(a9999, digit, 9);
    }
    set_bcd_digit(b0001, 0, 1);
    if (!check(a9999, b0001, false, "9999+0001")) {
        return EXIT_FAILURE;
    }
    if (!check(zero, zero, true, "cin1")) {
        return EXIT_FAILURE;
    }

    Packed400 a{};
    Packed400 b{};
    clear_u400(a);
    clear_u400(b);
    for (int digit = 0; digit < 100; ++digit) {
        for (int rep = 0; rep < 2; ++rep) {
            if (!check(a, b, false, "digit_off")) {
                return EXIT_FAILURE;
            }
            set_bcd_digit(a, digit, 9);
            set_bcd_digit(b, digit, 9);
            if (!check(a, b, false, "digit_on")) {
                return EXIT_FAILURE;
            }
            set_bcd_digit(a, digit, 0);
            set_bcd_digit(b, digit, 0);
            if (!check(a, b, false, "digit_off2")) {
                return EXIT_FAILURE;
            }
        }
    }

    for (int rep = 0; rep < 2; ++rep) {
        if (!check(zero, zero, false, "cin0")) {
            return EXIT_FAILURE;
        }
        if (!check(zero, zero, true, "cin1_toggle")) {
            return EXIT_FAILURE;
        }
        if (!check(zero, zero, false, "cin0_toggle")) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_043 passed: 100-digit BCD adder verified\n";
    return EXIT_SUCCESS;
}
