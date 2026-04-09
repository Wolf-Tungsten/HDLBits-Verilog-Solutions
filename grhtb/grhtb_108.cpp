#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

using Packed100 = std::array<std::uint64_t, 2>;

static void set_u100(Packed100 &dst, std::uint64_t low64, std::uint64_t high36)
{
    dst[0] = low64;
    dst[1] = high36 & 0xFFFFFFFFFull;
}

static std::uint8_t get_bit_u100(const Packed100 &src, int bit)
{
    const std::size_t word = static_cast<std::size_t>(bit) >> 6;
    const std::size_t offset = static_cast<std::size_t>(bit) & 63u;
    return static_cast<std::uint8_t>((src[word] >> offset) & 1u);
}

static void rotate_right1(std::uint64_t &low64, std::uint64_t &high36)
{
    const std::uint8_t bit0 = static_cast<std::uint8_t>(low64 & 1ull);
    const std::uint8_t bit64 = static_cast<std::uint8_t>(high36 & 1ull);
    const std::uint64_t newLow = (low64 >> 1) | (static_cast<std::uint64_t>(bit64) << 63);
    const std::uint64_t newHigh = ((high36 >> 1) & 0xFFFFFFFFFull) | (static_cast<std::uint64_t>(bit0) << 35);
    low64 = newLow;
    high36 = newHigh;
}

static void rotate_left1(std::uint64_t &low64, std::uint64_t &high36)
{
    const std::uint8_t bit99 = static_cast<std::uint8_t>((high36 >> 35) & 1ull);
    const std::uint8_t bit63 = static_cast<std::uint8_t>((low64 >> 63) & 1ull);
    const std::uint64_t newLow = (low64 << 1) | static_cast<std::uint64_t>(bit99);
    const std::uint64_t newHigh = ((high36 << 1) & 0xFFFFFFFFFull) | static_cast<std::uint64_t>(bit63);
    low64 = newLow;
    high36 = newHigh;
}

struct Stimulus108 {
    bool load;
    std::uint8_t ena;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.load = false;
    sim.ena = 0u;
    sim.eval();

    const std::array<Stimulus108, 6> stimuli{{{true, 0u}, {false, 0x1u}, {false, 0x1u}, {false, 0x2u}, {false, 0x2u}, {false, 0u}}};
    std::uint64_t low64 = 1ull;
    std::uint64_t high36 = 1ull << 35;
    set_u100(sim.data, low64, high36);

    for (const auto &stim : stimuli) {
        sim.load = stim.load;
        sim.ena = stim.ena & 0x3u;

        sim.clk = true;
        sim.eval();
        if (!stim.load && sim.ena == 0x1u) {
            rotate_right1(low64, high36);
        } else if (!stim.load && sim.ena == 0x2u) {
            rotate_left1(low64, high36);
        }

        const Packed100 expected = {low64, high36 & 0xFFFFFFFFFull};
        for (int bit = 0; bit < 100; ++bit) {
            if (get_bit_u100(sim.q, bit) != get_bit_u100(expected, bit)) {
                std::cerr << "[GrhTB] dut_108 failed(posedge) at bit " << bit << '\n';
                return EXIT_FAILURE;
            }
        }

        sim.clk = false;
        sim.eval();
        for (int bit = 0; bit < 100; ++bit) {
            if (get_bit_u100(sim.q, bit) != get_bit_u100(expected, bit)) {
                std::cerr << "[GrhTB] dut_108 failed(negedge) at bit " << bit << '\n';
                return EXIT_FAILURE;
            }
        }
    }

    std::cout << "[GrhTB] dut_108 passed: 100-bit bidirectional rotation\n";
    return EXIT_SUCCESS;
}
