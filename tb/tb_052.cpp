#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_052.h"

static inline uint8_t ref(uint8_t A, uint8_t B){ return (uint8_t)((A & 3u) == (B & 3u)); }

int main(int argc, char** argv){
    Verilated::commandArgs(argc, argv);
    auto ctx = std::make_unique<VerilatedContext>();
    ctx->traceEverOn(false);
    auto dut = std::make_unique<Vdut_052>(ctx.get());

    // Truth table over all 2-bit inputs
    for (int a=0;a<4;++a){
        for (int b=0;b<4;++b){
            dut->A=a; dut->B=b; dut->eval();
            if (dut->z != ref(a,b)) return EXIT_FAILURE;
        }
    }

    // Bidirectional toggles on bits of A and B
    auto check=[&](uint8_t a, uint8_t b){ dut->A=a; dut->B=b; dut->eval(); return dut->z == ref(a,b); };
    for (int rep=0; rep<2; ++rep){
        if (!check(0,0)) return EXIT_FAILURE; // baseline
        if (!check(1,0)) return EXIT_FAILURE; // A[0]
        if (!check(0,0)) return EXIT_FAILURE;
        if (!check(2,0)) return EXIT_FAILURE; // A[1]
        if (!check(0,0)) return EXIT_FAILURE;
        if (!check(0,1)) return EXIT_FAILURE; // B[0]
        if (!check(0,0)) return EXIT_FAILURE;
        if (!check(0,2)) return EXIT_FAILURE; // B[1]
        if (!check(0,0)) return EXIT_FAILURE;
    }

    std::cout << "[TB] dut_052 passed: z = (A==B)" << std::endl;
#if VM_COVERAGE
    const char* covPath = std::getenv("VERILATOR_COV_FILE"); if(!covPath||!*covPath) covPath="coverage.dat"; VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}

