#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_058.h"

static inline uint8_t pop3(uint8_t v){ return (uint8_t)((v&1u)+((v>>1)&1u)+((v>>2)&1u)); }

int main(int argc, char** argv){
    Verilated::commandArgs(argc, argv);
    auto ctx = std::make_unique<VerilatedContext>();
    ctx->traceEverOn(false);
    auto dut = std::make_unique<Vdut_058>(ctx.get());

    for (int v=0; v<8; ++v){
        dut->in = v & 7u; dut->eval();
        if (dut->out != pop3((uint8_t)v)) return EXIT_FAILURE;
    }
    // Toggle each input bit twice
    for (int i=0;i<3;++i){
        for (int rep=0; rep<2; ++rep){
            dut->in=0; dut->eval(); if (dut->out!=0) return EXIT_FAILURE;
            dut->in=(1u<<i); dut->eval(); if (dut->out!=1) return EXIT_FAILURE;
            dut->in=0; dut->eval(); if (dut->out!=0) return EXIT_FAILURE;
        }
    }

    std::cout << "[TB] dut_058 passed: popcount3" << std::endl;
#if VM_COVERAGE
    const char* covPath = std::getenv("VERILATOR_COV_FILE"); if(!covPath||!*covPath) covPath="coverage.dat"; VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}

