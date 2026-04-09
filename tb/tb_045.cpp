#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vdut_045.h"

int main(int argc, char **argv)
{
    Verilated::commandArgs(argc, argv);
    auto context = std::make_unique<VerilatedContext>();
    context->traceEverOn(false);
    auto dut = std::make_unique<Vdut_045>(context.get());

    for (int rep = 0; rep < 8; ++rep) {
        dut->eval();
        if (dut->out != 0) {
            std::cerr << "[TB] dut_045 failed: expected out=0, got " << static_cast<int>(dut->out) << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::cout << "[TB] dut_045 passed: constant zero output verified" << std::endl;

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}
