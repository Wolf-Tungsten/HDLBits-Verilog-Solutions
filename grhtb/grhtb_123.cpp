#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    for (std::uint8_t state = 0; state < 4; ++state) {
        for (std::uint8_t in = 0; in < 2; ++in) {
            sim.state = state;
            sim.in = in != 0u;
            sim.eval();

            std::uint8_t next = 0u;
            switch (state) {
            case 0: next = in ? 1u : 0u; break;
            case 1: next = in ? 1u : 2u; break;
            case 2: next = in ? 3u : 0u; break;
            case 3: next = in ? 1u : 2u; break;
            default: break;
            }
            if ((sim.next_state & 0x3u) != next || sim.out != (state == 3u)) {
                std::cerr << "[GrhTB] dut_123 failed(state=" << static_cast<int>(state)
                          << ", in=" << static_cast<int>(in) << ")\n";
                return EXIT_FAILURE;
            }
        }
    }

    std::cout << "[GrhTB] dut_123 passed: Mealy combinational FSM block\n";
    return EXIT_SUCCESS;
}
