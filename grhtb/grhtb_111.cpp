#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus111 {
    bool load;
    std::uint8_t value;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.KEY = 0u;
    sim.SW = 0u;
    sim.eval();

    std::uint8_t qModel = 0u;
    const std::array<Stimulus111, 9> pattern{{{true, 0x5u},
                                              {false, 0x0u},
                                              {false, 0x0u},
                                              {true, 0x3u},
                                              {false, 0x0u},
                                              {false, 0x0u},
                                              {true, 0x7u},
                                              {false, 0x0u},
                                              {false, 0x0u}}};

    for (const auto &stim : pattern) {
        sim.SW = static_cast<std::uint8_t>(stim.value & 0x7u);
        sim.KEY = static_cast<std::uint8_t>((stim.load ? 0x2u : 0u) | 0x1u);
        sim.eval();

        if (stim.load) {
            qModel = static_cast<std::uint8_t>(stim.value & 0x7u);
        } else {
            const std::uint8_t q2 = static_cast<std::uint8_t>((qModel >> 2) & 1u);
            const std::uint8_t q1 = static_cast<std::uint8_t>((qModel >> 1) & 1u);
            const std::uint8_t q0 = static_cast<std::uint8_t>(qModel & 1u);
            qModel = static_cast<std::uint8_t>(((q2 ^ q1) << 2) | (q0 << 1) | q2);
        }

        if ((sim.LEDR & 0x7u) != qModel) {
            std::cerr << "[GrhTB] dut_111 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.KEY &= static_cast<std::uint8_t>(~0x1u);
        sim.eval();
        if ((sim.LEDR & 0x7u) != qModel) {
            std::cerr << "[GrhTB] dut_111 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_111 passed: 3-bit LFSR with load\n";
    return EXIT_SUCCESS;
}
