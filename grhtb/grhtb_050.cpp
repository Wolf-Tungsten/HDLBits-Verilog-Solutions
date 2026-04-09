#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static bool check_outputs(GrhSIM_top_module &sim, std::uint8_t p1, std::uint8_t p2)
{
    sim.p1a = (p1 >> 0) & 1u;
    sim.p1b = (p1 >> 1) & 1u;
    sim.p1c = (p1 >> 2) & 1u;
    sim.p1d = (p1 >> 3) & 1u;
    sim.p2a = (p2 >> 0) & 1u;
    sim.p2b = (p2 >> 1) & 1u;
    sim.p2c = (p2 >> 2) & 1u;
    sim.p2d = (p2 >> 3) & 1u;
    sim.eval();

    const std::uint8_t p1y = static_cast<std::uint8_t>(~(((p1 >> 0) & 1u) & ((p1 >> 1) & 1u) &
                                                         ((p1 >> 2) & 1u) & ((p1 >> 3) & 1u)) &
                                                       1u);
    const std::uint8_t p2y = static_cast<std::uint8_t>(~(((p2 >> 0) & 1u) & ((p2 >> 1) & 1u) &
                                                         ((p2 >> 2) & 1u) & ((p2 >> 3) & 1u)) &
                                                       1u);
    return sim.p1y == p1y && sim.p2y == p2y;
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    if (!check_outputs(sim, 0x0u, 0x0u) || !check_outputs(sim, 0xFu, 0xFu) ||
        !check_outputs(sim, 0x7u, 0xEu) || !check_outputs(sim, 0x1u, 0x8u)) {
        return EXIT_FAILURE;
    }

    for (int bit = 0; bit < 4; ++bit) {
        for (int rep = 0; rep < 2; ++rep) {
            if (!check_outputs(sim, 0x0u, 0x0u) || !check_outputs(sim, static_cast<std::uint8_t>(1u << bit), 0x0u) ||
                !check_outputs(sim, 0x0u, 0x0u)) {
                return EXIT_FAILURE;
            }
        }
    }

    for (int bit = 0; bit < 4; ++bit) {
        for (int rep = 0; rep < 2; ++rep) {
            if (!check_outputs(sim, 0x0u, 0x0u) || !check_outputs(sim, 0x0u, static_cast<std::uint8_t>(1u << bit)) ||
                !check_outputs(sim, 0x0u, 0x0u)) {
                return EXIT_FAILURE;
            }
        }
    }

    std::cout << "[GrhTB] dut_050 passed: two 4-input NANDs\n";
    return EXIT_SUCCESS;
}
