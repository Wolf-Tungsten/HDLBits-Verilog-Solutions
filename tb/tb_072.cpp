#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_072.h"

// 4-digit (16-bit) BCD add: adds a and b (packed BCD) plus cin.
// Each nibble is one BCD digit (least-significant digit in bits [3:0]).
static inline void bcd_add4(uint16_t a, uint16_t b, uint8_t cin,
                            uint16_t &sum, uint8_t &cout) {
    uint8_t carry = cin & 1u;
    uint16_t s = 0;
    for (int d = 0; d < 4; ++d) {
        const uint8_t an = static_cast<uint8_t>((a >> (4 * d)) & 0xFu);
        const uint8_t bn = static_cast<uint8_t>((b >> (4 * d)) & 0xFu);
        uint8_t t = static_cast<uint8_t>(an + bn + carry);
        if (t > 9u) {
            t = static_cast<uint8_t>(t + 6u); // BCD adjust
            carry = 1u;
        } else {
            carry = 0u;
        }
        s |= static_cast<uint16_t>(t & 0xFu) << (4 * d);
    }
    sum = s;
    cout = carry & 1u;
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto context = std::make_unique<VerilatedContext>();
    context->traceEverOn(false);

    auto dut = std::make_unique<Vdut_072>(context.get());

    auto check = [&](uint16_t a, uint16_t b, uint8_t cin, const char *ctx) {
        dut->a = a;
        dut->b = b;
        dut->cin = cin & 1u;
        dut->eval();

        uint16_t exp_sum = 0;
        uint8_t exp_cout = 0;
        bcd_add4(a, b, cin, exp_sum, exp_cout);

        if (dut->sum != exp_sum || dut->cout != exp_cout) {
            std::cerr << "[TB] dut_072 failed(" << ctx << "): "
                      << "a=0x" << std::hex << a
                      << " b=0x" << b
                      << " cin=" << std::dec << int(cin)
                      << " expected sum=0x" << std::hex << exp_sum
                      << " cout=" << std::dec << int(exp_cout)
                      << " got sum=0x" << std::hex << dut->sum
                      << " cout=" << std::dec << int(dut->cout)
                      << std::endl;
            return false;
        }
        return true;
    };

    // Basic functional checks on representative BCD values.
    if (!check(0x0000u, 0x0000u, 0, "zero"))
        return EXIT_FAILURE;
    if (!check(0x1234u, 0x5678u, 0, "1234+5678"))
        return EXIT_FAILURE;
    if (!check(0x9999u, 0x0001u, 0, "9999+0001"))
        return EXIT_FAILURE;
    if (!check(0x0000u, 0x0000u, 1, "zero_cin1"))
        return EXIT_FAILURE;
    if (!check(0x9999u, 0x9999u, 0, "9999+9999"))
        return EXIT_FAILURE;
    if (!check(0x9999u, 0x9999u, 1, "9999+9999_cin1"))
        return EXIT_FAILURE;

    // Toggle cin 0->1->0 twice with zero operands.
    for (int rep = 0; rep < 2; ++rep) {
        if (!check(0x0000u, 0x0000u, 0, "cin0"))
            return EXIT_FAILURE;
        if (!check(0x0000u, 0x0000u, 1, "cin1"))
            return EXIT_FAILURE;
    }
    if (!check(0x0000u, 0x0000u, 0, "cin0_end"))
        return EXIT_FAILURE;

    // For each digit position, drive that digit as 9+9 to force carries through each bcd_fadd.
    for (int d = 0; d < 4; ++d) {
        const uint16_t digit_mask = static_cast<uint16_t>(0xFu << (4 * d));
        for (int rep = 0; rep < 2; ++rep) {
            if (!check(0x0000u, 0x0000u, 0, "digit_zero"))
                return EXIT_FAILURE;
            if (!check(digit_mask * 9u, digit_mask * 9u, 0, "digit_9plus9"))
                return EXIT_FAILURE;
            if (!check(0x0000u, 0x0000u, 0, "digit_back_zero"))
                return EXIT_FAILURE;
        }
    }

    // Bit-level toggles for a and b: toggle each bit 0->1->0 twice with other inputs zero.
    for (int bit = 0; bit < 16; ++bit) {
        const uint16_t mask = static_cast<uint16_t>(1u << bit);
        for (int rep = 0; rep < 2; ++rep) {
            if (!check(0x0000u, 0x0000u, 0, "a_bit_off"))
                return EXIT_FAILURE;
            if (!check(mask, 0x0000u, 0, "a_bit_on"))
                return EXIT_FAILURE;
            if (!check(0x0000u, 0x0000u, 0, "a_bit_off2"))
                return EXIT_FAILURE;
        }
    }
    for (int bit = 0; bit < 16; ++bit) {
        const uint16_t mask = static_cast<uint16_t>(1u << bit);
        for (int rep = 0; rep < 2; ++rep) {
            if (!check(0x0000u, 0x0000u, 0, "b_bit_off"))
                return EXIT_FAILURE;
            if (!check(0x0000u, mask, 0, "b_bit_on"))
                return EXIT_FAILURE;
            if (!check(0x0000u, 0x0000u, 0, "b_bit_off2"))
                return EXIT_FAILURE;
        }
    }

    std::cout << "[TB] dut_072 passed: 4-digit BCD ripple adder" << std::endl;

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}

