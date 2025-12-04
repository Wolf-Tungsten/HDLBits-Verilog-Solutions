#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_083.h"

struct Stim {
    uint8_t reset;
    uint8_t d;
};

static inline void tick(Vdut_083 *dut, VerilatedContext *ctx) {
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

    auto dut = std::make_unique<Vdut_083>(ctx.get());

    uint8_t q_model = 0;

    dut->clk = 0;
    dut->reset = 1;
    dut->d = 0;
    dut->eval();

    const Stim pattern[] = {
        {1u, 0xAAu},
        {0u, 0xAAu},
        {0u, 0x55u},
        {1u, 0xFFu},
        {0u, 0x00u},
    };

    for (const auto &s : pattern) {
        dut->reset = s.reset;
        dut->d = s.d;
        tick(dut.get(), ctx.get());

        if (s.reset) {
            q_model = 0u;
        } else {
            q_model = s.d;
        }

        if (dut->q != q_model) {
            std::cerr << "[TB] dut_083 failed: reset=" << int(s.reset)
                      << " d=0x" << std::hex << int(s.d)
                      << " expected q=0x" << int(q_model)
                      << " got 0x" << int(dut->q) << std::dec << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::cout << "[TB] dut_083 passed: sync reset 8-bit register" << std::endl;

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}

