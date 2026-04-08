#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

struct Stimulus_021 {
    bool a;
    bool b;
    bool c;
    bool d;
};

static inline bool compute_out1(const Stimulus_021 &s)
{
    return (s.a && s.b) || (s.c && s.d);
}

static inline bool compute_out2(const Stimulus_021 &s)
{
    return (s.a || s.b) && (s.c || s.d);
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<Stimulus_021, 11> stimuli{{
        {false, false, false, false},
        {true, false, false, false},
        {false, true, false, false},
        {false, false, true, false},
        {false, false, false, true},
        {true, true, false, false},
        {false, false, true, true},
        {false, true, true, false},
        {false, true, false, true},
        {true, false, true, true},
        {false, false, false, false},
    }};

    for (const auto &stim : stimuli) {
        sim.a = stim.a;
        sim.b = stim.b;
        sim.c = stim.c;
        sim.d = stim.d;
        sim.eval();

        const bool exp1 = compute_out1(stim);
        const bool exp2 = compute_out2(stim);
        if (sim.out1 != exp1 || sim.out2 != exp2) {
            std::cerr << "[GrhTB] dut_021 failed: a=" << static_cast<int>(stim.a)
                      << ", b=" << static_cast<int>(stim.b)
                      << ", c=" << static_cast<int>(stim.c)
                      << ", d=" << static_cast<int>(stim.d)
                      << ", expected out1/out2=" << static_cast<int>(exp1)
                      << "/" << static_cast<int>(exp2)
                      << ", got " << static_cast<int>(sim.out1)
                      << "/" << static_cast<int>(sim.out2) << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_021 passed: out1=(a&b)|(c&d), out2=(a|b)&(c|d)\n";
    return EXIT_SUCCESS;
}
