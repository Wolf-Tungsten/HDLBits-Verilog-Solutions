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

static bool send_good_frame_check(GrhSIM_top_module &sim, std::uint8_t data)
{
    send_frame(sim, data, true);
    if (!sim.done || sim.out_byte != data) {
        return false;
    }
    sim.in = true;
    tick(sim);
    return true;
}

static bool send_bad_frame_check(GrhSIM_top_module &sim, std::uint8_t data)
{
    send_frame(sim, data, false);
    if (sim.done) {
        return false;
    }
    sim.in = true;
    tick(sim);
    return true;
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.in = true;
    sim.reset = false;
    sim.eval();

    auto apply_reset = [&]() {
        sim.reset = true;
        sim.eval();
        tick(sim);
        sim.reset = false;
    };

    apply_reset();
    if (!send_good_frame_check(sim, 0x3Cu) || !send_bad_frame_check(sim, 0xA5u)) {
        std::cerr << "[GrhTB] dut_136 failed(basic frame)\n";
        return EXIT_FAILURE;
    }

    for (const std::uint8_t value : {0x00u, 0xFFu, 0xAAu, 0x55u}) {
        if (!send_good_frame_check(sim, value)) {
            std::cerr << "[GrhTB] dut_136 failed(pattern)\n";
            return EXIT_FAILURE;
        }
    }
    for (int bit = 0; bit < 8; ++bit) {
        if (!send_good_frame_check(sim, static_cast<std::uint8_t>(1u << bit))) {
            std::cerr << "[GrhTB] dut_136 failed(single bit)\n";
            return EXIT_FAILURE;
        }
    }

    if (!send_bad_frame_check(sim, 0x00u) || !send_bad_frame_check(sim, 0xFFu)) {
        std::cerr << "[GrhTB] dut_136 failed(wait path)\n";
        return EXIT_FAILURE;
    }

    apply_reset();
    if (!send_good_frame_check(sim, 0x0Fu) || !send_good_frame_check(sim, 0xF0u)) {
        std::cerr << "[GrhTB] dut_136 failed(post-reset)\n";
        return EXIT_FAILURE;
    }

    std::cout << "[GrhTB] dut_136 passed: serial receiver with data latch and full coverage patterns\n";
    return EXIT_SUCCESS;
}
