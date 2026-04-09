#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static std::uint8_t ref(std::uint8_t a, std::uint8_t b, std::uint8_t c, std::uint8_t d)
{
    const std::uint8_t na = static_cast<std::uint8_t>(~a & 1u);
    const std::uint8_t nb = static_cast<std::uint8_t>(~b & 1u);
    const std::uint8_t nc = static_cast<std::uint8_t>(~c & 1u);
    const std::uint8_t nd = static_cast<std::uint8_t>(~d & 1u);
    const std::uint8_t t0 = static_cast<std::uint8_t>(na & b & nc & nd);
    const std::uint8_t t1 = static_cast<std::uint8_t>(a & nb & nc & nd);
    const std::uint8_t t2 = static_cast<std::uint8_t>(na & nb & nc & d);
    const std::uint8_t t3 = static_cast<std::uint8_t>(a & b & nc & d);
    const std::uint8_t t4 = static_cast<std::uint8_t>(na & b & c & d);
    const std::uint8_t t5 = static_cast<std::uint8_t>(a & nb & c & d);
    const std::uint8_t t6 = static_cast<std::uint8_t>(na & nb & c & nd);
    const std::uint8_t t7 = static_cast<std::uint8_t>(a & b & c & nd);
    return static_cast<std::uint8_t>((t0 | t1 | t2 | t3 | t4 | t5 | t6 | t7) & 1u);
}

static bool check_outputs(GrhSIM_top_module &sim, std::uint8_t a, std::uint8_t b, std::uint8_t c, std::uint8_t d)
{
    sim.a = a;
    sim.b = b;
    sim.c = c;
    sim.d = d;
    sim.eval();
    return sim.out == ref(a, b, c, d);
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    for (int value = 0; value < 16; ++value) {
        if (!check_outputs(sim,
                           static_cast<std::uint8_t>(value & 1u),
                           static_cast<std::uint8_t>((value >> 1) & 1u),
                           static_cast<std::uint8_t>((value >> 2) & 1u),
                           static_cast<std::uint8_t>((value >> 3) & 1u))) {
            return EXIT_FAILURE;
        }
    }

    for (int rep = 0; rep < 2; ++rep) {
        if (!check_outputs(sim, 0, 0, 0, 0) || !check_outputs(sim, 1, 0, 0, 0) || !check_outputs(sim, 0, 0, 0, 0) ||
            !check_outputs(sim, 0, 1, 0, 0) || !check_outputs(sim, 0, 0, 0, 0) || !check_outputs(sim, 0, 0, 1, 0) ||
            !check_outputs(sim, 0, 0, 0, 0) || !check_outputs(sim, 0, 0, 0, 1) || !check_outputs(sim, 0, 0, 0, 0)) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_076 passed: 4-input sum-of-products logic\n";
    return EXIT_SUCCESS;
}
