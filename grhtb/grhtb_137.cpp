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

static void send_frame(GrhSIM_top_module &sim, std::uint8_t data, std::uint8_t parityBit, bool goodStop)
{
    sim.in = true;
    tick(sim);
    sim.in = false;
    tick(sim);
    for (int i = 0; i < 8; ++i) {
        sim.in = ((data >> i) & 1u) != 0u;
        tick(sim);
    }
    sim.in = (parityBit & 1u) != 0u;
    tick(sim);
    sim.in = goodStop;
    tick(sim);
}

static std::uint8_t parity_odd(std::uint8_t value)
{
    std::uint8_t parity = 0u;
    for (int i = 0; i < 8; ++i) {
        parity ^= static_cast<std::uint8_t>((value >> i) & 1u);
    }
    return static_cast<std::uint8_t>(parity & 1u);
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

    const std::uint8_t d1 = 0x5Au;
    const std::uint8_t p1 = static_cast<std::uint8_t>(parity_odd(d1) ^ 1u);
    send_frame(sim, d1, p1, true);
    if (!sim.done || sim.out_byte != d1) {
        std::cerr << "[GrhTB] dut_137 failed(good frame)\n";
        return EXIT_FAILURE;
    }
    sim.in = true;
    tick(sim);

    const std::uint8_t d2 = 0xA5u;
    const std::uint8_t p2 = parity_odd(d2);
    send_frame(sim, d2, p2, true);
    if (sim.done) {
        std::cerr << "[GrhTB] dut_137 failed(bad parity)\n";
        return EXIT_FAILURE;
    }

    for (const std::uint8_t value : {0x00u, 0xFFu, 0xAAu, 0x55u, 0x0Fu, 0xF0u}) {
        const std::uint8_t parity = static_cast<std::uint8_t>(parity_odd(value) ^ 1u);
        send_frame(sim, value, parity, true);
        if (!sim.done || sim.out_byte != value) {
            std::cerr << "[GrhTB] dut_137 failed(extended frame)\n";
            return EXIT_FAILURE;
        }
        sim.in = true;
        tick(sim);
    }

    sim.reset = true;
    sim.eval();
    tick(sim);
    sim.reset = false;
    const std::uint8_t dw = 0x3Cu;
    const std::uint8_t pw = static_cast<std::uint8_t>(parity_odd(dw) ^ 1u);
    send_frame(sim, dw, pw, false);
    if (sim.done) {
        std::cerr << "[GrhTB] dut_137 failed(wait path)\n";
        return EXIT_FAILURE;
    }
    sim.in = false;
    tick(sim);
    sim.in = true;
    tick(sim);

    sim.reset = true;
    sim.eval();
    tick(sim);
    sim.reset = false;

    std::cout << "[GrhTB] dut_137 passed: serial receiver with parity check\n";
    return EXIT_SUCCESS;
}
