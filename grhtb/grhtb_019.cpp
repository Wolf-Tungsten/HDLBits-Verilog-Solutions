#include <array>
#include <cstdlib>
#include <cstdint>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus_019
{
    bool a;
    bool b;
    bool c;
    bool d;
    bool e;
};

static uint32_t buildW1(bool a, bool b, bool c, bool d, bool e)
{
    uint32_t value = 0;
    for (int i = 0; i < 5; ++i)
    {
        value = (value << 1) | static_cast<uint32_t>(a);
    }
    for (int i = 0; i < 5; ++i)
    {
        value = (value << 1) | static_cast<uint32_t>(b);
    }
    for (int i = 0; i < 5; ++i)
    {
        value = (value << 1) | static_cast<uint32_t>(c);
    }
    for (int i = 0; i < 5; ++i)
    {
        value = (value << 1) | static_cast<uint32_t>(d);
    }
    for (int i = 0; i < 5; ++i)
    {
        value = (value << 1) | static_cast<uint32_t>(e);
    }
    return value & 0x1FFFFFFu;
}

static uint32_t buildW2(bool a, bool b, bool c, bool d, bool e)
{
    uint32_t value = 0;
    for (int rep = 0; rep < 5; ++rep)
    {
        value = (value << 1) | static_cast<uint32_t>(a);
        value = (value << 1) | static_cast<uint32_t>(b);
        value = (value << 1) | static_cast<uint32_t>(c);
        value = (value << 1) | static_cast<uint32_t>(d);
        value = (value << 1) | static_cast<uint32_t>(e);
    }
    return value & 0x1FFFFFFu;
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<Stimulus_019, 7> stimuli{{
        {false, false, false, false, false},
        {true, false, false, false, false},
        {true, true, false, false, false},
        {true, true, true, false, false},
        {true, true, true, true, false},
        {true, true, true, true, true},
        {false, false, false, false, false},
    }};

    for (const auto &stim : stimuli)
    {
        sim.a = stim.a;
        sim.b = stim.b;
        sim.c = stim.c;
        sim.d = stim.d;
        sim.e = stim.e;
        sim.eval();

        const uint32_t w1 = buildW1(stim.a, stim.b, stim.c, stim.d, stim.e);
        const uint32_t w2 = buildW2(stim.a, stim.b, stim.c, stim.d, stim.e);
        const uint32_t expected = (~(w1 ^ w2)) & 0x1FFFFFFu;
        if ((sim.out & 0x1FFFFFFu) != expected)
        {
            std::cerr << "[GrhTB] dut_019 failed: expected out=0x" << std::hex << expected
                      << ", got 0x" << (sim.out & 0x1FFFFFFu) << std::dec << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_019 passed: replicated structure and bitwise ops verified\n";
    return EXIT_SUCCESS;
}
