#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_059.h"

static inline uint8_t bit(uint8_t v, int i){ return (v>>i)&1u; }

int main(int argc, char** argv){
    Verilated::commandArgs(argc, argv);
    auto ctx = std::make_unique<VerilatedContext>();
    ctx->traceEverOn(false);
    auto dut = std::make_unique<Vdut_059>(ctx.get());

    // Check all 16 patterns
    for (int v=0; v<16; ++v){
        dut->in = v & 0xFu; dut->eval();
        // out_both[i] = in[i] & in[i+1]  (i=0..2)
        for (int i=0;i<3;++i){ if (bit(dut->out_both, i) != (uint8_t)(bit(v,i)&bit(v,i+1))) return EXIT_FAILURE; }
        // out_any has declared range [3:1], Verilator packs bit1..3 into CData bits 0..2
        // so access out_any[ i+1 ] as CData bit i
        for (int i=0;i<3;++i){ if (bit(dut->out_any, i) != (uint8_t)(bit(v,i)|bit(v,i+1))) return EXIT_FAILURE; }
        // out_different[i] = in[i]^in[i+1] (i=0..2), out_different[3] = in[3]^in[0]
        for (int i=0;i<3;++i){ if (bit(dut->out_different, i) != (uint8_t)(bit(v,i)^bit(v,i+1))) return EXIT_FAILURE; }
        if (bit(dut->out_different, 3) != (uint8_t)(bit(v,3)^bit(v,0))) return EXIT_FAILURE;
    }

    // Drive toggles to ensure 0->1->0 per relevant bit logic
    // For each pair i, i+1: set in[i+1]=1 and toggle in[i]; then set in[i]=1 and toggle in[i+1]
    for (int i=0;i<3;++i){
        uint8_t in=0;
        // Toggle in[i] with neighbor 1
        for (int rep=0; rep<2; ++rep){
            in = (uint8_t)(1u<<(i+1)); dut->in=in; dut->eval();
            in |= (uint8_t)(1u<<i);    dut->in=in; dut->eval();
            in &= (uint8_t)~(1u<<i);   dut->in=in; dut->eval();
        }
        // Toggle in[i+1] with neighbor 1
        for (int rep=0; rep<2; ++rep){
            in = (uint8_t)(1u<<i);     dut->in=in; dut->eval();
            in |= (uint8_t)(1u<<(i+1)); dut->in=in; dut->eval();
            in &= (uint8_t)~(1u<<(i+1)); dut->in=in; dut->eval();
        }
    }
    // Wrap-around pair (3,0)
    for (int rep=0; rep<2; ++rep){
        uint8_t in=1u<<0; dut->in=in; dut->eval();
        in |= (1u<<3);    dut->in=in; dut->eval();
        in &= ~(1u<<3);   dut->in=in; dut->eval();
    }

    std::cout << "[TB] dut_059 passed: neighbor pair ops on 4-bit ring" << std::endl;
#if VM_COVERAGE
    const char* covPath = std::getenv("VERILATOR_COV_FILE"); if(!covPath||!*covPath) covPath="coverage.dat"; VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}
