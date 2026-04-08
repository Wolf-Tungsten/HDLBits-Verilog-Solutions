#include <array>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus_032 {
    bool cpu_overheated;
    bool arrived;
    bool gas_empty;
};

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<Stimulus_032, 8> stimuli{{
        {false, false, false},
        {true, false, false},
        {true, false, true},
        {false, false, true},
        {false, true, false},
        {false, true, true},
        {true, true, true},
        {false, false, false},
    }};

    for (const auto &stim : stimuli) {
        sim.cpu_overheated = stim.cpu_overheated;
        sim.arrived = stim.arrived;
        sim.gas_tank_empty = stim.gas_empty;
        sim.eval();

        const bool expected_shut = stim.cpu_overheated;
        const bool expected_drive = (!stim.arrived) && (!stim.gas_empty);
        if (sim.shut_off_computer != expected_shut || sim.keep_driving != expected_drive) {
            std::cerr << "[GrhTB] dut_032 failed: overheated=" << static_cast<int>(stim.cpu_overheated)
                      << ", arrived=" << static_cast<int>(stim.arrived)
                      << ", gas_empty=" << static_cast<int>(stim.gas_empty)
                      << ", expected shut/drive=" << static_cast<int>(expected_shut)
                      << "/" << static_cast<int>(expected_drive)
                      << ", got " << static_cast<int>(sim.shut_off_computer)
                      << "/" << static_cast<int>(sim.keep_driving) << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_032 passed: control outputs match spec\n";
    return EXIT_SUCCESS;
}
