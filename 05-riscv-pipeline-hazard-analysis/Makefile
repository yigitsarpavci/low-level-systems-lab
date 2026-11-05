# ==============================================================================
# CORE-V Wally Pipelined Datapath & Hazard Verification Makefile
# ==============================================================================

TOOLCHAIN ?= riscv64-unknown-elf-
CC        := $(TOOLCHAIN)gcc
OBJDUMP   := $(TOOLCHAIN)objdump
WALLY_SIM ?= /opt/openhwgroup/cvw/sim/verilator

ARCH_FLAGS := -g -march=rv32i -mabi=ilp32 -nostdlib -I/opt/openhwgroup/cvw/addins/verilator
LDFLAGS    := -T linker.ld

SRC_DIR    := src
BUILD_DIR  := build
TARGET_ELF := $(BUILD_DIR)/hazard_analysis.elf
TARGET_DMP := $(BUILD_DIR)/hazard_analysis.objdump
SRCS       := $(SRC_DIR)/hazard_analysis.s

.PHONY: all clean dump simulate build-report docker-build docker-sim help

all: $(TARGET_ELF) $(TARGET_DMP)

help:
	@echo "Available targets:"
	@echo "  make all          - Assemble and link RV32I ELF binary + objdump disassembly"
	@echo "  make simulate     - Run Verilator RTL simulation on CORE-V Wally (generates testbench.vcd)"
	@echo "  make build-report - Compile report/report.tex via pdflatex"
	@echo "  make docker-build - Build ELF and objdump inside ghcr.io/bouncmpe/labs344 container"
	@echo "  make docker-sim   - Run full Verilator simulation inside ghcr.io/bouncmpe/labs344"
	@echo "  make clean        - Remove build and LaTeX intermediate artifacts"

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET_ELF): $(SRCS) linker.ld | $(BUILD_DIR)
	$(CC) $(ARCH_FLAGS) $(LDFLAGS) $< -o $@

$(TARGET_DMP): $(TARGET_ELF)
	$(OBJDUMP) -D $< > $@

simulate: $(TARGET_ELF)
	cd $(WALLY_SIM) && wally-batch-verilator --params "+memfile=$(CURDIR)/$(TARGET_ELF)" rv32i

build-report:
	cd report && (pdflatex -interaction=nonstopmode report.tex && pdflatex -interaction=nonstopmode report.tex || tectonic report.tex)
	rm -f report/*.aux report/*.log report/*.out report/*.synctex.gz report/*.toc

docker-build:
	docker run --rm --mount type=bind,source="$(CURDIR)",target=/workspace -w /workspace ghcr.io/bouncmpe/labs344:latest make all

docker-sim:
	docker run --rm --mount type=bind,source="$(CURDIR)",target=/workspace -w /workspace ghcr.io/bouncmpe/labs344:latest make simulate

clean:
	rm -rf $(BUILD_DIR)
	rm -f report/*.aux report/*.log report/*.out report/*.synctex.gz report/*.toc
