#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.aresetn = false;
    sim.x = false;
    sim.eval();

    enum State : std::uint8_t { IDLE = 0, ONE = 1, ONE_ZERO = 2 };
    State state = IDLE;

    auto mealy_z = [](State st, bool x) {
        return st == ONE_ZERO && x;
    };

    auto posedge = [&]() {
        sim.clk = true;
        sim.eval();
    };
    auto negedge = [&]() {
        sim.clk = false;
        sim.eval();
    };

    negedge();
    posedge();
    state = IDLE;
    sim.aresetn = true;

    auto step = [&](bool x) {
        sim.x = x;
        negedge();
        if (sim.z != mealy_z(state, x)) {
            std::cerr << "[GrhTB] dut_139 failed(comb)\n";
            return false;
        }

        State next = IDLE;
        switch (state) {
        case IDLE: next = x ? ONE : IDLE; break;
        case ONE: next = x ? ONE : ONE_ZERO; break;
        case ONE_ZERO: next = x ? ONE : IDLE; break;
        }
        posedge();
        state = next;
        if (sim.z != mealy_z(state, x)) {
            std::cerr << "[GrhTB] dut_139 failed(posedge)\n";
            return false;
        }
        return true;
    };

    for (const auto bit : {true, false, true, true, false, true, false, false}) {
        if (!step(bit)) {
            return EXIT_FAILURE;
        }
    }

    sim.aresetn = false;
    negedge();
    posedge();
    state = IDLE;
    sim.aresetn = true;

    for (const auto bit : {false, true, false, true, false}) {
        if (!step(bit)) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_139 passed: overlapping 101 detector FSM\n";
    return EXIT_SUCCESS;
}
