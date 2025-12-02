#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_082.h"

static inline void tick(Vdut_082 *dut, VerilatedContext *ctx) {
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

    auto dut = std::make_unique<Vdut_082>(ctx.get());

    uint8_t q_model = 0;

    dut->clk = 0;
    dut->d = 0;
    dut->eval();

    const uint8_t pattern[] = {0x00u, 0xFFu, 0x00u, 0xAAu, 0x55u, 0x00u};
    for (uint8_t d_val : pattern) {
        dut->d = d_val;
        tick(dut.get(), ctx.get());

        q_model = d_val;
        if (dut->q != q_model) {
            std::cerr << "[TB] dut_082 failed: d=0x" << std::hex << int(d_val)
                      << " expected q=0x" << int(q_model)
                      << " got 0x" << int(dut->q) << std::dec << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::cout << "[TB] dut_082 passed: 8-bit register" << std::endl;

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}

