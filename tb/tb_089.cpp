#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_089.h"

struct Stim89 {
    uint8_t r;
    uint8_t d;
};

static inline void tick(Vdut_089 *dut, VerilatedContext *ctx) {
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

    auto dut = std::make_unique<Vdut_089>(ctx.get());

    uint8_t q_model = 0;

    dut->clk = 0;
    dut->r = 0;
    dut->d = 0;
    dut->eval();

    const Stim89 pattern[] = {
        {1u, 1u},
        {0u, 1u},
        {0u, 0u},
        {1u, 0u},
        {0u, 1u},
    };

    for (const auto &s : pattern) {
        dut->r = s.r;
        dut->d = s.d;
        tick(dut.get(), ctx.get());

        if (s.r) {
            q_model = 0;
        } else {
            q_model = s.d & 1u;
        }

        if (dut->q != q_model) {
            std::cerr << "[TB] dut_089 failed: r=" << int(s.r)
                      << " d=" << int(s.d)
                      << " expected q=" << int(q_model)
                      << " got " << int(dut->q) << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::cout << "[TB] dut_089 passed: sync reset DFF" << std::endl;

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}

