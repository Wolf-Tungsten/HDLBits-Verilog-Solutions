#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static std::uint8_t ref(std::uint8_t x)
{
    const std::uint8_t x1 = static_cast<std::uint8_t>((x >> 0) & 1u);
    const std::uint8_t x2 = static_cast<std::uint8_t>((x >> 1) & 1u);
    const std::uint8_t x3 = static_cast<std::uint8_t>((x >> 2) & 1u);
    const std::uint8_t x4 = static_cast<std::uint8_t>((x >> 3) & 1u);
    return static_cast<std::uint8_t>((x3 & static_cast<std::uint8_t>(~x1 & 1u)) | (x2 & x3 & x4) |
                                     (static_cast<std::uint8_t>(~x4 & 1u) & static_cast<std::uint8_t>(~x2 & 1u)));
}

static bool check_outputs(GrhSIM_top_module &sim, std::uint8_t x)
{
    sim.x = x & 0xFu;
    sim.eval();
    return sim.f == ref(x);
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    for (int value = 0; value < 16; ++value) {
        if (!check_outputs(sim, static_cast<std::uint8_t>(value))) {
            return EXIT_FAILURE;
        }
    }

    for (int rep = 0; rep < 2; ++rep) {
        if (!check_outputs(sim, 0)) {
            return EXIT_FAILURE;
        }
        for (int bit = 0; bit < 4; ++bit) {
            if (!check_outputs(sim, static_cast<std::uint8_t>(1u << bit)) || !check_outputs(sim, 0)) {
                return EXIT_FAILURE;
            }
        }
    }

    std::cout << "[GrhTB] dut_079 passed: 4-input logic on x[4:1]\n";
    return EXIT_SUCCESS;
}
