#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static void set_all_inputs(GrhSIM_top_module &sim, std::uint16_t value)
{
    sim.a = value;
    sim.b = value;
    sim.c = value;
    sim.d = value;
    sim.e = value;
    sim.f = value;
    sim.g = value;
    sim.h = value;
    sim.i = value;
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    const std::array<std::uint16_t, 9> values{{0x0000u, 0x1111u, 0x2222u, 0x3333u, 0x4444u,
                                               0x5555u, 0x6666u, 0x7777u, 0x8888u}};
    sim.a = values[0];
    sim.b = values[1];
    sim.c = values[2];
    sim.d = values[3];
    sim.e = values[4];
    sim.f = values[5];
    sim.g = values[6];
    sim.h = values[7];
    sim.i = values[8];

    for (int sel = 0; sel < 9; ++sel) {
        sim.sel = static_cast<std::uint8_t>(sel);
        sim.eval();
        if (sim.out != values[static_cast<std::size_t>(sel)]) {
            return EXIT_FAILURE;
        }
    }

    sim.sel = 15;
    sim.eval();
    if (sim.out != 0xFFFFu) {
        return EXIT_FAILURE;
    }

    for (int sel = 0; sel < 9; ++sel) {
        set_all_inputs(sim, 0x0000u);
        sim.sel = static_cast<std::uint8_t>(sel);
        sim.eval();
        if (sim.out != 0x0000u) {
            return EXIT_FAILURE;
        }

        switch (sel) {
        case 0: sim.a = 0xFFFFu; break;
        case 1: sim.b = 0xFFFFu; break;
        case 2: sim.c = 0xFFFFu; break;
        case 3: sim.d = 0xFFFFu; break;
        case 4: sim.e = 0xFFFFu; break;
        case 5: sim.f = 0xFFFFu; break;
        case 6: sim.g = 0xFFFFu; break;
        case 7: sim.h = 0xFFFFu; break;
        case 8: sim.i = 0xFFFFu; break;
        default: return EXIT_FAILURE;
        }

        sim.eval();
        if (sim.out != 0xFFFFu) {
            return EXIT_FAILURE;
        }

        set_all_inputs(sim, 0x0000u);
        sim.eval();
        if (sim.out != 0x0000u) {
            return EXIT_FAILURE;
        }
    }

    set_all_inputs(sim, 0x0000u);
    sim.a = 0xFFFFu;
    for (int rep = 0; rep < 2; ++rep) {
        sim.sel = 0;
        sim.eval();
        if (sim.out != 0xFFFFu) {
            return EXIT_FAILURE;
        }
        sim.sel = 15;
        sim.eval();
        if (sim.out != 0xFFFFu) {
            return EXIT_FAILURE;
        }
        sim.sel = 0;
        sim.eval();
        if (sim.out != 0xFFFFu) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[GrhTB] dut_063 passed: 9:1 mux with default\n";
    return EXIT_SUCCESS;
}
