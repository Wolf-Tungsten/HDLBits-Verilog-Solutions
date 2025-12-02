#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_076.h"

static inline uint8_t ref(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    uint8_t na = static_cast<uint8_t>(~a & 1u);
    uint8_t nb = static_cast<uint8_t>(~b & 1u);
    uint8_t nc = static_cast<uint8_t>(~c & 1u);
    uint8_t nd = static_cast<uint8_t>(~d & 1u);
    uint8_t t0 = static_cast<uint8_t>(na & b & nc & nd);
    uint8_t t1 = static_cast<uint8_t>(a & nb & nc & nd);
    uint8_t t2 = static_cast<uint8_t>(na & nb & nc & d);
    uint8_t t3 = static_cast<uint8_t>(a & b & nc & d);
    uint8_t t4 = static_cast<uint8_t>(na & b & c & d);
    uint8_t t5 = static_cast<uint8_t>(a & nb & c & d);
    uint8_t t6 = static_cast<uint8_t>(na & nb & c & nd);
    uint8_t t7 = static_cast<uint8_t>(a & b & c & nd);
    return static_cast<uint8_t>((t0 | t1 | t2 | t3 | t4 | t5 | t6 | t7) & 1u);
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto ctx = std::make_unique<VerilatedContext>();
    ctx->traceEverOn(false);
    auto dut = std::make_unique<Vdut_076>(ctx.get());

    // Truth table over all 4 input bits.
    for (int v = 0; v < 16; ++v) {
        uint8_t a = static_cast<uint8_t>(v & 1u);
        uint8_t b = static_cast<uint8_t>((v >> 1) & 1u);
        uint8_t c = static_cast<uint8_t>((v >> 2) & 1u);
        uint8_t d = static_cast<uint8_t>((v >> 3) & 1u);
        dut->a = a;
        dut->b = b;
        dut->c = c;
        dut->d = d;
        dut->eval();
        if (dut->out != ref(a, b, c, d)) {
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

    std::cout << "[TB] dut_076 passed: 4-input sum-of-products logic" << std::endl;

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}

