# x86-64 CPU Scheduler (SchedSim)

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Architecture: x86-64](https://img.shields.io/badge/Architecture-x86--64%20GNU%20Assembly-red.svg)](#key-technical-highlights)
[![Platform: Linux Syscalls](https://img.shields.io/badge/Platform-Linux%20Kernel%20Syscalls-FCC624?logo=linux&logoColor=black)](#key-technical-highlights)
[![Course: CMPE 230](https://img.shields.io/badge/Course-CMPE%20230%20Boğaziçi-purple.svg)](#x86-64-cpu-scheduler-schedsim)

A zero-libc CPU process scheduling simulator written entirely in pure **x86-64 GNU Assembly** for Linux. It interacts directly with the Linux kernel via raw system calls (`sys_read`, `sys_write`, `sys_open`, `sys_exit`) with zero standard C library (`libc`) dependencies. Built as part of the **CMPE 230 (Systems Programming)** curriculum at **Boğaziçi University**.

---

## Key Technical Highlights

- **Zero-Libc Architecture:** Pure x86-64 assembly interacting directly with the Linux kernel (`syscall` ABI) without linking `libc`.
- **Custom Low-Level Parsing:** Built-in ASCII-to-integer conversion, dynamic buffer management, and burst-0 process filtering.
- **Process Control Blocks (PCB):** Tracks arrival times, burst durations, remaining execution times, and priorities directly in CPU registers and `.bss` memory arrays.
- **Automated Validation:** End-to-end verification against an automated Python grading harness (`test/grader.py`).

---

## Supported Scheduling Algorithms

| Algorithm | Code | Preemption | Tie-Breaking & Dispatch Policy |
| :--- | :--- | :--- | :--- |
| **First-Come, First-Served** | `FCFS` | Non-Preemptive | Arrival time, then lowest process ID (`A`–`J`) |
| **Shortest Job First** | `SJF` | Non-Preemptive | Shortest total burst time, then arrival time |
| **Shortest Remaining Time First** | `SRTF` | Preemptive | Shortest remaining burst time on every tick |
| **Priority First** | `PF` | Preemptive | Lowest numerical priority value (highest urgency) |
| **Round Robin** | `RR` | Preemptive (Quantum) | FIFO circular ready queue with configurable time quantum |

---

## Project Structure

```text
01-x86-64-cpu-scheduler/
├── Makefile                   # GNU as/ld build and automated test targets
├── LICENSE                    # MIT License
├── README.md                  # Architecture and usage documentation
├── OPTIMIZATIONS.md           # Low-level assembly optimization analysis
├── diagram.jpg                # Control-flow and state transition diagram
├── report.pdf                 # Compiled technical design report
├── report.tex                 # Full LaTeX report source
├── src/
│   └── schedsim.s             # Pure x86-64 GNU Assembly implementation
├── test/
│   ├── checker.py             # Output trace comparator
│   └── grader.py              # Automated test suite runner
└── test-cases/                # Reference input/output scheduling traces
```

---

## Build and Run

```bash
# Clone the systems lab repository
git clone https://github.com/yigitsarpavci/low-level-systems-lab.git
cd low-level-systems-lab/01-x86-64-cpu-scheduler

# Assemble and link (Linux x86-64)
make
./schedsim test-cases/input_00.txt output.txt
```

---

## Testing

Run the full automated grading suite:

```bash
make testgrade
```

Or invoke the Python grader directly after generating test outputs:

```bash
make testcases
python3 test/grader.py ./schedsim test-cases
```

---

## License

This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for details.

---

## Related Projects

- **[Solar-Escape-RISCV-Navigator](../02-bare-metal-riscv-simulator)**: Bare-metal RISC-V (RV32I) assembly simulation for fuel-constrained westward autonomous navigation on Spike.
- **[Unix-Command-Interpreter](../03-unix-command-interpreter)**: Modular C11 Unix shell featuring a recursive descent parser and grammar backtracking.
- **[CoreMark-Performance-Profiling](../04-coremark-performance-profiling)**: Empirical EEMBC CoreMark v1.0 compiler optimization and RISC-V ISA extension benchmarking suite.
- **[RISC-V-Pipeline-Hazard-Analysis](../05-riscv-pipeline-hazard-analysis)**: Cycle-accurate CORE-V Wally (`rv32i`) 5-stage RTL pipeline and hazard verification using Verilator and GTKWave.

---

*Developed by Yiğit Sarp Avcı*
