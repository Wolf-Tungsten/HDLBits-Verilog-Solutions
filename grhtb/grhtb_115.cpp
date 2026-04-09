#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static bool check_outputs(GrhSIM_top_module &sim, std::uint8_t qModel)
{
    for (std::uint8_t idx = 0; idx < 8; ++idx) {
        sim.A = static_cast<bool>((idx >> 2) & 1u);
        sim.B = static_cast<bool>((idx >> 1) & 1u);
        sim.C = static_cast<bool>(idx & 1u);
        sim.eval();
        const bool expected = ((qModel >> idx) & 1u) != 0u;
        if (sim.Z != expected) {
            std::cerr << "[GrhTB] dut_115 failed(addr=" << static_cast<int>(idx) << ")\n";
            return false;
        }
    }
    return true;
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.enable = false;
    sim.S = false;
    sim.A = false;
    sim.B = false;
    sim.C = false;
    sim.eval();

    std::uint8_t qModel = 0u;
    const std::array<bool, 8> serialIn{{true, false, true, true, false, false, true, false}};

    sim.enable = true;
    for (std::size_t i = 0; i < serialIn.size(); ++i) {
        sim.S = serialIn[i];
        sim.clk = true;
        sim.eval();
        qModel = static_cast<std::uint8_t>(((qModel << 1) & 0xFEu) | (serialIn[i] ? 1u : 0u));
        if (!check_outputs(sim, qModel)) {
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (!check_outputs(sim, qModel)) {
            return EXIT_FAILURE;
        }
    }

    sim.enable = false;
    const std::uint8_t held = qModel;
    for (int i = 0; i < 4; ++i) {
        sim.S = (i & 1) != 0;
        sim.clk = true;
        sim.eval();
        if (!check_outputs(sim, held)) {
            return EXIT_FAILURE;
        }

        sim.clk = false;
        sim.eval();
        if (!check_outputs(sim, held)) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_115 passed: serial load + muxed readout\n";
    return EXIT_SUCCESS;
}
