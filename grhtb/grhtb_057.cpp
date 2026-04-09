#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static bool check_outputs(GrhSIM_top_module &sim,
                          std::uint8_t tooCold,
                          std::uint8_t tooHot,
                          std::uint8_t mode,
                          std::uint8_t fanOn)
{
    sim.too_cold = tooCold;
    sim.too_hot = tooHot;
    sim.mode = mode;
    sim.fan_on = fanOn;
    sim.eval();

    const std::uint8_t heater = static_cast<std::uint8_t>((mode & tooCold) & 1u);
    const std::uint8_t aircon = static_cast<std::uint8_t>((static_cast<std::uint8_t>(~mode & 1u) & tooHot) & 1u);
    const std::uint8_t fan = static_cast<std::uint8_t>((fanOn | heater | aircon) & 1u);
    return sim.heater == heater && sim.aircon == aircon && sim.fan == fan;
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    for (int value = 0; value < 16; ++value) {
        if (!check_outputs(sim,
                           static_cast<std::uint8_t>((value >> 0) & 1u),
                           static_cast<std::uint8_t>((value >> 1) & 1u),
                           static_cast<std::uint8_t>((value >> 2) & 1u),
                           static_cast<std::uint8_t>((value >> 3) & 1u))) {
            return EXIT_FAILURE;
        }
    }

    for (int rep = 0; rep < 2; ++rep) {
        if (!check_outputs(sim, 0, 0, 0, 0) || !check_outputs(sim, 1, 0, 0, 0) || !check_outputs(sim, 0, 0, 0, 0) ||
            !check_outputs(sim, 0, 1, 0, 0) || !check_outputs(sim, 0, 0, 0, 0) || !check_outputs(sim, 0, 0, 1, 0) ||
            !check_outputs(sim, 0, 0, 0, 0) || !check_outputs(sim, 0, 0, 0, 1) || !check_outputs(sim, 0, 0, 0, 0)) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_057 passed: thermostat control\n";
    return EXIT_SUCCESS;
}
