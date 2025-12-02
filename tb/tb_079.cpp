#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_079.h"

static inline uint8_t ref(uint8_t x) {
    // x is 4-bit, corresponding to [4:1]; use bits [3:0] as digits 1..4.
    uint8_t x1 = static_cast<uint8_t>((x >> 0) & 1u);
    uint8_t x2 = static_cast<uint8_t>((x >> 1) & 1u);
    uint8_t x3 = static_cast<uint8_t>((x >> 2) & 1u);
    uint8_t x4 = static_cast<uint8_t>((x >> 3) & 1u);
    // HDL: f = (x[3] & ~x[1]) | (x[2] & x[3] & x[4]) | (~x[4] & ~x[2]);
    uint8_t nx1 = static_cast<uint8_t>(~x1 & 1u);
    uint8_t nx2 = static_cast<uint8_t>(~x2 & 1u);
    uint8_t nx4 = static_cast<uint8_t>(~x4 & 1u);
    uint8_t t1 = static_cast<uint8_t>(x3 & nx1);
    uint8_t t2 = static_cast<uint8_t>(x2 & x3 & x4);
    uint8_t t3 = static_cast<uint8_t>(nx4 & nx2);
    return static_cast<uint8_t>((t1 | t2 | t3) & 1u);
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto ctx = std::make_unique<VerilatedContext>();
    ctx->traceEverOn(false);
    auto dut = std::make_unique<Vdut_079>(ctx.get());

    // Truth table for all 4-bit x.
    for (int v = 0; v < 16; ++v) {
        uint8_t xv = static_cast<uint8_t>(v & 0xFu);
        dut->x = xv;
        dut->eval();
        if (dut->f != ref(xv)) {
            return EXIT_FAILURE;
        }
    }

    // Toggle each bit of x 0->1->0 twice.
    for (int rep = 0; rep < 2; ++rep) {
        dut->x = 0; dut->eval();
        for (int bit = 0; bit < 4; ++bit) {
            uint8_t v = static_cast<uint8_t>(1u << bit);
            dut->x = v; dut->eval();
            if (dut->f != ref(v)) return EXIT_FAILURE;
            dut->x = 0; dut->eval();
            if (dut->f != ref(0)) return EXIT_FAILURE;
        }
    }

    std::cout << "[TB] dut_079 passed: 4-input logic on x[4:1]" << std::endl;

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}

