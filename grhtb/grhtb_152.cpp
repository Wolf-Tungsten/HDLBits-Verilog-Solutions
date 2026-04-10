#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.reset = true;
    sim.eval();

    auto tick = [&]() {
        sim.clk = true;
        sim.eval();
        sim.clk = false;
        sim.eval();
    };

    tick();
    sim.reset = false;

    std::uint16_t qModel = 0u;
    for (int cycle = 0; cycle < 1105; ++cycle) {
        if ((sim.q & 0x3FFu) != qModel) {
            std::cerr << "[GrhTB] dut_152 failed(cycle=" << cycle << ")\n";
            return EXIT_FAILURE;
        }
        if (qModel < 999u) {
            ++qModel;
        } else {
            qModel = 0u;
        }
        tick();
    }

    std::cout << "[GrhTB] dut_152 passed: 0..999 counter with wrap-around\n";
    return EXIT_SUCCESS;
}
