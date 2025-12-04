#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_086.h"

struct Stim86 {
    uint8_t resetn;
    uint16_t d;
    uint8_t byteena;
};

static inline void tick(Vdut_086 *dut, VerilatedContext *ctx) {
    dut->clk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->clk = 1;
    dut->eval();
    ctx->timeInc(1);
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto ctx = std::make_unique<VerilatedContext>();
    ctx->traceEverOn(false);

    auto dut = std::make_unique<Vdut_086>(ctx.get());

    uint16_t q_model = 0;

    dut->clk = 0;
    dut->resetn = 0;
    dut->byteena = 0;
    dut->d = 0;
    dut->eval();

    const Stim86 pattern[] = {
        {0u, 0xAAAAu, 0x3u}, // reset active: q -> 0
        {1u, 0x1234u, 0x1u}, // low byte update
        {1u, 0xDEADu, 0x2u}, // high byte update
        {1u, 0xBEEFu, 0x3u}, // both bytes update
        {1u, 0x0000u, 0x0u}, // no bytes update
        {0u, 0xFFFFu, 0x3u}, // another reset to 0
        {1u, 0x00FFu, 0x1u}, // update low only
        {1u, 0xFF00u, 0x2u}, // update high only
    };

    for (const auto &s : pattern) {
        dut->resetn = s.resetn;
        dut->d = s.d;
        dut->byteena = s.byteena;
        tick(dut.get(), ctx.get());

        if (!s.resetn) {
            q_model = 0;
        } else {
            // Byte 0
            if (s.byteena & 0x1u) {
                q_model = static_cast<uint16_t>((q_model & 0xFF00u) | (s.d & 0x00FFu));
            }
            // Byte 1
            if (s.byteena & 0x2u) {
                q_model = static_cast<uint16_t>((q_model & 0x00FFu) | (s.d & 0xFF00u));
            }
        }

        if (dut->q != q_model) {
            std::cerr << "[TB] dut_086 failed: resetn=" << int(s.resetn)
                      << " byteena=0x" << std::hex << int(s.byteena)
                      << " d=0x" << int(s.d)
                      << " expected q=0x" << int(q_model)
                      << " got 0x" << int(dut->q) << std::dec << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::cout << "[TB] dut_086 passed: byte-enable 16-bit register" << std::endl;

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}

