#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_085.h"

static inline void tick(Vdut_085 *dut, VerilatedContext *ctx) {
    dut->clk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->clk = 1;
    dut->eval();
    ctx->timeInc(1);
}

static inline void apply_async_reset(Vdut_085 *dut, uint8_t &q_model) {
    dut->areset = 1;
    dut->eval();
    dut->areset = 0;
    dut->eval();
    q_model = 0;
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto ctx = std::make_unique<VerilatedContext>();
    ctx->traceEverOn(false);

    auto dut = std::make_unique<Vdut_085>(ctx.get());

    uint8_t q_model = 0;

    dut->clk = 0;
    dut->areset = 0;
    dut->d = 0;
    dut->eval();

    // Initial async reset
    apply_async_reset(dut.get(), q_model);

    const uint8_t pattern[] = {0x00u, 0xFFu, 0xAAu, 0x55u};
    for (uint8_t d_val : pattern) {
        dut->d = d_val;
        dut->areset = 0;
        tick(dut.get(), ctx.get());

        q_model = d_val;
        if (dut->q != q_model) {
            std::cerr << "[TB] dut_085 failed normal cycle: d=0x" << std::hex << int(d_val)
                      << " expected q=0x" << int(q_model)
                      << " got 0x" << int(dut->q) << std::dec << std::endl;
            return EXIT_FAILURE;
        }

        // Occasionally assert async reset between cycles.
        apply_async_reset(dut.get(), q_model);
        if (dut->q != q_model) {
            std::cerr << "[TB] dut_085 failed after async reset: expected q=0"
                      << " got " << int(dut->q) << std::endl;
            return EXIT_FAILURE;
        }
    }

    // Exercise areset coincident with a clock edge.
    dut->d = 0x3Cu;
    dut->areset = 1;
    tick(dut.get(), ctx.get());
    q_model = 0;
    if (dut->q != q_model) {
        std::cerr << "[TB] dut_085 failed coincident reset: expected q=0"
                  << " got " << int(dut->q) << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "[TB] dut_085 passed: async reset 8-bit register" << std::endl;

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}

