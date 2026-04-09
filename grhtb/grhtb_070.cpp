#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static std::uint8_t overflow(std::uint8_t a, std::uint8_t b, std::uint8_t sum)
{
    const std::uint8_t signA = static_cast<std::uint8_t>((a >> 7) & 1u);
    const std::uint8_t signB = static_cast<std::uint8_t>((b >> 7) & 1u);
    const std::uint8_t signSum = static_cast<std::uint8_t>((sum >> 7) & 1u);
    return static_cast<std::uint8_t>((signA == signB) && (signSum != signA));
}

static bool check_outputs(GrhSIM_top_module &sim, std::uint8_t a, std::uint8_t b)
{
    sim.a = a;
    sim.b = b;
    sim.eval();

    const std::uint8_t sum = static_cast<std::uint8_t>(a + b);
    return sim.s == sum && sim.overflow == overflow(a, b, sum);
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    if (!check_outputs(sim, 0x00u, 0x00u) || !check_outputs(sim, 0x7Fu, 0x01u) ||
        !check_outputs(sim, 0x80u, 0x80u) || !check_outputs(sim, 0x7Fu, 0xFFu) ||
        !check_outputs(sim, 0x80u, 0x01u)) {
        return EXIT_FAILURE;
    }

    for (int bit = 0; bit < 8; ++bit) {
        for (int rep = 0; rep < 2; ++rep) {
            if (!check_outputs(sim, static_cast<std::uint8_t>(1u << bit), 0) || !check_outputs(sim, 0, 0)) {
                return EXIT_FAILURE;
            }
        }
    }

    for (int bit = 0; bit < 8; ++bit) {
        for (int rep = 0; rep < 2; ++rep) {
            if (!check_outputs(sim, 0, static_cast<std::uint8_t>(1u << bit)) || !check_outputs(sim, 0, 0)) {
                return EXIT_FAILURE;
            }
        }
    }

    std::cout << "[GrhTB] dut_070 passed: 8-bit add with overflow\n";
    return EXIT_SUCCESS;
}
