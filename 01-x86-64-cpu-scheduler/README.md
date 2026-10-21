# x86-64 CPU Scheduler (SchedSim)

SchedSim is a zero-libc CPU scheduling simulator written entirely in pure x86-64 GNU Assembly for Linux. It interacts directly with the Linux kernel via raw syscalls (`sys_read`, `sys_write`, `sys_open`, `sys_exit`) with zero standard library dependencies.

## Key Technical Highlights

- **Zero-Libc Architecture:** Pure assembly interacting directly with Linux kernel syscalls without `libc`.
- **Custom Low-Level Parsing:** Built-in ASCII-to-integer parsing and dynamic input buffer management.
- **Process Control Blocks (PCB):** Tracks arrival times, burst durations, remaining times, and priorities directly in CPU registers and stack buffers.
- **Automated Validation:** Test suite verified against custom Python automated grading harnesses.

## Supported Algorithms

- **FCFS:** First Come First Serve
- **SJF:** Shortest Job First
- **SRTF:** Shortest Remaining Time First
- **PF:** Priority First
- **RR:** Round Robin

Burst-0 processes are ignored during parsing, as clarified for this assignment.

## Tech Stack

- **Assembly:** x86-64 GNU Assembly
- **Toolchain:** GNU `as`, `ld`, Make
- **Testing:** Python Grader Test Harness

## Build and Run

```sh
make
./schedsim
```

## Test

Run the full local test flow:

```sh
make testgrade
```

`make grade` also runs the provided test cases before invoking the Python
grader.

The grader can also be invoked directly after generating outputs:

```sh
make testcases
python3 test/grader.py ./schedsim test-cases
```
