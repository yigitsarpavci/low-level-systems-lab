# Low-Level Systems & Computer Architecture Lab

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![ISA: x86-64 | RISC-V](https://img.shields.io/badge/ISA-x86--64%20%7C%20RISC--V%20(RV32I)-red)](#lab-modules-overview)
[![Simulation: Spike | Verilator](https://img.shields.io/badge/Simulation-Spike%20%7C%20Verilator-green)](#lab-modules-overview)
[![Language: C11](https://img.shields.io/badge/Language-C11-00599C?logo=c&logoColor=white)](#3-unix-command-interpreter-03-unix-command-interpreter)
[![Platform: Linux Syscalls](https://img.shields.io/badge/Platform-Linux%20Kernel%20Syscalls-FCC624?logo=linux&logoColor=black)](#1-zero-libc-x86-64-cpu-scheduler-01-x86-64-cpu-scheduler)

A laboratory of low-level systems programming implementations, bare-metal simulators, compiler optimization benchmarks, and processor pipeline analyses across **x86-64**, **ARM64**, and **RISC-V (RV32I)** architectures. Focuses on zero-dependency `libc` bypasses, raw Linux kernel syscalls, custom linker scripts, recursive descent command interpreters, and 5-stage pipeline hazard resolution.

---

## Lab Modules Overview

| Module | Architecture / Language | Execution Target | Core Concepts |
|---|---|---|---|
| **[01. Zero-Libc x86-64 CPU Scheduler](./01-x86-64-cpu-scheduler)** | x86-64 GNU Assembly | Linux Kernel (Direct Syscalls) | FCFS, SJF, SRTF, Priority First, Round Robin |
| **[02. Bare-Metal RISC-V Navigator](./02-bare-metal-riscv-simulator)** | RISC-V Assembly (RV32I) | Spike ISA Simulator (Bare-Metal) | Custom Linker Script (`.ld`), HTIF Driver, ABI Stack Framing |
| **[03. Unix Command Interpreter](./03-unix-command-interpreter)** | C11, POSIX Shell | Linux / Unix | Recursive Descent Parser, Grammar Backtracking, State Execution |
| **[04. CoreMark Performance Profiling](./04-coremark-performance-profiling)** | C11, Clang/LLVM, GCC | Apple Silicon (M2 ARM64) & RISC-V | Compiler Flags (`-O0` to `-Ofast`), ISA Extensions (M, C, GC), ILP |
| **[05. RISC-V Pipeline & Hazard Analysis](./05-riscv-pipeline-hazard-analysis)** | RISC-V Assembly (RV32I), SystemVerilog | Verilator & GTKWave | 5-Stage Pipeline, Data Forwarding, Load-Use Stalls, Branch Flushing |

---

## Detailed Lab Specifications

### 1. Zero-Libc x86-64 CPU Scheduler (`01-x86-64-cpu-scheduler`)
- **Philosophy:** Completely eliminates standard C runtime (`libc`) dependencies. Interacts directly with the Linux kernel through the `syscall` instruction.
- **Syscall Surface:** `sys_read` (0), `sys_write` (1), `sys_open` (2), `sys_exit` (60).
- **Scheduling Engines:**
  - First-Come First-Served (FCFS)
  - Shortest Job First (SJF) & Shortest Remaining Time First (SRTF)
  - Preemptive / Non-Preemptive Priority First (PF)
  - Quantum-driven Round Robin (RR)
- **Internals:** Manual ASCII-to-integer conversion, buffer-managed process control blocks (PCBs), register preservation across context loops, verified by an automated Python test harness.

### 2. Bare-Metal RISC-V Navigator (`02-bare-metal-riscv-simulator`)
- **Philosophy:** True bare-metal RV32I flight pathfinding execution with no operating system, BIOS, or standard libraries.
- **Memory Architecture:** Custom linker scripts (`.ld`) configuring precise address spaces:
  - `.text` ROM mapped to `0x80000000`
  - `.data` / `.bss` RAM mapped to `0x80010000`
- **Driver Layer:** Custom Host-Target Interface (HTIF) console driver communicating directly with the Spike ISA simulator (`tohost`/`fromhost`).
- **Algorithm:** Greedy graph routing under strict fuel constraints, manual ABI stack framing, and instruction-efficient arithmetic without hardware M-extension.

### 3. Unix Command Interpreter (`03-unix-command-interpreter`)
- **Philosophy:** A robust C11 command interpreter with a clean modular separation between tokenizer, recursive descent parser, state management, and execution.
- **Grammar & Parsing:** Custom parser handling command sequences, token lookaheads, and grammar backtracking for ambiguous syntax constructs.
- **Verification:** Strict `-std=c11 -Wall -Wextra -pedantic` compliance validated against an automated Python grading harness.

### 4. CoreMark Performance Profiling (`04-coremark-performance-profiling`)
- **Philosophy:** Rigorous empirical evaluation of EEMBC CoreMark v1.0 benchmark throughput across compiler optimization flags, instruction-level parallelism (ILP), and hardware execution architectures.
- **Architectures Profiled:** Apple Silicon (M2 ARM64, Clang/LLVM) vs. RISC-V (CORE-V Wally, GCC).
- **Optimization Analysis:** Evaluated compiler transformations from `-O0` to `-Ofast` (`-march=native`), tracking Iterations/sec, instruction count reduction, vectorization, and cache effects.
- **ISA Extension Benchmarking:** Comparative analysis of hardware vs. software multiply/divide and code density across RISC-V extensions (RV32I, RV32IM, RV32IMC, RV32GC, RV64I), automated via Python test harnesses.

### 5. RISC-V Pipeline & Hazard Analysis (`05-riscv-pipeline-hazard-analysis`)
- **Philosophy:** Cycle-by-cycle waveform analysis of a 5-stage pipelined RISC-V (`rv32i`) processor (Fetch, Decode, Execute, Memory, Writeback) using **Verilator** and **GTKWave**.
- **Control Hazards:** Traces static predict-not-taken branch evaluation in the Execute stage and two-cycle pipeline flushing of wrong-path instructions when a branch is taken.
- **Data Forwarding:** Verifies zero-stall read-after-write (RAW) hazard resolution by forwarding results from the Writeback stage directly back to the ALU inputs.
- **Load-Use Stalls:** Custom bare-metal RV32I assembly testbench (`lw` immediately followed by a dependent `add`) demonstrating a single-cycle pipeline stall and bubble insertion before forwarding the loaded word.

---

## Build & Run

```bash
# Clone the repository
git clone https://github.com/yigitsarpavci/low-level-systems-lab.git
cd low-level-systems-lab

# 1. Build and test x86-64 CPU Scheduler (Linux x86-64)
cd 01-x86-64-cpu-scheduler
make
make testgrade

# 2. Build and run RISC-V Navigator (Spike Simulator)
cd ../02-bare-metal-riscv-simulator
make
make run

# 3. Build and test Unix Command Interpreter
cd ../03-unix-command-interpreter
make
python3 test/grader.py ./spireslayer test-cases

# 4. Run CoreMark Profiling Benchmarks
cd ../04-coremark-performance-profiling
make benchmark

# 5. Assemble & Simulate RV32I Pipeline Hazards on CORE-V Wally (Verilator)
cd ../05-riscv-pipeline-hazard-analysis
make docker-build
make docker-sim
```

---

## License

This repository is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for details.

---

## Author

**Yiğit Sarp Avcı**  
Computer Engineering Student @ Boğaziçi University  
[GitHub Profile](https://github.com/yigitsarpavci) · [LinkedIn](https://www.linkedin.com/in/yigitsarpavci)
