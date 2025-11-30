#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_057.h"

static inline void check(Vdut_057* d, uint8_t too_cold, uint8_t too_hot, uint8_t mode, uint8_t fan_on){
    d->too_cold=too_cold; d->too_hot=too_hot; d->mode=mode; d->fan_on=fan_on; d->eval();
    uint8_t heater = (mode & too_cold) & 1u;
    uint8_t aircon = ((uint8_t)(~mode & 1u) & too_hot) & 1u;
    uint8_t fan = (fan_on | (heater | aircon)) & 1u;
    if (d->heater!=heater || d->aircon!=aircon || d->fan!=fan) std::exit(EXIT_FAILURE);
}

int main(int argc, char** argv){
    Verilated::commandArgs(argc, argv);
    auto ctx = std::make_unique<VerilatedContext>();
    ctx->traceEverOn(false);
    auto dut = std::make_unique<Vdut_057>(ctx.get());

    // Iterate all 16 combinations
    for (int v=0; v<16; ++v){
        check(dut.get(), (v>>0)&1u, (v>>1)&1u, (v>>2)&1u, (v>>3)&1u);
    }
    // Toggle each input twice around baseline
    for (int rep=0; rep<2; ++rep){
        check(dut.get(),0,0,0,0);
        check(dut.get(),1,0,0,0);
        check(dut.get(),0,0,0,0);
        check(dut.get(),0,1,0,0);
        check(dut.get(),0,0,0,0);
        check(dut.get(),0,0,1,0);
        check(dut.get(),0,0,0,0);
        check(dut.get(),0,0,0,1);
        check(dut.get(),0,0,0,0);
    }

    std::cout << "[TB] dut_057 passed: thermostat control" << std::endl;
#if VM_COVERAGE
    const char* covPath = std::getenv("VERILATOR_COV_FILE"); if(!covPath||!*covPath) covPath="coverage.dat"; VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}

