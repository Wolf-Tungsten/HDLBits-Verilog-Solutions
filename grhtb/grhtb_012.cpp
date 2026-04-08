#include <array>
#include <cstdlib>
#include <cstdint>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<uint16_t, 3> stimuli{{0x0000u, 0xFFFFu, 0x0000u}};
    for (uint16_t value : stimuli)
    {
        sim.in = value;
        sim.eval();

        const uint8_t expectedHi = static_cast<uint8_t>((value >> 8) & 0xFFu);
        const uint8_t expectedLo = static_cast<uint8_t>(value & 0xFFu);
        if (sim.out_hi != expectedHi || sim.out_lo != expectedLo)
        {
            std::cerr << "[GrhTB] dut_012 failed: in=0x" << std::hex << value
                      << std::dec << ", expected hi/lo=" << static_cast<int>(expectedHi)
                      << "/" << static_cast<int>(expectedLo)
                      << ", got " << static_cast<int>(sim.out_hi)
                      << "/" << static_cast<int>(sim.out_lo) << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_012 passed: out_hi=in[15:8], out_lo=in[7:0]\n";
    return EXIT_SUCCESS;
}
