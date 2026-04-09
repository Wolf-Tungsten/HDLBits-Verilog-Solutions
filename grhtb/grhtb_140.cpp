#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.areset = true;
    sim.x = false;
    sim.eval();

    enum State : std::uint8_t { A = 0, B = 1, C = 2 };
    State state = A;

    auto tick = [&]() {
        sim.clk = true;
        sim.eval();
        sim.clk = false;
        sim.eval();
    };

    tick();
    state = A;
    sim.areset = false;

    auto step = [&](bool x) {
        State next = A;
        switch (state) {
        case A: next = x ? B : A; break;
        case B: next = x ? C : B; break;
        case C: next = x ? C : B; break;
        }
        sim.x = x;
        tick();
        state = next;
        if (sim.z != (state == B)) {
            std::cerr << "[GrhTB] dut_140 failed(x=" << static_cast<int>(x) << ")\n";
            return false;
        }
        return true;
    };

    for (const auto bit : {false, true, true, false, true, false, false, true, true, true}) {
        if (!step(bit)) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_140 passed: 3-state FSM with z on state B\n";
    return EXIT_SUCCESS;
}
