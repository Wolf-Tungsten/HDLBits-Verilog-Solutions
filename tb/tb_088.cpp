#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_088.h"

static inline void tick(Vdut_088 *dut, VerilatedContext *ctx) {
    dut->clk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->clk = 1;
    dut->eval();
    ctx->timeInc(1);
}

static inline void apply_async_reset(Vdut_088 *dut, uint8_t &q_model) {
    dut->ar = 1;
    dut->eval();
    dut->ar = 0;
    dut->eval();
    q_model = 0;
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto ctx = std::make_unique<VerilatedContext>();
    ctx->traceEverOn(false);

    auto dut = std::make_unique<Vdut_088>(ctx.get());

    uint8_t q_model = 0;

    dut->clk = 0;
    dut->ar = 0;
    dut->d = 0;
    dut->eval();

    apply_async_reset(dut.get(), q_model);

    const uint8_t pattern[] = {0u, 1u, 0u, 1u, 1u, 0u};
    for (uint8_t d_val : pattern) {
        dut->d = d_val;
        dut->ar = 0;
        tick(dut.get(), ctx.get());

        q_model = d_val & 1u;
        if (dut->q != q_model) {
            std::cerr << "[TB] dut_088 failed normal cycle: d=" << int(d_val)
                      << " expected q=" << int(q_model)
                      << " got " << int(dut->q) << std::endl;
            return EXIT_FAILURE;
        }

        apply_async_reset(dut.get(), q_model);
        if (dut->q != q_model) {
            std::cerr << "[TB] dut_088 failed after async reset: expected q=0"
                      << " got " << int(dut->q) << std::endl;
            return EXIT_FAILURE;
        }
    }

    // Reset coincident with a clock edge.
    dut->d = 1;
    dut->ar = 1;
    tick(dut.get(), ctx.get());
    q_model = 0;
    if (dut->q != q_model) {
        std::cerr << "[TB] dut_088 failed coincident reset" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "[TB] dut_088 passed: async reset DFF" << std::endl;

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}

