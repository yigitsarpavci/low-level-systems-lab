# Low-Level Systems & Computer Architecture Lab

![Assembly](https://img.shields.io/badge/ISA-x86--64%20%7C%20RISC--V%20(RV32I)-red)
![C](https://img.shields.io/badge/Language-C11-00599C?logo=c&logoColor=white)
![Linux](https://img.shields.io/badge/Platform-Linux%20Kernel%20Syscalls-FCC624?logo=linux&logoColor=black)
![License](https://img.shields.io/badge/License-MIT-blue.svg)

A laboratory of low-level systems programming implementations, bare-metal hardware simulators, and process interpreters across **x86-64** and **RISC-V (RV32I)** architectures. Focuses on zero-dependency libc bypasses, raw Linux kernel syscalls, custom linker scripts, and modular command interpreters.

---

## Lab Modules Overview

| Module | Architecture / Language | Execution Target | Core Concepts |
|---|---|---|---|
| **[01. Zero-Libc x86-64 CPU Scheduler](./01-x86-64-cpu-scheduler)** | x86-64 GNU Assembly | Linux Kernel (Direct Syscalls) | FCFS, SJF, SRTF, Priority First, Round Robin |
| **[02. Bare-Metal RISC-V Navigator](./02-bare-metal-riscv-simulator)** | RISC-V Assembly (RV32I) | Spike ISA Simulator (Bare-Metal) | Custom Linker Script (`.ld`), HTIF Driver, ABI Stack Framing |
| **[03. Unix Command Interpreter](./03-unix-command-interpreter)** | C11, POSIX Shell | Linux / Unix | Recursive Descent Parser, Grammar Backtracking, `fork`/`exec` |

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
- **Internals:** Manual ASCII-to-integer conversion, buffer-managed process control blocks (PCBs), register preservation across context loops, verified by an automated Python test harness across 53 revisions.

### 2. Bare-Metal RISC-V Navigator (`02-bare-metal-riscv-simulator`)
- **Philosophy:** True bare-metal RV32I flight pathfinding execution with no operating system, BIOS, or standard libraries.
- **Memory Architecture:** Custom linker scripts (`.ld`) configuring precise address spaces:
  - `.text` ROM mapped to `0x80000000`
  - `.data` / `.bss` RAM mapped to `0x80010000`
- **Driver Layer:** Custom Host-Target Interface (HTIF) console driver communicating directly with the Spike ISA simulator (`tohost`/`fromhost`).
- **Algorithm:** Greedy graph routing under strict fuel constraints, manual ABI stack framing, and instruction-efficient arithmetic without hardware M-extension.

### 3. Unix Command Interpreter (`03-unix-command-interpreter`)
- **Philosophy:** A robust C11 Unix shell with a clean modular separation between tokenizer, recursive descent parser, state management, and kernel execution.
- **Grammar & Parsing:** Custom parser handling command sequences, token lookaheads, and grammar backtracking for ambiguous syntax constructs.
- **Process Management:** POSIX process lifecycles via `fork()`, `execvp()`, I/O redirection (`dup2`), and status evaluation (`waitpid`).

---

## Build & Run

```bash
# Clone the repository
git clone https://github.com/yigitsarpavci/low-level-systems-lab.git
cd low-level-systems-lab

# 1. Build and run x86-64 CPU Scheduler (Linux x86-64)
cd 01-x86-64-cpu-scheduler
make
./schedsim input.txt output.txt

# 2. Build and run RISC-V Navigator (Spike Simulator)
cd ../02-bare-metal-riscv-simulator
riscv64-unknown-elf-as -march=rv32i -mabi=ilp32 flight.s -o flight.o
riscv64-unknown-elf-ld -T link.ld -m elf32lriscv flight.o -o flight.elf
spike --isa=rv32i flight.elf

# 3. Build and run Unix Command Interpreter
cd ../03-unix-command-interpreter
make
./spireslayer
```

---

## Author

**Yiğit Sarp Avcı**  
Computer Engineering Student @ Boğaziçi University  
[GitHub Profile](https://github.com/yigitsarpavci) · [LinkedIn](https://www.linkedin.com/in/yigitsarpavci)
