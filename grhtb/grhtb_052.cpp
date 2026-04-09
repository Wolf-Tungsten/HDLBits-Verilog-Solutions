#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static std::uint8_t ref(std::uint8_t a, std::uint8_t b)
{
    return static_cast<std::uint8_t>((a & 3u) == (b & 3u));
}

static bool check_outputs(GrhSIM_top_module &sim, std::uint8_t a, std::uint8_t b)
{
    sim.A = a & 3u;
    sim.B = b & 3u;
    sim.eval();
    return sim.z == ref(a, b);
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    for (int a = 0; a < 4; ++a) {
        for (int b = 0; b < 4; ++b) {
            if (!check_outputs(sim, static_cast<std::uint8_t>(a), static_cast<std::uint8_t>(b))) {
                return EXIT_FAILURE;
            }
        }
    }

    for (int rep = 0; rep < 2; ++rep) {
        if (!check_outputs(sim, 0, 0) || !check_outputs(sim, 1, 0) || !check_outputs(sim, 0, 0) ||
            !check_outputs(sim, 2, 0) || !check_outputs(sim, 0, 0) || !check_outputs(sim, 0, 1) ||
            !check_outputs(sim, 0, 0) || !check_outputs(sim, 0, 2) || !check_outputs(sim, 0, 0)) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_052 passed: z = (A==B)\n";
    return EXIT_SUCCESS;
}
