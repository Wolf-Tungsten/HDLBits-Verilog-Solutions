#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static void set_bcd_digit(std::uint16_t &value, int digit, std::uint8_t nibble)
{
    const std::uint16_t mask = static_cast<std::uint16_t>(0xFu << (digit * 4));
    value = static_cast<std::uint16_t>((value & ~mask) | ((nibble & 0xFu) << (digit * 4)));
}

static void bcd_add4(std::uint16_t a, std::uint16_t b, bool cin, std::uint16_t &sum, bool &cout)
{
    std::uint8_t carry = cin ? 1u : 0u;
    sum = 0;
    for (int digit = 0; digit < 4; ++digit) {
        std::uint8_t t = static_cast<std::uint8_t>(((a >> (digit * 4)) & 0xFu) +
                                                   ((b >> (digit * 4)) & 0xFu) + carry);
        carry = t > 9 ? 1u : 0u;
        if (carry) {
            t = static_cast<std::uint8_t>(t + 6u);
        }
        set_bcd_digit(sum, digit, static_cast<std::uint8_t>(t & 0xFu));
    }
    cout = carry != 0u;
}

static bool check_outputs(GrhSIM_top_module &sim, std::uint16_t a, std::uint16_t b, bool cin, const char *ctx)
{
    sim.a = a;
    sim.b = b;
    sim.cin = cin;
    sim.eval();

    std::uint16_t expectedSum = 0;
    bool expectedCout = false;
    bcd_add4(a, b, cin, expectedSum, expectedCout);
    if (sim.sum != expectedSum || sim.cout != expectedCout) {
        std::cerr << "[GrhTB] dut_072 failed(" << ctx << "): expected sum=0x" << std::hex << expectedSum
                  << " cout=" << std::dec << static_cast<int>(expectedCout)
                  << ", got sum=0x" << std::hex << static_cast<std::uint16_t>(sim.sum)
                  << " cout=" << std::dec << static_cast<int>(sim.cout) << '\n';
        return false;
    }
    return true;
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    if (!check_outputs(sim, 0x0000u, 0x0000u, false, "zero") ||
        !check_outputs(sim, 0x1234u, 0x5678u, false, "1234+5678") ||
        !check_outputs(sim, 0x9999u, 0x0001u, false, "9999+0001") ||
        !check_outputs(sim, 0x0000u, 0x0000u, true, "zero_cin1") ||
        !check_outputs(sim, 0x9999u, 0x9999u, false, "9999+9999") ||
        !check_outputs(sim, 0x9999u, 0x9999u, true, "9999+9999_cin1")) {
        return EXIT_FAILURE;
    }

    for (int rep = 0; rep < 2; ++rep) {
        if (!check_outputs(sim, 0x0000u, 0x0000u, false, "cin0") || !check_outputs(sim, 0x0000u, 0x0000u, true, "cin1")) {
            return EXIT_FAILURE;
        }
    }
    if (!check_outputs(sim, 0x0000u, 0x0000u, false, "cin0_end")) {
        return EXIT_FAILURE;
    }

    for (int digit = 0; digit < 4; ++digit) {
        std::uint16_t a = 0;
        std::uint16_t b = 0;
        for (int rep = 0; rep < 2; ++rep) {
            if (!check_outputs(sim, a, b, false, "digit_zero")) {
                return EXIT_FAILURE;
            }
            set_bcd_digit(a, digit, 9);
            set_bcd_digit(b, digit, 9);
            if (!check_outputs(sim, a, b, false, "digit_9plus9")) {
                return EXIT_FAILURE;
            }
            set_bcd_digit(a, digit, 0);
            set_bcd_digit(b, digit, 0);
            if (!check_outputs(sim, a, b, false, "digit_back_zero")) {
                return EXIT_FAILURE;
            }
        }
    }

    for (int bit = 0; bit < 16; ++bit) {
        const std::uint16_t mask = static_cast<std::uint16_t>(1u << bit);
        for (int rep = 0; rep < 2; ++rep) {
            if (!check_outputs(sim, 0x0000u, 0x0000u, false, "a_off") || !check_outputs(sim, mask, 0x0000u, false, "a_on") ||
                !check_outputs(sim, 0x0000u, 0x0000u, false, "a_off2")) {
                return EXIT_FAILURE;
            }
        }
    }

    for (int bit = 0; bit < 16; ++bit) {
        const std::uint16_t mask = static_cast<std::uint16_t>(1u << bit);
        for (int rep = 0; rep < 2; ++rep) {
            if (!check_outputs(sim, 0x0000u, 0x0000u, false, "b_off") || !check_outputs(sim, 0x0000u, mask, false, "b_on") ||
                !check_outputs(sim, 0x0000u, 0x0000u, false, "b_off2")) {
                return EXIT_FAILURE;
            }
        }
    }

    std::cout << "[GrhTB] dut_072 passed: 4-digit BCD ripple adder\n";
    return EXIT_SUCCESS;
}
