#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static std::uint8_t umin(std::uint8_t x, std::uint8_t y)
{
    return x < y ? x : y;
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();

    auto drive_and_check = [&](std::uint8_t a, std::uint8_t b, std::uint8_t c, std::uint8_t d, const char *ctx) {
        sim.a = a;
        sim.b = b;
        sim.c = c;
        sim.d = d;
        sim.eval();

        const std::uint8_t expected = umin(umin(a, b), umin(c, d));
        if (sim.min != expected) {
            std::cerr << "[GrhTB] dut_037 failed(" << ctx << "): a/b/c/d="
                      << static_cast<int>(a) << "/" << static_cast<int>(b)
                      << "/" << static_cast<int>(c) << "/" << static_cast<int>(d)
                      << ", expected min=" << static_cast<int>(expected)
                      << ", got " << static_cast<int>(sim.min) << '\n';
            std::exit(EXIT_FAILURE);
        }
    };

    drive_and_check(1, 2, 3, 4, "sanity1");
    drive_and_check(5, 2, 1, 7, "sanity2");
    drive_and_check(9, 8, 10, 7, "sanity3");
    drive_and_check(0, 255, 128, 64, "sanity4");

    for (int rep = 0; rep < 2; ++rep) {
        drive_and_check(0x00, 0xFF, 0xFF, 0xFF, "A a=00");
        drive_and_check(0xFF, 0xFF, 0xFF, 0xFF, "A a=FF");
    }
    drive_and_check(0x00, 0xFF, 0xFF, 0xFF, "A a=00 end");

    for (int rep = 0; rep < 2; ++rep) {
        drive_and_check(0xFF, 0x00, 0xFF, 0xFF, "B b=00");
        drive_and_check(0xFF, 0xFF, 0xFF, 0xFF, "B b=FF");
    }
    drive_and_check(0xFF, 0x00, 0xFF, 0xFF, "B b=00 end");

    for (int rep = 0; rep < 2; ++rep) {
        drive_and_check(0xFF, 0xFF, 0x00, 0xFF, "C c=00");
        drive_and_check(0xFF, 0xFF, 0xFF, 0xFF, "C c=FF");
    }
    drive_and_check(0xFF, 0xFF, 0x00, 0xFF, "C c=00 end");

    for (int rep = 0; rep < 2; ++rep) {
        drive_and_check(0xFF, 0xFF, 0xFF, 0x00, "D d=00");
        drive_and_check(0xFF, 0xFF, 0xFF, 0xFF, "D d=FF");
    }
    drive_and_check(0xFF, 0xFF, 0xFF, 0x00, "D d=00 end");

    std::cout << "[GrhTB] dut_037 passed: min(a,b,c,d) computed via chained mins\n";
    return EXIT_SUCCESS;
}
