#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus114 {
    bool load;
    bool enable;
    bool serialIn;
    std::uint8_t parallelIn;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.KEY = 0u;
    sim.SW = 0u;
    sim.eval();

    std::uint8_t qModel = 0u;
    const std::array<Stimulus114, 7> pattern{{{true, false, false, 0xAu},
                                              {false, true, true, 0x0u},
                                              {false, true, false, 0x0u},
                                              {true, false, false, 0x5u},
                                              {false, true, true, 0x0u},
                                              {false, true, true, 0x0u},
                                              {false, false, false, 0x0u}}};

    for (const auto &stim : pattern) {
        sim.SW = static_cast<std::uint8_t>(stim.parallelIn & 0xFu);
        sim.KEY = static_cast<std::uint8_t>((stim.serialIn ? 0x8u : 0u) |
                                            (stim.load ? 0x4u : 0u) |
                                            (stim.enable ? 0x2u : 0u) | 0x1u);
        sim.eval();

        const std::uint8_t prev = qModel;
        std::uint8_t next = 0u;
        for (int bit = 3; bit >= 0; --bit) {
            const std::uint8_t hold = static_cast<std::uint8_t>((prev >> bit) & 1u);
            const std::uint8_t shiftIn = (bit == 3)
                                             ? (stim.serialIn ? 1u : 0u)
                                             : static_cast<std::uint8_t>((prev >> (bit + 1)) & 1u);
            const std::uint8_t afterEnable = stim.enable ? shiftIn : hold;
            const std::uint8_t afterLoad =
                stim.load ? static_cast<std::uint8_t>((stim.parallelIn >> bit) & 1u) : afterEnable;
            next |= static_cast<std::uint8_t>(afterLoad << bit);
        }
        qModel = next;

        if ((sim.LEDR & 0xFu) != qModel) {
            std::cerr << "[GrhTB] dut_114 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.KEY &= static_cast<std::uint8_t>(~0x1u);
        sim.eval();
        if ((sim.LEDR & 0xFu) != qModel) {
            std::cerr << "[GrhTB] dut_114 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_114 passed: chained MUXDFF behavior\n";
    return EXIT_SUCCESS;
}
