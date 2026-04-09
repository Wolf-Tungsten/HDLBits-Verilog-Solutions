#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "grhsim_top_module.hpp"

static bool is_walk_left(const GrhSIM_top_module &sim)
{
    return sim.walk_left && !sim.walk_right && !sim.aaah && !sim.digging;
}

static bool is_walk_right(const GrhSIM_top_module &sim)
{
    return !sim.walk_left && sim.walk_right && !sim.aaah && !sim.digging;
}

static bool is_fall(const GrhSIM_top_module &sim)
{
    return sim.aaah && !sim.digging;
}

static bool is_dig(const GrhSIM_top_module &sim)
{
    return sim.digging && !sim.aaah;
}

static bool is_splatter(const GrhSIM_top_module &sim)
{
    return !sim.walk_left && !sim.walk_right && !sim.aaah && !sim.digging;
}

int main()
{
    GrhSIM_top_module sim;
    sim.init();
    sim.clk = false;
    sim.areset = false;
    sim.bump_left = false;
    sim.bump_right = false;
    sim.ground = true;
    sim.dig = false;
    sim.eval();

    auto tick = [&]() {
        sim.clk = true;
        sim.eval();
        sim.clk = false;
        sim.eval();
    };

    auto reset_to_walk_left = [&]() {
        sim.areset = true;
        sim.bump_left = false;
        sim.bump_right = false;
        sim.ground = true;
        sim.dig = false;
        sim.eval();
        tick();
        sim.areset = false;
        if (!is_walk_left(sim)) {
            std::cerr << "[GrhTB] dut_131 failed(reset)\n";
            std::exit(EXIT_FAILURE);
        }
    };

    reset_to_walk_left();
    sim.bump_left = true;
    tick();
    sim.bump_left = false;
    if (!is_walk_right(sim)) {
        std::cerr << "[GrhTB] dut_131 failed(walk_r_after_bump_left)\n";
        return EXIT_FAILURE;
    }

    sim.bump_right = true;
    tick();
    sim.bump_right = false;
    if (!is_walk_left(sim)) {
        std::cerr << "[GrhTB] dut_131 failed(walk_l_after_bump_right)\n";
        return EXIT_FAILURE;
    }

    sim.dig = true;
    sim.ground = true;
    tick();
    if (!is_dig(sim)) {
        std::cerr << "[GrhTB] dut_131 failed(dig_l)\n";
        return EXIT_FAILURE;
    }

    sim.ground = false;
    tick();
    if (!is_fall(sim)) {
        std::cerr << "[GrhTB] dut_131 failed(fall_l_start)\n";
        return EXIT_FAILURE;
    }

    sim.dig = false;
    for (int i = 0; i < 3; ++i) {
        tick();
        if (!is_fall(sim)) {
            std::cerr << "[GrhTB] dut_131 failed(fall_l_loop)\n";
            return EXIT_FAILURE;
        }
    }

    sim.ground = true;
    tick();
    if (!is_walk_left(sim)) {
        std::cerr << "[GrhTB] dut_131 failed(fall_l_to_walk_l)\n";
        return EXIT_FAILURE;
    }

    reset_to_walk_left();
    sim.ground = false;
    tick();
    if (!is_fall(sim)) {
        std::cerr << "[GrhTB] dut_131 failed(long_fall_l_start)\n";
        return EXIT_FAILURE;
    }
    for (int i = 0; i < 30; ++i) {
        tick();
        if (!is_fall(sim)) {
            std::cerr << "[GrhTB] dut_131 failed(long_fall_l_loop)\n";
            return EXIT_FAILURE;
        }
    }
    sim.ground = true;
    tick();
    if (!is_splatter(sim)) {
        std::cerr << "[GrhTB] dut_131 failed(splatter_from_fall_l)\n";
        return EXIT_FAILURE;
    }

    reset_to_walk_left();
    sim.bump_left = true;
    tick();
    sim.bump_left = false;
    if (!is_walk_right(sim)) {
        std::cerr << "[GrhTB] dut_131 failed(walk_r_start)\n";
        return EXIT_FAILURE;
    }
    sim.ground = false;
    tick();
    if (!is_fall(sim)) {
        std::cerr << "[GrhTB] dut_131 failed(fall_r_start)\n";
        return EXIT_FAILURE;
    }
    sim.ground = true;
    tick();
    if (!is_walk_right(sim)) {
        std::cerr << "[GrhTB] dut_131 failed(fall_r_to_walk_r)\n";
        return EXIT_FAILURE;
    }

    reset_to_walk_left();
    sim.bump_left = true;
    tick();
    sim.bump_left = false;
    sim.ground = false;
    tick();
    if (!is_fall(sim)) {
        std::cerr << "[GrhTB] dut_131 failed(long_fall_r_start)\n";
        return EXIT_FAILURE;
    }
    for (int i = 0; i < 30; ++i) {
        tick();
        if (!is_fall(sim)) {
            std::cerr << "[GrhTB] dut_131 failed(long_fall_r_loop)\n";
            return EXIT_FAILURE;
        }
    }
    sim.ground = true;
    tick();
    if (!is_splatter(sim)) {
        std::cerr << "[GrhTB] dut_131 failed(splatter_from_fall_r)\n";
        return EXIT_FAILURE;
    }

    reset_to_walk_left();
    sim.bump_left = true;
    tick();
    sim.bump_left = false;
    sim.dig = true;
    sim.ground = true;
    tick();
    if (!is_dig(sim)) {
        std::cerr << "[GrhTB] dut_131 failed(dig_r)\n";
        return EXIT_FAILURE;
    }
    for (int i = 0; i < 3; ++i) {
        tick();
        if (!is_dig(sim)) {
            std::cerr << "[GrhTB] dut_131 failed(dig_r_hold)\n";
            return EXIT_FAILURE;
        }
    }
    sim.ground = false;
    tick();
    if (!is_fall(sim)) {
        std::cerr << "[GrhTB] dut_131 failed(fall_r_from_dig_r)\n";
        return EXIT_FAILURE;
    }
    for (int i = 0; i < 150; ++i) {
        tick();
        if (!is_fall(sim)) {
            std::cerr << "[GrhTB] dut_131 failed(long_count_toggle_fall)\n";
            return EXIT_FAILURE;
        }
    }
    sim.ground = true;
    sim.dig = false;
    tick();
    if (!is_splatter(sim)) {
        std::cerr << "[GrhTB] dut_131 failed(final_splatter)\n";
        return EXIT_FAILURE;
    }

    std::cout << "[GrhTB] dut_131 passed: extended Lemmings with full coverage paths\n";
    return EXIT_SUCCESS;
}
