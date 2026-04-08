#include <array>
#include <cstdlib>
#include <cstdint>
#include <iostream>

#include "grhsim_top_module.hpp"

static uint32_t sext8to32(uint8_t value)
{
    return static_cast<uint32_t>(static_cast<int32_t>(static_cast<int8_t>(value)));
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<uint8_t, 4> stimuli{{0x00u, 0x80u, 0x7Fu, 0xFFu}};
    for (uint8_t value : stimuli)
    {
        sim.in = value;
        sim.eval();

        const uint32_t expected = sext8to32(value);
        if (sim.out != expected)
        {
            std::cerr << "[GrhTB] dut_018 failed: in=0x" << std::hex << static_cast<int>(value)
                      << ", expected out=0x" << expected
                      << ", got 0x" << sim.out << std::dec << '\n';
            return EXIT_FAILURE;
        }
    }

    sim.in = 0x00u;
    sim.eval();
    sim.in = 0xFFu;
    sim.eval();
    sim.in = 0x00u;
    sim.eval();

    std::cout << "[GrhTB] dut_018 passed: out is 32-bit sign-extended in\n";
    return EXIT_SUCCESS;
}
