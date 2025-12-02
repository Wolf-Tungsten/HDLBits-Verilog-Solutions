#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_081.h"

static inline void tick(Vdut_081 *dut, VerilatedContext *ctx) {
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

    auto dut = std::make_unique<Vdut_081>(ctx.get());

    // Model for q: captures d on each rising edge.
    uint8_t q_model = 0;

    // Initial state
    dut->clk = 0;
    dut->d = 0;
    dut->eval();

    const uint8_t pattern[] = {0, 1, 1, 0, 0, 1, 0};
    for (uint8_t d_val : pattern) {
        dut->d = d_val;
        tick(dut.get(), ctx.get());

        q_model = d_val & 1u;
        if (dut->q != q_model) {
            std::cerr << "[TB] dut_081 failed: d=" << int(d_val)
                      << " expected q=" << int(q_model)
                      << " got " << int(dut->q) << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::cout << "[TB] dut_081 passed: simple D flip-flop" << std::endl;

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}

