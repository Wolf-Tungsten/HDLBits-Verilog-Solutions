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
    sim.resetn = false;
    sim.in = false;
    sim.eval();

    sim.clk = true;
    sim.eval();
    if (sim.out) {
        std::cerr << "[GrhTB] dut_113 failed(initial reset)\n";
        return EXIT_FAILURE;
    }
    sim.clk = false;
    sim.eval();

    std::uint8_t q0 = 0u;
    std::uint8_t q1 = 0u;
    std::uint8_t q2 = 0u;
    const std::array<bool, 16> inSeq{{false, true, false, true, true, false, false, true,
                                      true,  false, false, true, false, true,  false, false}};

    sim.resetn = true;
    for (std::size_t i = 0; i < inSeq.size(); ++i) {
        sim.in = inSeq[i];
        sim.clk = true;
        sim.eval();

        const std::uint8_t prevQ0 = q0;
        const std::uint8_t prevQ1 = q1;
        const std::uint8_t prevQ2 = q2;
        q0 = inSeq[i] ? 1u : 0u;
        q1 = prevQ0;
        q2 = prevQ1;
        const std::uint8_t outModel = prevQ2;
        if (sim.out != (outModel != 0u)) {
            std::cerr << "[GrhTB] dut_113 failed(shift posedge " << i << ")\n";
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (sim.out != (outModel != 0u)) {
            std::cerr << "[GrhTB] dut_113 failed(shift negedge " << i << ")\n";
            return EXIT_FAILURE;
        }
    }

    sim.resetn = false;
    sim.in = true;
    sim.clk = true;
    sim.eval();
    if (sim.out) {
        std::cerr << "[GrhTB] dut_113 failed(reset posedge)\n";
        return EXIT_FAILURE;
    }
    sim.clk = false;
    sim.eval();
    if (sim.out) {
        std::cerr << "[GrhTB] dut_113 failed(reset negedge)\n";
        return EXIT_FAILURE;
    }

    std::cout << "[GrhTB] dut_113 passed: 3-stage shift with reset\n";
    return EXIT_SUCCESS;
}
