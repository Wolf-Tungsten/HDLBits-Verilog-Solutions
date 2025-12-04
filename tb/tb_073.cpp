#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_073.h"

static inline uint8_t ref(uint8_t a, uint8_t b, uint8_t c) {
    return static_cast<uint8_t>((a | b | c) & 1u);
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto ctx = std::make_unique<VerilatedContext>();
    ctx->traceEverOn(false);
    auto dut = std::make_unique<Vdut_073>(ctx.get());

    // Truth table over all 3 input bits.
    for (int v = 0; v < 8; ++v) {
        uint8_t a = static_cast<uint8_t>(v & 1u);
        uint8_t b = static_cast<uint8_t>((v >> 1) & 1u);
        uint8_t c = static_cast<uint8_t>((v >> 2) & 1u);
        dut->a = a;
        dut->b = b;
        dut->c = c;
        dut->eval();
        if (dut->out != ref(a, b, c)) {
            return EXIT_FAILURE;
        }
    }

    // Bidirectional toggles on each input.
    for (int rep = 0; rep < 2; ++rep) {
        dut->a = 0; dut->b = 0; dut->c = 0; dut->eval();
        dut->a = 1; dut->eval(); if (dut->out != 1) return EXIT_FAILURE;
        dut->a = 0; dut->eval(); if (dut->out != 0) return EXIT_FAILURE;

        dut->b = 1; dut->eval(); if (dut->out != 1) return EXIT_FAILURE;
        dut->b = 0; dut->eval(); if (dut->out != 0) return EXIT_FAILURE;

        dut->c = 1; dut->eval(); if (dut->out != 1) return EXIT_FAILURE;
        dut->c = 0; dut->eval(); if (dut->out != 0) return EXIT_FAILURE;
    }

    std::cout << "[TB] dut_073 passed: 3-input OR gate" << std::endl;

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}

