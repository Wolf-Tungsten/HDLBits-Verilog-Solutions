#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "grhsim_top_module.hpp"

struct Stimulus_028 {
    std::uint32_t a;
    std::uint32_t b;
    bool sub;
};

static inline std::uint32_t addsub(std::uint32_t a, std::uint32_t b, bool sub)
{
    const std::uint32_t b_xor = sub ? ~b : b;
    return static_cast<std::uint32_t>(static_cast<std::uint64_t>(a) +
                                      static_cast<std::uint64_t>(b_xor) +
                                      static_cast<std::uint64_t>(sub ? 1u : 0u));
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    std::vector<Stimulus_028> stimuli;
    stimuli.reserve(12 * 32);
    stimuli.push_back({0x00000000u, 0x00000000u, false});
    stimuli.push_back({0x00000000u, 0x00000000u, true});

    for (int i = 0; i < 32; ++i) {
        const std::uint32_t bit = static_cast<std::uint32_t>(1u) << i;
        stimuli.push_back({bit, 0x00000000u, false});
        stimuli.push_back({0x00000000u, bit, false});
        stimuli.push_back({bit, 0x00000000u, true});
        stimuli.push_back({0x00000000u, bit, true});
        stimuli.push_back({0x00000000u, 0x00000000u, false});
    }

    stimuli.push_back({0x0000FFFFu, 0x00000001u, false});
    stimuli.push_back({0x7FFFFFFFu, 0x00000001u, false});
    stimuli.push_back({0xFFFFFFFFu, 0x00000001u, false});
    stimuli.push_back({0x00010000u, 0x00000001u, true});
    stimuli.push_back({0x00000000u, 0x00000001u, true});
    stimuli.push_back({0x80000000u, 0x00000001u, true});
    stimuli.push_back({0x12345678u, 0x11111111u, false});
    stimuli.push_back({0x89ABCDEFu, 0x01020304u, true});
    stimuli.push_back({0x00000000u, 0x00000000u, false});

    for (const auto &stim : stimuli) {
        sim.a = stim.a;
        sim.b = stim.b;
        sim.sub = stim.sub;
        sim.eval();

        const std::uint32_t expected = addsub(stim.a, stim.b, stim.sub);
        if (sim.sum != expected) {
            std::cerr << "[GrhTB] dut_028 failed: a=0x" << std::hex << stim.a
                      << ", b=0x" << stim.b
                      << ", sub=" << std::dec << static_cast<int>(stim.sub)
                      << ", expected sum=0x" << std::hex << expected
                      << ", got 0x" << sim.sum << std::dec << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_028 passed: sum matches add/sub behavior\n";
    return EXIT_SUCCESS;
}
