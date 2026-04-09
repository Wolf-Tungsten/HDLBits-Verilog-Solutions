#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

using Packed256 = std::array<std::uint64_t, 4>;

static std::uint8_t get_bit(const Packed256 &value, int bit)
{
    const std::size_t word = static_cast<std::size_t>(bit) >> 6;
    const std::size_t offset = static_cast<std::size_t>(bit) & 63u;
    return static_cast<std::uint8_t>((value[word] >> offset) & 1u);
}

static void set_bit(Packed256 &value, int bit, bool on)
{
    const std::size_t word = static_cast<std::size_t>(bit) >> 6;
    const std::size_t offset = static_cast<std::size_t>(bit) & 63u;
    const std::uint64_t mask = UINT64_C(1) << offset;
    value[word] = on ? (value[word] | mask) : (value[word] & ~mask);
}

static void set_pattern(Packed256 &value)
{
    for (std::size_t i = 0; i < value.size(); ++i) {
        value[i] = UINT64_C(0x0F0F0F0F0F0F0F0F) ^ (UINT64_C(0x0101010101010101) * i);
    }
}

static Packed256 step_life(const Packed256 &cur)
{
    Packed256 next{};
    const auto idx = [](int x, int y) { return x + y * 16; };
    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            const int xm1 = x == 0 ? 15 : x - 1;
            const int xp1 = x == 15 ? 0 : x + 1;
            const int ym1 = y == 0 ? 15 : y - 1;
            const int yp1 = y == 15 ? 0 : y + 1;
            std::uint8_t pop = 0u;
            pop = static_cast<std::uint8_t>(pop + get_bit(cur, idx(xm1, ym1)));
            pop = static_cast<std::uint8_t>(pop + get_bit(cur, idx(xm1, y)));
            pop = static_cast<std::uint8_t>(pop + get_bit(cur, idx(xm1, yp1)));
            pop = static_cast<std::uint8_t>(pop + get_bit(cur, idx(x, ym1)));
            pop = static_cast<std::uint8_t>(pop + get_bit(cur, idx(x, yp1)));
            pop = static_cast<std::uint8_t>(pop + get_bit(cur, idx(xp1, ym1)));
            pop = static_cast<std::uint8_t>(pop + get_bit(cur, idx(xp1, y)));
            pop = static_cast<std::uint8_t>(pop + get_bit(cur, idx(xp1, yp1)));
            const std::uint8_t current = get_bit(cur, idx(x, y));
            const bool aliveNext = pop == 3u || (pop == 2u && current != 0u);
            set_bit(next, idx(x, y), aliveNext);
        }
    }
    return next;
}

static bool same_bits(const Packed256 &lhs, const Packed256 &rhs)
{
    for (std::size_t i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

static std::uint32_t lfsr_next(std::uint32_t state)
{
    const std::uint32_t bit = ((state >> 0) ^ (state >> 1) ^ (state >> 21) ^ (state >> 31)) & 1u;
    return (state >> 1) | (bit << 31);
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.load = true;
    Packed256 qModel{};
    set_pattern(qModel);
    sim.data = qModel;
    sim.eval();

    sim.clk = true;
    sim.eval();
    if (!same_bits(sim.q, qModel)) {
        std::cerr << "[GrhTB] dut_118 failed(load posedge)\n";
        return EXIT_FAILURE;
    }
    sim.clk = false;
    sim.eval();
    if (!same_bits(sim.q, qModel)) {
        std::cerr << "[GrhTB] dut_118 failed(load negedge)\n";
        return EXIT_FAILURE;
    }

    sim.load = false;
    for (int step = 0; step < 8; ++step) {
        sim.clk = true;
        sim.eval();
        qModel = step_life(qModel);
        if (!same_bits(sim.q, qModel)) {
            std::cerr << "[GrhTB] dut_118 failed(life step " << step << ", posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (!same_bits(sim.q, qModel)) {
            std::cerr << "[GrhTB] dut_118 failed(life step " << step << ", negedge)\n";
            return EXIT_FAILURE;
        }
    }

    for (int bit = 0; bit < 256; ++bit) {
        Packed256 oneHot{};
        set_bit(oneHot, bit, true);
        sim.load = true;
        sim.data = oneHot;
        sim.clk = true;
        sim.eval();
        if (!same_bits(sim.q, oneHot)) {
            std::cerr << "[GrhTB] dut_118 failed(toggle set " << bit << ")\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (!same_bits(sim.q, oneHot)) {
            std::cerr << "[GrhTB] dut_118 failed(toggle set hold " << bit << ")\n";
            return EXIT_FAILURE;
        }

        Packed256 zeros{};
        sim.data = zeros;
        sim.clk = true;
        sim.eval();
        if (!same_bits(sim.q, zeros)) {
            std::cerr << "[GrhTB] dut_118 failed(toggle clear " << bit << ")\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (!same_bits(sim.q, zeros)) {
            std::cerr << "[GrhTB] dut_118 failed(toggle clear hold " << bit << ")\n";
            return EXIT_FAILURE;
        }
    }

    std::uint32_t lfsr = 0x1u;
    for (int pattern = 0; pattern < 64; ++pattern) {
        Packed256 cur{};
        for (int bit = 0; bit < 256; ++bit) {
            set_bit(cur, bit, (lfsr & 1u) != 0u);
            lfsr = lfsr_next(lfsr);
        }
        const Packed256 next = step_life(cur);

        sim.load = true;
        sim.data = cur;
        sim.clk = true;
        sim.eval();
        if (!same_bits(sim.q, cur)) {
            std::cerr << "[GrhTB] dut_118 failed(random load " << pattern << ")\n";
            return EXIT_FAILURE;
        }
        sim.clk = false;
        sim.eval();
        if (!same_bits(sim.q, cur)) {
            std::cerr << "[GrhTB] dut_118 failed(random load hold " << pattern << ")\n";
            return EXIT_FAILURE;
        }

        sim.load = false;
        sim.clk = true;
        sim.eval();
        if (!same_bits(sim.q, next)) {
            std::cerr << "[GrhTB] dut_118 failed(random step " << pattern << ")\n";
            return EXIT_FAILURE;
        }
        sim.clk = false;
        sim.eval();
        if (!same_bits(sim.q, next)) {
            std::cerr << "[GrhTB] dut_118 failed(random step hold " << pattern << ")\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_118 passed: 16x16 toroidal Game of Life\n";
    return EXIT_SUCCESS;
}
