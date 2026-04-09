#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static void tick(GrhSIM_top_module &sim)
{
    sim.clk = true;
    sim.eval();
    sim.clk = false;
    sim.eval();
}

static void send_frame(GrhSIM_top_module &sim, std::uint8_t data, bool goodStop)
{
    sim.in = true;
    tick(sim);
    sim.in = false;
    tick(sim);
    for (int i = 0; i < 8; ++i) {
        sim.in = ((data >> i) & 1u) != 0u;
        tick(sim);
    }
    sim.in = goodStop;
    tick(sim);
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.reset = true;
    sim.in = true;
    sim.eval();
    tick(sim);
    sim.reset = false;

    send_frame(sim, 0xA5u, true);
    if (!sim.done) {
        std::cerr << "[GrhTB] dut_135 failed(good frame)\n";
        return EXIT_FAILURE;
    }
    sim.in = true;
    tick(sim);

    send_frame(sim, 0x5Au, false);
    if (sim.done) {
        std::cerr << "[GrhTB] dut_135 failed(bad stop)\n";
        return EXIT_FAILURE;
    }

    std::cout << "[GrhTB] dut_135 passed: serial receiver done flag behavior\n";
    return EXIT_SUCCESS;
}
