#include <array>
#include <cstdlib>
#include <cstdint>
#include <iostream>

#include "grhsim_top_module.hpp"

static uint8_t bitrev8(uint8_t value)
{
    uint8_t result = 0;
    for (int i = 0; i < 8; ++i)
    {
        result |= ((value >> (7 - i)) & 0x1u) << i;
    }
    return result;
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<uint8_t, 4> stimuli{{0x00u, 0xFFu, 0xA5u, 0x00u}};
    for (uint8_t value : stimuli)
    {
        sim.in = value;
        sim.eval();

        const uint8_t expected = bitrev8(value);
        if (sim.out != expected)
        {
            std::cerr << "[GrhTB] dut_017 failed: in=0x" << std::hex << static_cast<int>(value)
                      << ", expected out=0x" << static_cast<int>(expected)
                      << ", got 0x" << static_cast<int>(sim.out) << std::dec << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_017 passed: out is bit-reversed in\n";
    return EXIT_SUCCESS;
}
