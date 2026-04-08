#include <array>
#include <cstdlib>
#include <cstdint>
#include <iostream>

#include "grhsim_top_module.hpp"

static uint32_t byteswap32(uint32_t value)
{
    return ((value & 0x000000FFu) << 24) |
           ((value & 0x0000FF00u) << 8) |
           ((value & 0x00FF0000u) >> 8) |
           ((value & 0xFF000000u) >> 24);
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<uint32_t, 4> stimuli{{0x00000000u, 0xFFFFFFFFu, 0x12345678u, 0x00000000u}};
    for (uint32_t value : stimuli)
    {
        sim.in = value;
        sim.eval();

        const uint32_t expected = byteswap32(value);
        if (sim.out != expected)
        {
            std::cerr << "[GrhTB] dut_013 failed: in=0x" << std::hex << value
                      << ", expected out=0x" << expected
                      << ", got 0x" << sim.out << std::dec << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_013 passed: out bytes are reversed from in\n";
    return EXIT_SUCCESS;
}
