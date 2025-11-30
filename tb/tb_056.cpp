#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_056.h"

static inline void check(Vdut_056* d, uint8_t ring, uint8_t vib){
    d->ring=ring; d->vibrate_mode=vib; d->eval();
    uint8_t r = ring & (uint8_t)(~vib & 1u);
    uint8_t m = ring & vib;
    if (d->ringer!=r || d->motor!=m){ std::exit(EXIT_FAILURE);} }

int main(int argc, char** argv){
    Verilated::commandArgs(argc, argv);
    auto ctx = std::make_unique<VerilatedContext>();
    ctx->traceEverOn(false);
    auto dut = std::make_unique<Vdut_056>(ctx.get());

    // Truth table
    check(dut.get(),0,0); check(dut.get(),0,1); check(dut.get(),1,0); check(dut.get(),1,1);
    // Bidirectional toggle sequences
    for (int rep=0; rep<2; ++rep){
        check(dut.get(),0,0); check(dut.get(),1,0); check(dut.get(),0,0);
        check(dut.get(),0,1); check(dut.get(),0,0);
        check(dut.get(),1,1); check(dut.get(),0,1); check(dut.get(),1,1);
    }

    std::cout << "[TB] dut_056 passed: ringer/motor selection" << std::endl;
#if VM_COVERAGE
    const char* covPath = std::getenv("VERILATOR_COV_FILE"); if(!covPath||!*covPath) covPath="coverage.dat"; VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}

