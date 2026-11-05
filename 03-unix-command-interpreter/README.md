# Unix Command Interpreter (Spireslayer)

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Language: C11](https://img.shields.io/badge/Language-C11-00599C?logo=c&logoColor=white)](#tech-stack)
[![Platform: Linux / POSIX](https://img.shields.io/badge/Platform-Linux%20%2F%20POSIX-FCC624?logo=linux&logoColor=black)](#build-and-run)
[![Course: CMPE 230](https://img.shields.io/badge/Course-CMPE%20230%20Boğaziçi-purple.svg)](#unix-command-interpreter-spireslayer)

A modular C11 command interpreter featuring a custom recursive descent parser, lookahead token stream, and grammar backtracking engine. Developed for the **CMPE 230 (Systems Programming)** curriculum at **Boğaziçi University**.

---

## Tech Stack

- **Language:** C11 (`-std=c11 -Wall -Wextra -pedantic`)
- **Build System:** GNU Make
- **Verification:** Python automated grading pipeline (`test/grader.py` & `test/checker.py`)
- **CI:** GitHub Actions

---

## Key Features

- **Recursive Descent Parser:** Custom grammar engine interpreting complex natural-language-like command sequences.
- **Grammar Backtracking:** Token lookahead and state rollback for disambiguating compound syntax constructs.
- **Clean Modularity:** Strict separation between lexical tokenization, syntactic parsing (`parser.c`), entity state management (`state.c`), and execution (`main.c`).
- **Automated Verification:** Validated against 7 comprehensive input/output test suites (`test-cases/`).

---

## Project Structure

```text
03-unix-command-interpreter/
├── main.c              # Entry point and main command loop
├── parser.c / parser.h # Recursive descent parser with backtracking
├── state.c / state.h   # Entity and state transition engine
├── spireslayer.h       # Shared data structures and prototypes
├── Makefile            # Build configuration
├── LICENSE             # MIT License
├── report.pdf          # Technical design and grammar specification report
├── test-cases/         # Reference input/output test pairs (01–07)
└── test/               # Automated grading scripts (checker.py, grader.py)
```

---

## Build and Run

### Prerequisites
- GCC or Clang (with C11 support)
- GNU Make
- Python 3.8+ (for running the test suite)

### Compile & Execute
```bash
git clone https://github.com/yigitsarpavci/low-level-systems-lab.git
cd low-level-systems-lab/03-unix-command-interpreter
make
./spireslayer < test-cases/input01.txt
```

---

## Testing

Run the automated grader against all test cases:

```bash
python3 test/grader.py ./spireslayer test-cases
```

---

## License

This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for details.

---

## Related Projects

- **[SchedSim-CPU-Scheduler](../01-x86-64-cpu-scheduler)**: Zero-libc CPU scheduling simulator written in pure x86-64 GNU Assembly using direct Linux kernel syscalls.
- **[Solar-Escape-RISCV-Navigator](../02-bare-metal-riscv-simulator)**: Bare-metal RISC-V (RV32I) assembly simulation for fuel-constrained westward autonomous navigation on Spike.
- **[CoreMark-Performance-Profiling](../04-coremark-performance-profiling)**: Empirical EEMBC CoreMark v1.0 compiler optimization and RISC-V ISA extension benchmarking suite.
- **[RISC-V-Pipeline-Hazard-Analysis](../05-riscv-pipeline-hazard-analysis)**: Cycle-accurate CORE-V Wally (`rv32i`) 5-stage RTL pipeline and hazard verification using Verilator and GTKWave.

---

*Developed by Yiğit Sarp Avcı*
