#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_055.h"

static inline uint8_t ref(uint8_t x, uint8_t y){
    uint8_t t1 = ((x^y)&1u) & (x&1u);
    uint8_t t2 = (uint8_t)(~(x^y) & 1u);
    return (uint8_t)((t1 | t2) ^ (t1 & t2));
}

int main(int argc, char** argv){
    Verilated::commandArgs(argc, argv);
    auto ctx = std::make_unique<VerilatedContext>();
    ctx->traceEverOn(false);
    auto dut = std::make_unique<Vdut_055>(ctx.get());

    for (int v=0; v<4; ++v){
        uint8_t x=(v>>0)&1u, y=(v>>1)&1u;
        dut->x=x; dut->y=y; dut->eval();
        if (dut->z != ref(x,y)) return EXIT_FAILURE;
    }
    // Toggle both inputs twice
    for (int rep=0; rep<2; ++rep){
        dut->x=0; dut->y=0; dut->eval(); if (dut->z!=ref(0,0)) return EXIT_FAILURE;
        dut->x=1; dut->eval(); if (dut->z!=ref(1,0)) return EXIT_FAILURE;
        dut->x=0; dut->eval(); if (dut->z!=ref(0,0)) return EXIT_FAILURE;
        dut->y=1; dut->eval(); if (dut->z!=ref(0,1)) return EXIT_FAILURE;
        dut->y=0; dut->eval(); if (dut->z!=ref(0,0)) return EXIT_FAILURE;
    }

    std::cout << "[TB] dut_055 passed: expression verified" << std::endl;
#if VM_COVERAGE
    const char* covPath = std::getenv("VERILATOR_COV_FILE"); if(!covPath||!*covPath) covPath="coverage.dat"; VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}

