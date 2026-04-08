#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus_024 {
    std::uint8_t d;
    std::uint8_t sel;
};

static void tick(GrhSIM_top_module &sim)
{
    sim.clk = true;
    sim.eval();
    sim.clk = false;
    sim.eval();
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<Stimulus_024, 12> stimuli{{
        {0x00u, 0u},
        {0xFFu, 0u},
        {0x00u, 0u},
        {0xFFu, 0u},
        {0x00u, 0u},
        {0xAAu, 1u},
        {0x55u, 2u},
        {0x0Fu, 3u},
        {0xF0u, 1u},
        {0x00u, 2u},
        {0xFFu, 3u},
        {0x00u, 3u},
    }};

    std::uint8_t w1 = 0u;
    std::uint8_t w2 = 0u;

    for (const auto &stim : stimuli) {
        sim.d = stim.d;
        sim.sel = stim.sel & 0x3u;
        tick(sim);

        const std::uint8_t next_w1 = stim.d;
        const std::uint8_t next_w2 = w1;
        const std::uint8_t next_w3 = w2;

        std::uint8_t expected_q = 0u;
        switch (stim.sel & 0x3u) {
        case 0u:
            expected_q = stim.d;
            break;
        case 1u:
            expected_q = next_w1;
            break;
        case 2u:
            expected_q = next_w2;
            break;
        case 3u:
            expected_q = next_w3;
            break;
        }

        if ((sim.q & 0xFFu) != expected_q) {
            std::cerr << "[GrhTB] dut_024 failed: d=0x" << std::hex << static_cast<int>(stim.d)
                      << ", sel=" << std::dec << static_cast<int>(stim.sel)
                      << ", expected q=0x" << std::hex << static_cast<int>(expected_q)
                      << ", got 0x" << static_cast<int>(sim.q & 0xFFu) << std::dec << '\n';
            return EXIT_FAILURE;
        }

        w1 = next_w1;
        w2 = next_w2;
    }

    std::cout << "[GrhTB] dut_024 passed: q taps d/w1/w2/w3 per sel\n";
    return EXIT_SUCCESS;
}
