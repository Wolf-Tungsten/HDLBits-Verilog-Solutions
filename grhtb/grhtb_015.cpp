#include <array>
#include <cstdlib>
#include <cstdint>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<uint8_t, 4> stimuli{{0x0u, 0xFu, 0x1u, 0x0u}};
    for (uint8_t value : stimuli)
    {
        sim.in = value & 0xFu;
        sim.eval();

        const bool expectedAnd = (value & 0xFu) == 0xFu;
        const bool expectedOr = (value & 0xFu) != 0u;
        const bool expectedXor = (((value >> 3) ^ (value >> 2) ^ (value >> 1) ^ value) & 0x1u) != 0;
        if (sim.out_and != expectedAnd ||
            sim.out_or != expectedOr ||
            sim.out_xor != expectedXor)
        {
            std::cerr << "[GrhTB] dut_015 failed: in=" << static_cast<int>(value)
                      << ", exp and/or/xor=" << static_cast<int>(expectedAnd)
                      << "/" << static_cast<int>(expectedOr)
                      << "/" << static_cast<int>(expectedXor)
                      << ", got " << static_cast<int>(sim.out_and)
                      << "/" << static_cast<int>(sim.out_or)
                      << "/" << static_cast<int>(sim.out_xor) << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_015 passed: reductions AND/OR/XOR match\n";
    return EXIT_SUCCESS;
}
