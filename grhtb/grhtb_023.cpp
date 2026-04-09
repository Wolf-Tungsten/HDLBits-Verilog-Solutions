#include <array>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;

    const std::array<bool, 7> stimuli{{true, false, true, true, false, false, true}};
    bool w1 = false;
    bool w2 = false;

    for (const bool din : stimuli) {
        sim.d = din;
        const bool expected_q = w2;

        sim.clk = true;
        sim.eval();
        if (sim.q != expected_q) {
            std::cerr << "[GrhTB] dut_023 failed(posedge): d=" << static_cast<int>(din)
                      << ", expected q=" << static_cast<int>(expected_q)
                      << ", got " << static_cast<int>(sim.q) << '\n';
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (sim.q != expected_q) {
            std::cerr << "[GrhTB] dut_023 failed(negedge): d=" << static_cast<int>(din)
                      << ", expected q=" << static_cast<int>(expected_q)
                      << ", got " << static_cast<int>(sim.q) << '\n';
            return EXIT_FAILURE;
        }

        const bool next_w1 = din;
        const bool next_w2 = w1;
        w1 = next_w1;
        w2 = next_w2;
    }

    std::cout << "[GrhTB] dut_023 passed: q follows a 3-stage DFF pipeline\n";
    return EXIT_SUCCESS;
}
