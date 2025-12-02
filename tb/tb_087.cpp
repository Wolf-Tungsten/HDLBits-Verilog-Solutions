#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_087.h"

struct Stim87 {
    uint8_t ena;
    uint8_t d;
};

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto ctx = std::make_unique<VerilatedContext>();
    ctx->traceEverOn(false);

    auto dut = std::make_unique<Vdut_087>(ctx.get());

    uint8_t q_model = 0;

    dut->ena = 0;
    dut->d = 0;
    dut->eval();

    const Stim87 pattern[] = {
        {0u, 0u},
        {1u, 1u},
        {0u, 0u},
        {1u, 0u},
        {0u, 1u},
        {1u, 1u},
    };

    for (const auto &s : pattern) {
        dut->ena = s.ena;
        dut->d = s.d;
        dut->eval();

        if (s.ena) {
            q_model = s.d & 1u;
        }

        if (dut->q != q_model) {
            std::cerr << "[TB] dut_087 failed: ena=" << int(s.ena)
                      << " d=" << int(s.d)
                      << " expected q=" << int(q_model)
                      << " got " << int(dut->q) << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::cout << "[TB] dut_087 passed: level-sensitive latch with enable" << std::endl;

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}

