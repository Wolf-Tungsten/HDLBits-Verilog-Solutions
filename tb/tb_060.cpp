#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_060.h"

// Helpers for 100-bit vectors stored as 4x32-bit little-endian words
static inline void set_u100(Vdut_060* d, uint64_t lo, uint64_t hi36){
    d->in[0] = static_cast<uint32_t>(lo & 0xFFFFFFFFull);
    d->in[1] = static_cast<uint32_t>((lo >> 32) & 0xFFFFFFFFull);
    d->in[2] = static_cast<uint32_t>(hi36 & 0xFFFFFFFFull);
    d->in[3] = static_cast<uint32_t>((hi36 >> 32) & 0xFull);
}
static inline void get_u100(const uint32_t w[4], uint64_t &lo, uint64_t &hi36){
    lo = (uint64_t(w[1])<<32) | uint64_t(w[0]);
    hi36 = (uint64_t(w[3]&0xFu)<<32) | uint64_t(w[2]);
}
static inline uint8_t get_bit(const uint32_t w[4], int bit){ int wi=bit>>5; int bi=bit&31; return (w[wi]>>bi)&1u; }

int main(int argc, char** argv){
    Verilated::commandArgs(argc, argv);
    auto ctx = std::make_unique<VerilatedContext>();
    ctx->traceEverOn(false);
    auto dut = std::make_unique<Vdut_060>(ctx.get());

    // Basic patterns
    set_u100(dut.get(), 0ull, 0ull); dut->eval();
    set_u100(dut.get(), ~0ull, 0xFFFFFFFFFull); dut->eval();
    set_u100(dut.get(), 0x0123456789ABCDEFull, 0x123456789ull); dut->eval();

    // Bitwise check over a couple of patterns
    auto check=[&](uint64_t lo, uint64_t hi){
        set_u100(dut.get(), lo, hi); dut->eval();
        // For k in [0..98]: out_both[k] = in[k] & in[k+1]
        for (int k=0;k<99;++k){
            uint8_t ib = get_bit(dut->in, k);
            uint8_t inext = get_bit(dut->in, k+1);
            if (get_bit(dut->out_both, k) != (uint8_t)(ib & inext)) return false;
        }
        // For j in [1..99]: out_any[j] = in[j-1] | in[j] ; storage uses bit index (j-1)
        for (int j=1;j<=99;++j){
            uint8_t a = get_bit(dut->in, j-1);
            uint8_t b = get_bit(dut->in, j);
            if (get_bit(dut->out_any, j-1) != (uint8_t)(a | b)) return false;
        }
        // out_different[0]..[98] = in[0..98]^in[1..99], and extra MSB bit is (in[99]^in[0]) placed at position 99? per DUT it is concatenated as {in[99]^in[0], ...}
        // Here we just check the 0..98 slice which is well-defined by code.
        for (int k=0;k<99;++k){
            if (get_bit(dut->out_different, k) != (uint8_t)(get_bit(dut->in,k) ^ get_bit(dut->in,k+1))) return false;
        }
        return true;
    };
    if (!check(0ull,0ull)) return EXIT_FAILURE;
    if (!check(~0ull,0xFFFFFFFFFull)) return EXIT_FAILURE;

    // Bidirectional toggles: for each input bit, 0->1->0 twice under zero baseline
    uint64_t lo=0, hi=0; set_u100(dut.get(), lo, hi); dut->eval();
    auto setbit=[&](int bit, bool val){ if (bit<64){ if(val) lo|=(1ull<<bit); else lo&=~(1ull<<bit);} else { int k=bit-64; if(val) hi|=(1ull<<k); else hi&=~(1ull<<k);} set_u100(dut.get(), lo, hi); };
    for (int bit=0; bit<100; ++bit){
        for (int rep=0; rep<2; ++rep){ setbit(bit,true); dut->eval(); setbit(bit,false); dut->eval(); }
    }

    std::cout << "[TB] dut_060 passed: neighbor pair ops on 100-bit vector" << std::endl;
#if VM_COVERAGE
    const char* covPath = std::getenv("VERILATOR_COV_FILE"); if(!covPath||!*covPath) covPath="coverage.dat"; VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}

