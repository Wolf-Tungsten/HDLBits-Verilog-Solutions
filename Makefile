VERILATOR ?= verilator
VERILATOR_FLAGS ?= -Wall -Wno-DECLFILENAME
VERILATOR_COVERAGE ?= 1

ifeq ($(VERILATOR_COVERAGE),1)
VERILATOR_FLAGS += --coverage
endif

BUILD_DIR := build
TB_001_DIR := $(BUILD_DIR)/tb_001
TB_001_BIN := $(TB_001_DIR)/Vtop_module
COVERAGE_ROOT := coverage
TB_001_COVERAGE_DIR := $(COVERAGE_ROOT)/dut_001
TB_001_COVERAGE_DAT := $(TB_001_COVERAGE_DIR)/coverage.dat
TB_001_COVERAGE_INFO := $(TB_001_COVERAGE_DIR)/coverage.info
TB_001_COVERAGE_ANNOTATE_DIR := $(TB_001_COVERAGE_DIR)/annotate

.PHONY: all run_tb_001 clean coverage_report

all: run_tb_001

$(TB_001_DIR):
	@mkdir -p $@

$(TB_001_BIN): dut/dut_001.v tb/tb_001.cpp | $(TB_001_DIR)
	$(VERILATOR) $(VERILATOR_FLAGS) --cc dut/dut_001.v --exe tb/tb_001.cpp -o Vtop_module -Mdir $(TB_001_DIR)
	$(MAKE) -C $(TB_001_DIR) -f Vdut_001.mk Vtop_module

run_tb_001: $(TB_001_BIN)
	@mkdir -p $(TB_001_COVERAGE_DIR)
	VERILATOR_COV_FILE=$(TB_001_COVERAGE_DAT) ./$(TB_001_BIN)
ifeq ($(VERILATOR_COVERAGE),1)
	@if [ -f $(TB_001_COVERAGE_DAT) ]; then \
		$(MAKE) coverage_report \
			COV_DAT=$(TB_001_COVERAGE_DAT) \
			COV_INFO=$(TB_001_COVERAGE_INFO) \
			COV_ANNOTATE_DIR=$(TB_001_COVERAGE_ANNOTATE_DIR); \
	else \
		echo "[WARN] Coverage data missing; skipping report."; \
	fi
endif

coverage_report:
	@if [ -z "$(COV_DAT)" ] || [ -z "$(COV_INFO)" ] || [ -z "$(COV_ANNOTATE_DIR)" ]; then \
		echo "[ERROR] coverage_report requires COV_DAT, COV_INFO, and COV_ANNOTATE_DIR"; \
		exit 1; \
	fi
	@mkdir -p $(COV_ANNOTATE_DIR)
	verilator_coverage --write-info $(COV_INFO) $(COV_DAT)
	verilator_coverage --annotate $(COV_ANNOTATE_DIR) $(COV_DAT)

clean:
	rm -rf $(BUILD_DIR) $(COVERAGE_ROOT) coverage.dat coverage.info coverage_annotate
