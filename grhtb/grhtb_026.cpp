#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "grhsim_top_module.hpp"

struct Stimulus_026 {
    std::uint32_t a;
    std::uint32_t b;
};

static inline std::uint32_t add32(std::uint32_t a, std::uint32_t b)
{
    return static_cast<std::uint32_t>(static_cast<std::uint64_t>(a) + static_cast<std::uint64_t>(b));
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    std::vector<Stimulus_026> stimuli;
    stimuli.reserve(8 * 32 + 16);
    stimuli.push_back({0x00000000u, 0x00000000u});

    for (int i = 0; i < 32; ++i) {
        const std::uint32_t bit = static_cast<std::uint32_t>(1u) << i;
        stimuli.push_back({bit, 0x00000000u});
        stimuli.push_back({0x00000000u, 0x00000000u});
        stimuli.push_back({0x00000000u, bit});
        stimuli.push_back({0x00000000u, 0x00000000u});
    }

    for (int i = 0; i < 16; ++i) {
        const std::uint32_t lower_ones = (i == 0) ? 0u : ((static_cast<std::uint32_t>(1u) << i) - 1u);
        stimuli.push_back({lower_ones, 0x00000001u});
        stimuli.push_back({0x00000000u, 0x00000000u});
    }

    for (int k = 1; k <= 16; ++k) {
        const std::uint32_t upper_ones = ((static_cast<std::uint32_t>(1u) << k) - 1u) << 16;
        const std::uint32_t a_val = upper_ones | 0x0000FFFFu;
        stimuli.push_back({a_val, 0x00000001u});
        stimuli.push_back({0x00000000u, 0x00000000u});
    }

    stimuli.push_back({0x12345678u, 0x11111111u});
    stimuli.push_back({0x0000FFFFu, 0x00000001u});
    stimuli.push_back({0xFFFF0000u, 0x0000FFFFu});
    stimuli.push_back({0xFFFFFFFFu, 0x00000001u});
    stimuli.push_back({0x00000000u, 0x00000000u});

    for (const auto &stim : stimuli) {
        sim.a = stim.a;
        sim.b = stim.b;
        sim.eval();

        const std::uint32_t expected = add32(stim.a, stim.b);
        if (sim.sum != expected) {
            std::cerr << "[GrhTB] dut_026 failed: a=0x" << std::hex << stim.a
                      << ", b=0x" << stim.b
                      << ", expected sum=0x" << expected
                      << ", got 0x" << sim.sum << std::dec << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_026 passed: ripple-carry adder matches a+b\n";
    return EXIT_SUCCESS;
}
