#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static bool check_outputs(GrhSIM_top_module &sim, std::uint8_t ring, std::uint8_t vibrateMode)
{
    sim.ring = ring;
    sim.vibrate_mode = vibrateMode;
    sim.eval();

    const std::uint8_t ringer = static_cast<std::uint8_t>(ring & static_cast<std::uint8_t>(~vibrateMode & 1u));
    const std::uint8_t motor = static_cast<std::uint8_t>(ring & vibrateMode);
    return sim.ringer == ringer && sim.motor == motor;
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    if (!check_outputs(sim, 0, 0) || !check_outputs(sim, 0, 1) || !check_outputs(sim, 1, 0) ||
        !check_outputs(sim, 1, 1)) {
        return EXIT_FAILURE;
    }

    for (int rep = 0; rep < 2; ++rep) {
        if (!check_outputs(sim, 0, 0) || !check_outputs(sim, 1, 0) || !check_outputs(sim, 0, 0) ||
            !check_outputs(sim, 0, 1) || !check_outputs(sim, 0, 0) || !check_outputs(sim, 1, 1) ||
            !check_outputs(sim, 0, 1) || !check_outputs(sim, 1, 1)) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_056 passed: ringer/motor selection\n";
    return EXIT_SUCCESS;
}
