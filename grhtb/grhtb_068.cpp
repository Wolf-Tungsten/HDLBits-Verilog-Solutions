#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static void ref(std::uint8_t a, std::uint8_t b, std::uint8_t cin, std::uint8_t &coutVec, std::uint8_t &sumVec)
{
    std::uint8_t carry = cin & 1u;
    sumVec = 0;
    coutVec = 0;
    for (int bit = 0; bit < 3; ++bit) {
        const std::uint8_t sum = static_cast<std::uint8_t>(((a >> bit) & 1u) ^ ((b >> bit) & 1u) ^ carry);
        const std::uint8_t cout =
            static_cast<std::uint8_t>((((a >> bit) & 1u) & ((b >> bit) & 1u)) |
                                      (((a >> bit) & 1u) & carry) |
                                      (((b >> bit) & 1u) & carry));
        if (sum) {
            sumVec |= static_cast<std::uint8_t>(1u << bit);
        }
        if (cout) {
            coutVec |= static_cast<std::uint8_t>(1u << bit);
        }
        carry = cout;
    }
}

static bool check_outputs(GrhSIM_top_module &sim, std::uint8_t a, std::uint8_t b, std::uint8_t cin)
{
    sim.a = a & 7u;
    sim.b = b & 7u;
    sim.cin = cin;
    sim.eval();

    std::uint8_t coutVec = 0;
    std::uint8_t sumVec = 0;
    ref(a, b, cin, coutVec, sumVec);
    return sim.sum == sumVec && sim.cout == coutVec;
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    for (int a = 0; a < 8; ++a) {
        for (int b = 0; b < 8; ++b) {
            for (int cin = 0; cin < 2; ++cin) {
                if (!check_outputs(sim,
                                   static_cast<std::uint8_t>(a),
                                   static_cast<std::uint8_t>(b),
                                   static_cast<std::uint8_t>(cin))) {
                    return EXIT_FAILURE;
                }
            }
        }
    }

    for (int bit = 0; bit < 3; ++bit) {
        for (int rep = 0; rep < 2; ++rep) {
            if (!check_outputs(sim, 0, 0, 0) || !check_outputs(sim, static_cast<std::uint8_t>(1u << bit), 0, 0) ||
                !check_outputs(sim, 0, 0, 0) || !check_outputs(sim, 0, static_cast<std::uint8_t>(1u << bit), 0) ||
                !check_outputs(sim, 0, 0, 0)) {
                return EXIT_FAILURE;
            }
        }
    }

    for (int rep = 0; rep < 2; ++rep) {
        if (!check_outputs(sim, 0, 0, 0) || !check_outputs(sim, 0, 0, 1) || !check_outputs(sim, 0, 0, 0)) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_068 passed: 3-bit ripple\n";
    return EXIT_SUCCESS;
}
