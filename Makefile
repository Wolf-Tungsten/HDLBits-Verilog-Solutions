SHELL := /bin/bash

VERILATOR ?= verilator
CXX ?= c++
PYTHON ?= python3
# -Wno-fatal: do not treat warnings as errors (needed for vector logical ops etc.)
VERILATOR_FLAGS ?= -Wall -Wno-DECLFILENAME --coverage -Wno-fatal
CXXFLAGS ?= -std=c++20 -O2
export CCACHE_DISABLE ?= 1
DUT ?= 001
TOP ?= top_module
PREFIX ?= Vdut_$(DUT)
MODEL := $(PREFIX)
GRHSIM_TOP ?= $(TOP)
GRHSIM_PREFIX := grhsim_$(GRHSIM_TOP)
GRHSIM_SCRIPT ?= ../../scripts/wolvrix_hdlbits_grhsim.py

BUILD_DIR := build
COVERAGE_ROOT := coverage
LIB_SRCS := $(wildcard dut/lib/*.v)
DUT_SRC := dut/dut_$(DUT).v
TB_SRC := tb/tb_$(DUT).cpp
BUILD_SUBDIR := $(BUILD_DIR)/tb_$(DUT)
BIN := $(BUILD_SUBDIR)/V$(TOP)
GRHTB_SRC := grhtb/grhtb_$(DUT).cpp
GRHSIM_BUILD_SUBDIR := $(BUILD_DIR)/grhtb_$(DUT)
GRHSIM_OUT_DIR := $(GRHSIM_BUILD_SUBDIR)/$(GRHSIM_PREFIX)
GRHSIM_LIB := $(GRHSIM_OUT_DIR)/lib$(GRHSIM_PREFIX).a
GRHSIM_BIN := $(GRHSIM_BUILD_SUBDIR)/$(GRHSIM_PREFIX)_tb
COV_DIR := $(COVERAGE_ROOT)/dut_$(DUT)
COV_DAT := $(COV_DIR)/coverage.dat
COV_INFO := $(COV_DIR)/coverage.info
COV_ANNOTATE_DIR := $(COV_DIR)/annotate

.PHONY: all run_tb run_grhtb clean coverage_report check_grhtb_id force_grhsim_emit

all: run_tb

$(BUILD_SUBDIR):
	@mkdir -p $@

$(GRHSIM_BUILD_SUBDIR):
	@mkdir -p $@

$(GRHSIM_OUT_DIR):
	@mkdir -p $@

$(BIN): $(DUT_SRC) $(TB_SRC) $(LIB_SRCS) | $(BUILD_SUBDIR)
	$(VERILATOR) $(VERILATOR_FLAGS) --cc $(DUT_SRC) $(LIB_SRCS) --exe ../../$(TB_SRC) \
		--top-module $(TOP) --prefix $(PREFIX) -o V$(TOP) -Mdir $(BUILD_SUBDIR)
	$(MAKE) -C $(BUILD_SUBDIR) -f $(MODEL).mk V$(TOP)

check_grhtb_id:
	@if [[ ! "$(DUT)" =~ ^[0-9]{3}$$ ]]; then \
		echo "DUT must be a three-digit number (e.g. DUT=001)"; \
		exit 1; \
	fi
	@test -f $(DUT_SRC) || { echo "Missing DUT source: $(DUT_SRC)"; exit 1; }
	@test -f $(GRHTB_SRC) || { echo "Missing GrhSIM testbench: $(GRHTB_SRC)"; exit 1; }

# Always re-emit GrhSIM code so the current py_install'ed wolvrix is reflected
# even when the DUT/TB/script paths themselves are unchanged.
force_grhsim_emit:

$(GRHSIM_LIB): force_grhsim_emit $(DUT_SRC) $(GRHTB_SRC) $(GRHSIM_SCRIPT) | check_grhtb_id $(GRHSIM_OUT_DIR)
	$(PYTHON) $(GRHSIM_SCRIPT) $(DUT) $(GRHSIM_OUT_DIR)
	$(MAKE) -C $(GRHSIM_OUT_DIR)
	@ACTUAL_LIB="$$(sed -n 's/^LIB := //p' $(GRHSIM_OUT_DIR)/Makefile)"; \
	if [ -n "$$ACTUAL_LIB" ] && [ "$$ACTUAL_LIB" != "lib$(GRHSIM_PREFIX).a" ] && [ -f "$(GRHSIM_OUT_DIR)/$$ACTUAL_LIB" ]; then \
		cp -f "$(GRHSIM_OUT_DIR)/$$ACTUAL_LIB" "$(GRHSIM_LIB)"; \
	fi

$(GRHSIM_BIN): $(GRHTB_SRC) $(GRHSIM_LIB) | $(GRHSIM_BUILD_SUBDIR)
	$(CXX) $(CXXFLAGS) -I$(GRHSIM_OUT_DIR) $(GRHTB_SRC) -L$(GRHSIM_OUT_DIR) -l$(GRHSIM_PREFIX) -o $(GRHSIM_BIN)

run_tb: $(BIN)
	@mkdir -p $(COV_DIR)
	@echo "[RUN] DUT=$(DUT)"
	VERILATOR_COV_FILE=$(COV_DAT) ./$(BIN)
	@test -f $(COV_DAT) || (echo "[ERROR] Coverage data missing for DUT $(DUT)" && exit 1)
	$(MAKE) coverage_report \
		COV_DAT=$(COV_DAT) \
		COV_INFO=$(COV_INFO) \
		COV_ANNOTATE_DIR=$(COV_ANNOTATE_DIR)

run_grhtb: $(GRHSIM_BIN)
	@echo "[RUN] DUT=$(DUT) GRHSIM"
	$(GRHSIM_BIN)

coverage_report:
	@if [ -z "$(COV_DAT)" ] || [ -z "$(COV_INFO)" ] || [ -z "$(COV_ANNOTATE_DIR)" ]; then \
		echo "[ERROR] coverage_report requires COV_DAT, COV_INFO, and COV_ANNOTATE_DIR"; \
		exit 1; \
	fi
	@mkdir -p $(COV_ANNOTATE_DIR)
	# Line coverage (LCOV format) and annotated sources
	verilator_coverage --write-info $(COV_INFO) $(COV_DAT)
	verilator_coverage --annotate-min 1 --annotate $(COV_ANNOTATE_DIR) $(COV_DAT)
	# Toggle coverage summary (per-bit points, bidirectional) and combined summary
	@awk -v dut=dut/dut_$(DUT).v 'BEGIN{ltot=0;lcov=0;tt=0;tc=0} FNR==NR&&/^DA:/{ltot++;split($$0,a,",");if(a[2]+0>0)lcov++;next} /^C /&&$$0~dut&&$$0~/v_toggle/{tt++;if($$NF+0>=2)tc++;next} END{combt=ltot+tt;combc=lcov+tc; printf("Toggle coverage (bidirectional) (%d/%d) %0.2f%%\n",tc,tt,(tt?100.0*tc/tt:0)); printf("Combined coverage (lines+toggles) (%d/%d) %0.2f%%\n",combc,combt,(combt?100.0*combc/combt:0))}' $(COV_INFO) $(COV_DAT)

clean:
	rm -rf $(BUILD_DIR) $(COVERAGE_ROOT) coverage.dat coverage.info coverage_annotate
