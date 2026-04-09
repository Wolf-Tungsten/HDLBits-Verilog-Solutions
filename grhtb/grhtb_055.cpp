#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static std::uint8_t ref(std::uint8_t x, std::uint8_t y)
{
    const std::uint8_t t1 = static_cast<std::uint8_t>(((x ^ y) & 1u) & (x & 1u));
    const std::uint8_t t2 = static_cast<std::uint8_t>(~(x ^ y) & 1u);
    return static_cast<std::uint8_t>((t1 | t2) ^ (t1 & t2));
}

static bool check_outputs(GrhSIM_top_module &sim, std::uint8_t x, std::uint8_t y)
{
    sim.x = x;
    sim.y = y;
    sim.eval();
    return sim.z == ref(x, y);
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    for (int value = 0; value < 4; ++value) {
        if (!check_outputs(sim,
                           static_cast<std::uint8_t>((value >> 0) & 1u),
                           static_cast<std::uint8_t>((value >> 1) & 1u))) {
            return EXIT_FAILURE;
        }
    }

    for (int rep = 0; rep < 2; ++rep) {
        if (!check_outputs(sim, 0, 0) || !check_outputs(sim, 1, 0) || !check_outputs(sim, 0, 0) ||
            !check_outputs(sim, 0, 1) || !check_outputs(sim, 0, 0)) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_055 passed: expression verified\n";
    return EXIT_SUCCESS;
}
