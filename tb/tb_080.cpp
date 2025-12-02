#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_080.h"

static inline uint8_t ref0(uint8_t c, uint8_t d) { return static_cast<uint8_t>((c | d) & 1u); }
static inline uint8_t ref1(uint8_t /*c*/, uint8_t /*d*/) { return 0u; }
static inline uint8_t ref2(uint8_t /*c*/, uint8_t d) { return static_cast<uint8_t>((~d) & 1u); }
static inline uint8_t ref3(uint8_t c, uint8_t d) { return static_cast<uint8_t>((c & d) & 1u); }

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto ctx = std::make_unique<VerilatedContext>();
    ctx->traceEverOn(false);
    auto dut = std::make_unique<Vdut_080>(ctx.get());

    // Truth table over c,d in {0,1}.
    for (int v = 0; v < 4; ++v) {
        uint8_t c = static_cast<uint8_t>(v & 1u);
        uint8_t d = static_cast<uint8_t>((v >> 1) & 1u);
        dut->c = c;
        dut->d = d;
        dut->eval();
        const uint8_t m = dut->mux_in;
        if ( ((m >> 0) & 1u) != ref0(c,d) ||
             ((m >> 1) & 1u) != ref1(c,d) ||
             ((m >> 2) & 1u) != ref2(c,d) ||
             ((m >> 3) & 1u) != ref3(c,d) ) {
            return EXIT_FAILURE;
        }
    }

    // Bidirectional toggles on c and d 0->1->0 twice.
    for (int rep = 0; rep < 2; ++rep) {
        dut->c = 0; dut->d = 0; dut->eval();
        dut->c = 1; dut->eval();
        dut->c = 0; dut->eval();
        dut->d = 1; dut->eval();
        dut->d = 0; dut->eval();
    }

    // Explicitly toggle mux_in[3] (c & d) through 1->0 as well.
    for (int rep = 0; rep < 2; ++rep) {
        dut->c = 1; dut->d = 1; dut->eval();
        dut->c = 0; dut->d = 0; dut->eval();
    }

    std::cout << "[TB] dut_080 passed: 2-input mux_in pattern generator" << std::endl;

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}
