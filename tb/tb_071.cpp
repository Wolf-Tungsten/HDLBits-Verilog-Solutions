#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_071.h"

// Map 100-bit values into 4x32-bit words used by Verilator (little-endian words).
static inline void set_u100(Vdut_071 *dut,
                            uint64_t a_lo, uint64_t a_hi36,
                            uint64_t b_lo, uint64_t b_hi36,
                            uint8_t cin) {
    dut->a[0] = static_cast<uint32_t>(a_lo & 0xFFFFFFFFull);
    dut->a[1] = static_cast<uint32_t>((a_lo >> 32) & 0xFFFFFFFFull);
    dut->a[2] = static_cast<uint32_t>(a_hi36 & 0xFFFFFFFFull);
    dut->a[3] = static_cast<uint32_t>((a_hi36 >> 32) & 0xFull);

    dut->b[0] = static_cast<uint32_t>(b_lo & 0xFFFFFFFFull);
    dut->b[1] = static_cast<uint32_t>((b_lo >> 32) & 0xFFFFFFFFull);
    dut->b[2] = static_cast<uint32_t>(b_hi36 & 0xFFFFFFFFull);
    dut->b[3] = static_cast<uint32_t>((b_hi36 >> 32) & 0xFull);

    dut->cin = cin & 1u;
}

static inline uint8_t get_bit_u100(const uint32_t w[4], int bit) {
    const int wi = bit >> 5;
    const int bi = bit & 31;
    return static_cast<uint8_t>((w[wi] >> bi) & 1u);
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto context = std::make_unique<VerilatedContext>();
    context->traceEverOn(false);

    auto dut = std::make_unique<Vdut_071>(context.get());

    auto check = [&](uint64_t alo, uint64_t ahi36,
                     uint64_t blo, uint64_t bhi36,
                     uint8_t cin, const char *ctx) {
        set_u100(dut.get(), alo, ahi36, blo, bhi36, cin);
        dut->eval();

        uint8_t carry = cin & 1u;
        for (int i = 0; i < 100; ++i) {
            const uint8_t ai = (i < 64)
                                   ? static_cast<uint8_t>((alo >> i) & 1u)
                                   : static_cast<uint8_t>((ahi36 >> (i - 64)) & 1u);
            const uint8_t bi = (i < 64)
                                   ? static_cast<uint8_t>((blo >> i) & 1u)
                                   : static_cast<uint8_t>((bhi36 >> (i - 64)) & 1u);
            const uint8_t sum = static_cast<uint8_t>((ai ^ bi) ^ carry);
            const uint8_t cout = static_cast<uint8_t>((ai & bi) | (ai & carry) | (bi & carry));

            const uint8_t dut_sum = get_bit_u100(dut->sum, i);
            if (dut_sum != sum) {
                std::cerr << "[TB] dut_071 failed(" << ctx << ") at bit " << i
                          << " expected sum=" << int(sum)
                          << " got " << int(dut_sum) << std::endl;
                std::exit(EXIT_FAILURE);
            }
            carry = cout;
        }

        const uint8_t exp_cout = carry & 1u;
        if (dut->cout != exp_cout) {
            std::cerr << "[TB] dut_071 failed(" << ctx << "): expected cout="
                      << int(exp_cout) << " got " << int(dut->cout) << std::endl;
            std::exit(EXIT_FAILURE);
        }
    };

    // Basic sanity checks.
    check(0ull, 0ull, 0ull, 0ull, 0u, "zero");
    check(0ull, 0ull, 0ull, 0ull, 1u, "zero_cin1");

    const uint64_t ALL_ONES_LO = ~0ull;
    const uint64_t ALL_ONES_HI36 = 0xFFFFFFFFFull; // 36 bits of 1

    // a = max, b = 0 -> no carry out
    check(ALL_ONES_LO, ALL_ONES_HI36, 0ull, 0ull, 0u, "max_plus_zero");
    // a = max, b = 1 -> carry out = 1, sum = 0
    check(ALL_ONES_LO, ALL_ONES_HI36, 1ull, 0ull, 0u, "max_plus_one");
    // a = max, cin = 1 -> carry out = 1, sum = 0x...0 with LSB = 0
    check(ALL_ONES_LO, ALL_ONES_HI36, 0ull, 0ull, 1u, "max_plus_cin1");

    // Toggle cin 0->1->0 twice with zero operands.
    for (int rep = 0; rep < 2; ++rep) {
        check(0ull, 0ull, 0ull, 0ull, 0u, "cin0");
        check(0ull, 0ull, 0ull, 0ull, 1u, "cin1");
    }
    check(0ull, 0ull, 0ull, 0ull, 0u, "cin0_end");

    // Toggle each bit of a (with b=0, cin=0) 0->1->0 twice.
    for (int i = 0; i < 100; ++i) {
        uint64_t alo = 0ull, ahi = 0ull;
        uint64_t blo = 0ull, bhi = 0ull;
        auto setbit = [](uint64_t &lo, uint64_t &hi, int bit, bool val) {
            if (bit < 64) {
                if (val) lo |= (1ull << bit);
                else     lo &= ~(1ull << bit);
            } else {
                const int k = bit - 64;
                if (val) hi |= (1ull << k);
                else     hi &= ~(1ull << k);
            }
        };
        for (int rep = 0; rep < 2; ++rep) {
            check(alo, ahi, blo, bhi, 0u, "a_bit_off");
            setbit(alo, ahi, i, true);
            check(alo, ahi, blo, bhi, 0u, "a_bit_on");
            setbit(alo, ahi, i, false);
            check(alo, ahi, blo, bhi, 0u, "a_bit_off2");
        }
    }

    // Toggle each bit of b (with a=0, cin=0) 0->1->0 twice.
    for (int i = 0; i < 100; ++i) {
        uint64_t alo = 0ull, ahi = 0ull;
        uint64_t blo = 0ull, bhi = 0ull;
        auto setbit = [](uint64_t &lo, uint64_t &hi, int bit, bool val) {
            if (bit < 64) {
                if (val) lo |= (1ull << bit);
                else     lo &= ~(1ull << bit);
            } else {
                const int k = bit - 64;
                if (val) hi |= (1ull << k);
                else     hi &= ~(1ull << k);
            }
        };
        for (int rep = 0; rep < 2; ++rep) {
            check(alo, ahi, blo, bhi, 0u, "b_bit_off");
            setbit(blo, bhi, i, true);
            check(alo, ahi, blo, bhi, 0u, "b_bit_on");
            setbit(blo, bhi, i, false);
            check(alo, ahi, blo, bhi, 0u, "b_bit_off2");
        }
    }

    std::cout << "[TB] dut_071 passed: 100-bit adder with single carry-out" << std::endl;

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') { covPath = "coverage.dat"; }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}

