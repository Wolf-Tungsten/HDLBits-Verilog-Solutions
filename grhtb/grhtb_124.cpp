#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    for (std::uint8_t state = 0; state < 4; ++state) {
        const std::uint8_t stateVec = static_cast<std::uint8_t>(1u << state);
        for (std::uint8_t in = 0; in < 2; ++in) {
            sim.state = stateVec;
            sim.in = in != 0u;
            sim.eval();

            std::uint8_t nextVec = 0u;
            switch (state) {
            case 0: nextVec = in ? 0x2u : 0x1u; break;
            case 1: nextVec = in ? 0x2u : 0x4u; break;
            case 2: nextVec = in ? 0x8u : 0x1u; break;
            case 3: nextVec = in ? 0x2u : 0x4u; break;
            default: break;
            }
            if ((sim.next_state & 0xFu) != nextVec || sim.out != (state == 3u)) {
                std::cerr << "[GrhTB] dut_124 failed(state=" << static_cast<int>(state)
                          << ", in=" << static_cast<int>(in) << ")\n";
                return EXIT_FAILURE;
            }
        }
    }

    std::cout << "[GrhTB] dut_124 passed: one-hot encoded FSM combinational block\n";
    return EXIT_SUCCESS;
}
