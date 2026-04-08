#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "grhsim_top_module.hpp"

struct Stimulus_025 {
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

    std::vector<Stimulus_025> stimuli;
    stimuli.reserve(4 * 32 + 8);
    stimuli.push_back({0x00000000u, 0x00000000u});

    for (int i = 0; i < 32; ++i) {
        const std::uint32_t bit = static_cast<std::uint32_t>(1u) << i;
        stimuli.push_back({bit, 0x00000000u});
        stimuli.push_back({0x00000000u, 0x00000000u});
        stimuli.push_back({0x00000000u, bit});
        stimuli.push_back({0x00000000u, 0x00000000u});
    }

    stimuli.push_back({0x0000FFFFu, 0x00000001u});
    stimuli.push_back({0x00000000u, 0x00000000u});
    stimuli.push_back({0xFFFFFFFFu, 0x00000001u});
    stimuli.push_back({0x00000000u, 0x00000000u});
    stimuli.push_back({0x70000000u, 0x0F000000u});
    stimuli.push_back({0x00000000u, 0x00000000u});

    for (const auto &stim : stimuli) {
        sim.a = stim.a;
        sim.b = stim.b;
        sim.eval();

        const std::uint32_t expected = add32(stim.a, stim.b);
        if (sim.sum != expected) {
            std::cerr << "[GrhTB] dut_025 failed: a=0x" << std::hex << stim.a
                      << ", b=0x" << stim.b
                      << ", expected sum=0x" << expected
                      << ", got 0x" << sim.sum << std::dec << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_025 passed: sum == a+b across the 16-bit blocks\n";
    return EXIT_SUCCESS;
}
