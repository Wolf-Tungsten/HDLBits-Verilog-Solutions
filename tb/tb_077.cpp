#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_077.h"

// Returns 0/1 for specified minterms; -1 for unreachable don't-care cases.
static inline int8_t expected_out(int v) {
    switch (v) {
        case 0:
        case 1:
        case 4:
        case 5:
        case 6:
        case 9:
        case 10:
        case 13:
        case 14:
            return 0;
        case 2:
        case 7:
        case 15:
            return 1;
        default:
            return -1;
    }
}

static inline uint8_t ref_sop(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    // out_sop = (c & d) | (~a & ~b & c);
    uint8_t na = static_cast<uint8_t>(~a & 1u);
    uint8_t nb = static_cast<uint8_t>(~b & 1u);
    return static_cast<uint8_t>(((c & d) | (na & nb & c)) & 1u);
}

static inline uint8_t ref_pos(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    // out_pos = c & (~a | b) & (d | ~b);
    uint8_t na = static_cast<uint8_t>(~a & 1u);
    uint8_t nb = static_cast<uint8_t>(~b & 1u);
    uint8_t term1 = c;
    uint8_t term2 = static_cast<uint8_t>((na | b) & 1u);
    uint8_t term3 = static_cast<uint8_t>((d | nb) & 1u);
    return static_cast<uint8_t>((term1 & term2 & term3) & 1u);
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto ctx = std::make_unique<VerilatedContext>();
    ctx->traceEverOn(false);
    auto dut = std::make_unique<Vdut_077>(ctx.get());

    // Truth table: both forms must implement the same function.
    for (int v = 0; v < 16; ++v) {
        uint8_t d = static_cast<uint8_t>(v & 1u);
        uint8_t c = static_cast<uint8_t>((v >> 1) & 1u);
        uint8_t b = static_cast<uint8_t>((v >> 2) & 1u);
        uint8_t a = static_cast<uint8_t>((v >> 3) & 1u);
        uint8_t sop = ref_sop(a, b, c, d);
        uint8_t pos = ref_pos(a, b, c, d);
        dut->a = a;
        dut->b = b;
        dut->c = c;
        dut->d = d;
        dut->eval();
        int8_t exp = expected_out(v);
        if (exp < 0) {
            // Unreachable / don't care combinations should not fail the testbench.
            continue;
        }
        uint8_t expected = static_cast<uint8_t>(exp);
        if (sop != expected || pos != expected) {
            // Reference equations must match the specified truth table entries.
            std::cerr << "[TB] Reference mismatch at v=" << v
                      << " (a b c d = " << int(a) << int(b) << int(c) << int(d)
                      << "): expected " << int(expected)
                      << " got sop=" << int(sop) << " pos=" << int(pos) << std::endl;
            return EXIT_FAILURE;
        }
        if (dut->out_sop != expected || dut->out_pos != expected) {
            std::cerr << "[TB] DUT mismatch at v=" << v
                      << " (a b c d = " << int(a) << int(b) << int(c) << int(d)
                      << "): expected " << int(expected)
                      << " got sop=" << int(dut->out_sop)
                      << " pos=" << int(dut->out_pos) << std::endl;
            return EXIT_FAILURE;
        }
    }

    // Toggle each input bit 0->1->0 twice.
    for (int rep = 0; rep < 2; ++rep) {
        dut->a = 0; dut->b = 0; dut->c = 0; dut->d = 0; dut->eval();
        dut->a = 1; dut->eval(); dut->a = 0; dut->eval();
        dut->b = 1; dut->eval(); dut->b = 0; dut->eval();
        dut->c = 1; dut->eval(); dut->c = 0; dut->eval();
        dut->d = 1; dut->eval(); dut->d = 0; dut->eval();
    }

    std::cout << "[TB] dut_077 passed: SOP/POS equivalence" << std::endl;

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}
