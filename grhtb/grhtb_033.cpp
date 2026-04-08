#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    auto check = [&](std::uint8_t expected, const char *ctx) {
        if (sim.out != expected) {
            std::cerr << "[GrhTB] dut_033 failed(" << ctx << "): sel=" << static_cast<int>(sim.sel)
                      << ", expected=" << static_cast<int>(expected)
                      << ", got " << static_cast<int>(sim.out) << '\n';
            std::exit(EXIT_FAILURE);
        }
    };

    sim.data0 = 0x0;
    sim.data1 = 0x0;
    sim.data2 = 0x0;
    sim.data3 = 0x0;
    sim.data4 = 0x0;
    sim.data5 = 0x0;

    sim.sel = 0;
    sim.eval();
    sim.sel = 7;
    sim.eval();
    check(0x0, "default-7");
    sim.sel = 0;
    sim.eval();
    check(0x0, "select-0-init");

    for (int i = 0; i <= 5; ++i) {
        sim.sel = static_cast<std::uint8_t>(i);
        sim.eval();
        check(0x0, "observe-zero");

        switch (i) {
        case 0: sim.data0 = 0xF; break;
        case 1: sim.data1 = 0xF; break;
        case 2: sim.data2 = 0xF; break;
        case 3: sim.data3 = 0xF; break;
        case 4: sim.data4 = 0xF; break;
        case 5: sim.data5 = 0xF; break;
        }
        sim.eval();
        check(0xF, "drive-ones");

        switch (i) {
        case 0: sim.data0 = 0x0; break;
        case 1: sim.data1 = 0x0; break;
        case 2: sim.data2 = 0x0; break;
        case 3: sim.data3 = 0x0; break;
        case 4: sim.data4 = 0x0; break;
        case 5: sim.data5 = 0x0; break;
        }
        sim.eval();
        check(0x0, "drive-zeros");
    }

    sim.sel = 6;
    sim.eval();
    check(0x0, "default-6");

    std::cout << "[GrhTB] dut_033 passed: 6:1 mux selects correct data; default hit; all inputs toggled\n";
    return EXIT_SUCCESS;
}
