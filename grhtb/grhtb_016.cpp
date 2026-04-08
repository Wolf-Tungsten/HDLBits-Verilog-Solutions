#include <array>
#include <cstdlib>
#include <cstdint>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus_016
{
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t f;
};

static uint32_t pack32(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f)
{
    uint32_t result = 0;
    result |= (static_cast<uint32_t>(a & 0x1Fu) << 27);
    result |= (static_cast<uint32_t>(b & 0x1Fu) << 22);
    result |= (static_cast<uint32_t>(c & 0x1Fu) << 17);
    result |= (static_cast<uint32_t>(d & 0x1Fu) << 12);
    result |= (static_cast<uint32_t>(e & 0x1Fu) << 7);
    result |= (static_cast<uint32_t>(f & 0x1Fu) << 2);
    result |= 0x3u;
    return result;
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<Stimulus_016, 4> stimuli{{
        {0, 0, 0, 0, 0, 0},
        {31, 31, 31, 31, 31, 31},
        {5, 9, 17, 3, 24, 12},
        {0, 0, 0, 0, 0, 0},
    }};

    for (const auto &stim : stimuli)
    {
        sim.a = stim.a;
        sim.b = stim.b;
        sim.c = stim.c;
        sim.d = stim.d;
        sim.e = stim.e;
        sim.f = stim.f;
        sim.eval();

        const uint32_t packed = pack32(stim.a, stim.b, stim.c, stim.d, stim.e, stim.f);
        const uint8_t expectedW = static_cast<uint8_t>((packed >> 24) & 0xFFu);
        const uint8_t expectedX = static_cast<uint8_t>((packed >> 16) & 0xFFu);
        const uint8_t expectedY = static_cast<uint8_t>((packed >> 8) & 0xFFu);
        const uint8_t expectedZ = static_cast<uint8_t>(packed & 0xFFu);

        if (sim.w != expectedW || sim.x != expectedX || sim.y != expectedY || sim.z != expectedZ)
        {
            std::cerr << "[GrhTB] dut_016 failed: expected wxyz="
                      << static_cast<int>(expectedW) << ","
                      << static_cast<int>(expectedX) << ","
                      << static_cast<int>(expectedY) << ","
                      << static_cast<int>(expectedZ)
                      << ", got " << static_cast<int>(sim.w) << ","
                      << static_cast<int>(sim.x) << ","
                      << static_cast<int>(sim.y) << ","
                      << static_cast<int>(sim.z) << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_016 passed: concatenation mapping verified\n";
    return EXIT_SUCCESS;
}
