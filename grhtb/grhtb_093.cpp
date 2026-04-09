#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.x = false;
    sim.eval();

    const std::array<bool, 8> stimuli{{false, true, false, true, true, false, true, false}};
    bool q0 = false;
    bool q1 = false;
    bool q2 = false;

    for (const bool x : stimuli) {
        sim.x = x;
        sim.clk = true;
        sim.eval();

        const bool prev_q0 = q0;
        const bool prev_q1 = q1;
        const bool prev_q2 = q2;
        q0 = static_cast<bool>(prev_q0 ^ x);
        q1 = static_cast<bool>((!prev_q1) & x);
        q2 = static_cast<bool>((!prev_q2) | x);
        const bool zModel = !(q0 | q1 | q2);
        if (sim.z != zModel) {
            std::cerr << "[GrhTB] dut_093 failed(posedge)\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (sim.z != zModel) {
            std::cerr << "[GrhTB] dut_093 failed(negedge)\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_093 passed: 3-bit state machine output\n";
    return EXIT_SUCCESS;
}
