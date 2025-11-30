#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_051.h"

static inline uint8_t ref(uint8_t x1, uint8_t x2, uint8_t x3){
    return (uint8_t)((x1 & x3) | ((uint8_t)(~x3 & 1u) & x2));
}

int main(int argc, char** argv){
    Verilated::commandArgs(argc, argv);
    auto ctx = std::make_unique<VerilatedContext>();
    ctx->traceEverOn(false);
    auto dut = std::make_unique<Vdut_051>(ctx.get());

    // Truth table
    for (int v=0; v<8; ++v){
        uint8_t x1=(v>>0)&1u, x2=(v>>1)&1u, x3=(v>>2)&1u;
        dut->x1=x1; dut->x2=x2; dut->x3=x3; dut->eval();
        if (dut->f != ref(x1,x2,x3)) return EXIT_FAILURE;
    }

    // Bidirectional toggles for each input bit
    for (int rep=0; rep<2; ++rep){
        dut->x1=0; dut->x2=0; dut->x3=0; dut->eval(); if (dut->f!=ref(0,0,0)) return EXIT_FAILURE;
        dut->x1=1; dut->eval(); if (dut->f!=ref(1,0,0)) return EXIT_FAILURE;
        dut->x1=0; dut->eval(); if (dut->f!=ref(0,0,0)) return EXIT_FAILURE;
        dut->x2=1; dut->eval(); if (dut->f!=ref(0,1,0)) return EXIT_FAILURE;
        dut->x2=0; dut->eval(); if (dut->f!=ref(0,0,0)) return EXIT_FAILURE;
        dut->x3=1; dut->eval(); if (dut->f!=ref(0,0,1)) return EXIT_FAILURE;
        dut->x3=0; dut->eval(); if (dut->f!=ref(0,0,0)) return EXIT_FAILURE;
    }

    std::cout << "[TB] dut_051 passed: f = x1&x3 | ~x3&x2" << std::endl;
#if VM_COVERAGE
    const char* covPath = std::getenv("VERILATOR_COV_FILE"); if(!covPath||!*covPath) covPath="coverage.dat"; VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}

