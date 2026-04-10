#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static bool state_bit(std::uint16_t state, std::uint8_t index)
{
    return ((state >> index) & 1u) != 0u;
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    auto check = [&](std::uint16_t state, bool d, bool doneCounting, bool ack) {
        sim.state = state;
        sim.d = d;
        sim.done_counting = doneCounting;
        sim.ack = ack;
        sim.eval();

        const bool b3Next = state_bit(state, 6);
        const bool sNext = (state_bit(state, 0) && !d) || (state_bit(state, 1) && !d) || (state_bit(state, 3) && !d) ||
                           (state_bit(state, 9) && ack);
        const bool s1Next = state_bit(state, 0) && d;
        const bool countNext = state_bit(state, 7) || (state_bit(state, 8) && !doneCounting);
        const bool waitNext = (state_bit(state, 8) && doneCounting) || (state_bit(state, 9) && !ack);
        const bool done = state_bit(state, 9);
        const bool counting = state_bit(state, 8);
        const bool shiftEna = state_bit(state, 4) || state_bit(state, 5) || state_bit(state, 6) || state_bit(state, 7);

        if (static_cast<bool>(sim.B3_next) != b3Next || static_cast<bool>(sim.S_next) != sNext ||
            static_cast<bool>(sim.S1_next) != s1Next || static_cast<bool>(sim.Count_next) != countNext ||
            static_cast<bool>(sim.Wait_next) != waitNext || static_cast<bool>(sim.done) != done ||
            static_cast<bool>(sim.counting) != counting || static_cast<bool>(sim.shift_ena) != shiftEna) {
            std::cerr << "[GrhTB] dut_158 failed(state=0x" << std::hex << state << std::dec << ", d=" << static_cast<int>(d)
                      << ", done_counting=" << static_cast<int>(doneCounting) << ", ack=" << static_cast<int>(ack) << ")\n";
            return false;
        }
        return true;
    };

    for (std::uint8_t s = 0; s < 10; ++s) {
        const std::uint16_t onehot = static_cast<std::uint16_t>(1u << s);
        for (const bool d : {false, true}) {
            for (const bool doneCounting : {false, true}) {
                for (const bool ack : {false, true}) {
                    if (!check(onehot, d, doneCounting, ack)) {
                        return EXIT_FAILURE;
                    }
                }
            }
        }
    }
    if (!check(0u, false, false, false)) {
        return EXIT_FAILURE;
    }

    std::cout << "[GrhTB] dut_158 passed: one-hot next-state/output combinational block\n";
    return EXIT_SUCCESS;
}
