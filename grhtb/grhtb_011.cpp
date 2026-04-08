#include <array>
#include <cstdlib>
#include <cstdint>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<uint8_t, 7> stimuli{{0b000u, 0b001u, 0b011u, 0b010u, 0b110u, 0b100u, 0b000u}};
    for (uint8_t value : stimuli)
    {
        sim.vec = value & 0x7u;
        sim.eval();

        const bool expectedO0 = ((value >> 0) & 0x1u) != 0;
        const bool expectedO1 = ((value >> 1) & 0x1u) != 0;
        const bool expectedO2 = ((value >> 2) & 0x1u) != 0;
        if ((sim.outv & 0x7u) != (value & 0x7u) ||
            sim.o0 != expectedO0 ||
            sim.o1 != expectedO1 ||
            sim.o2 != expectedO2)
        {
            std::cerr << "[GrhTB] dut_011 failed: vec=" << static_cast<int>(value)
                      << ", expected outv=" << static_cast<int>(value & 0x7u)
                      << ", o0/o1/o2=" << static_cast<int>(expectedO0)
                      << "/" << static_cast<int>(expectedO1)
                      << "/" << static_cast<int>(expectedO2)
                      << ", got outv=" << static_cast<int>(sim.outv & 0x7u)
                      << ", o0/o1/o2=" << static_cast<int>(sim.o0)
                      << "/" << static_cast<int>(sim.o1)
                      << "/" << static_cast<int>(sim.o2) << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_011 passed: outv mirrors vec and o[2:0]==vec[2:0] for all stimuli\n";
    return EXIT_SUCCESS;
}
