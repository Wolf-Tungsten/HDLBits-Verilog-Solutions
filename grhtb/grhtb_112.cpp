#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static std::uint32_t step_lfsr(std::uint32_t q)
{
    const std::uint32_t q0 = q & 1u;
    const std::uint32_t bit22 = (q >> 22) & 1u;
    const std::uint32_t bit2 = (q >> 2) & 1u;
    const std::uint32_t bit1 = (q >> 1) & 1u;

    std::uint32_t next = 0u;
    next |= q0 << 31;
    next |= (q & 0xFF800000u) >> 1;
    next |= (bit22 ^ q0) << 21;
    next |= (q & 0x003FFFF8u) >> 1;
    next |= (bit2 ^ q0) << 1;
    next |= bit1 ^ q0;
    return next;
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.reset = true;
    sim.eval();

    std::uint32_t qModel = 0u;
    sim.clk = true;
    sim.eval();
    qModel = 0x1u;
    if (sim.q != qModel) {
        std::cerr << "[GrhTB] dut_112 failed(reset posedge)\n";
        return EXIT_FAILURE;
    }
    sim.clk = false;
    sim.eval();
    if (sim.q != qModel) {
        std::cerr << "[GrhTB] dut_112 failed(reset negedge)\n";
        return EXIT_FAILURE;
    }

    sim.reset = false;
    for (int step = 0; step < 128; ++step) {
        sim.clk = true;
        sim.eval();
        qModel = step_lfsr(qModel);
        if (sim.q != qModel) {
            std::cerr << "[GrhTB] dut_112 failed(step " << step << ", posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (sim.q != qModel) {
            std::cerr << "[GrhTB] dut_112 failed(step " << step << ", negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_112 passed: 32-bit LFSR sequence\n";
    return EXIT_SUCCESS;
}
