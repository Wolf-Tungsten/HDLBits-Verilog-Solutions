#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_084.h"

struct Stim84 {
    uint8_t reset;
    uint8_t d;
};

static inline void tick_negedge(Vdut_084 *dut, VerilatedContext *ctx) {
    // Falling edge: 1 -> 0 triggers the flop.
    dut->clk = 1;
    dut->eval();
    ctx->timeInc(1);
    dut->clk = 0;
    dut->eval();
    ctx->timeInc(1);
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto ctx = std::make_unique<VerilatedContext>();
    ctx->traceEverOn(false);

    auto dut = std::make_unique<Vdut_084>(ctx.get());

    uint8_t q_model = 0x34u;

    dut->clk = 1;
    dut->reset = 1;
    dut->d = 0;
    dut->eval();

    const Stim84 pattern[] = {
        {1u, 0x00u},
        {0u, 0x12u},
        {0u, 0xA5u},
        {1u, 0xFFu},
        {0u, 0x5Au},
    };

    for (const auto &s : pattern) {
        dut->reset = s.reset;
        dut->d = s.d;
        tick_negedge(dut.get(), ctx.get());

        if (s.reset) {
            q_model = 0x34u;
        } else {
            q_model = s.d;
        }

        if (dut->q != q_model) {
            std::cerr << "[TB] dut_084 failed: reset=" << int(s.reset)
                      << " d=0x" << std::hex << int(s.d)
                      << " expected q=0x" << int(q_model)
                      << " got 0x" << int(dut->q) << std::dec << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::cout << "[TB] dut_084 passed: negedge clock with reset to 0x34" << std::endl;

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}

