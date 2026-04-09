#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

using Packed512 = std::array<std::uint64_t, 8>;

static std::uint8_t get_bit(const Packed512 &value, int bit)
{
    const std::size_t word = static_cast<std::size_t>(bit) >> 6;
    const std::size_t offset = static_cast<std::size_t>(bit) & 63u;
    return static_cast<std::uint8_t>((value[word] >> offset) & 1u);
}

static void set_pattern(Packed512 &value)
{
    for (std::size_t i = 0; i < value.size(); ++i) {
        value[i] = UINT64_C(0xA5A5A5A5A5A5A5A5) ^ (UINT64_C(0x1111111111111111) * i);
    }
}

static Packed512 step_rule(const Packed512 &cur)
{
    Packed512 next{};
    for (int bit = 0; bit < 512; ++bit) {
        const std::uint8_t left = bit == 0 ? 0u : get_bit(cur, bit - 1);
        const std::uint8_t right = bit == 511 ? 0u : get_bit(cur, bit + 1);
        if ((left ^ right) != 0u) {
            const std::size_t word = static_cast<std::size_t>(bit) >> 6;
            const std::size_t offset = static_cast<std::size_t>(bit) & 63u;
            next[word] |= UINT64_C(1) << offset;
        }
    }
    return next;
}

static bool same_bits(const Packed512 &lhs, const Packed512 &rhs)
{
    for (std::size_t i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.load = true;
    Packed512 qModel{};
    set_pattern(qModel);
    sim.data = qModel;
    sim.eval();

    sim.clk = true;
    sim.eval();
    if (!same_bits(sim.q, qModel)) {
        std::cerr << "[GrhTB] dut_116 failed(load posedge)\n";
        return EXIT_FAILURE;
    }

    sim.clk = false;
    sim.eval();
    if (!same_bits(sim.q, qModel)) {
        std::cerr << "[GrhTB] dut_116 failed(load negedge)\n";
        return EXIT_FAILURE;
    }

    sim.load = false;
    for (int step = 0; step < 16; ++step) {
        sim.clk = true;
        sim.eval();
        qModel = step_rule(qModel);
        if (!same_bits(sim.q, qModel)) {
            std::cerr << "[GrhTB] dut_116 failed(step " << step << ", posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (!same_bits(sim.q, qModel)) {
            std::cerr << "[GrhTB] dut_116 failed(step " << step << ", negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_116 passed: 512-bit XOR cellular update\n";
    return EXIT_SUCCESS;
}
