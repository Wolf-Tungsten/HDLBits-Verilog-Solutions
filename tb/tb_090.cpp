#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_090.h"

static inline void tick(Vdut_090 *dut, VerilatedContext *ctx) {
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

    auto dut = std::make_unique<Vdut_090>(ctx.get());

    uint8_t out_model = 0;

    dut->clk = 0;
    dut->in = 0;
    dut->eval();

    const uint8_t pattern[] = {0u, 1u, 1u, 0u, 1u, 0u};
    for (uint8_t in_val : pattern) {
        dut->in = in_val;
        tick(dut.get(), ctx.get());

        out_model = static_cast<uint8_t>((out_model ^ (in_val & 1u)) & 1u);

        if (dut->out != out_model) {
            std::cerr << "[TB] dut_090 failed: in=" << int(in_val)
                      << " expected out=" << int(out_model)
                      << " got " << int(dut->out) << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::cout << "[TB] dut_090 passed: XOR accumulator" << std::endl;

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}

