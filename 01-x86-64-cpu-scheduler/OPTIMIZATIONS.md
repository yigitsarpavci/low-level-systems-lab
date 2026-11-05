# SchedSim Assembly Optimization & Refactoring Report

**Target:** x86-64 GNU Assembly (Linux Kernel Syscalls)

This report outlines architectural optimizations and low-level refactoring strategies applicable to the `SchedSim` CPU scheduler implementation to reduce instruction footprint, eliminate branching overhead, and strengthen memory safety.

---

## 1. Code Redundancy & Modularity: Preprocessor Macros

- **Current Implementation:** Multi-digit ASCII-to-integer parsing is repeated across four separate input fields (burst time, arrival time, priority, and time quantum).
- **Optimization:** Encapsulate numeric accumulation inside an assembler macro (e.g., `.macro PARSE_INT reg`).
- **Architectural Benefit:**
  - Enforces the DRY (Don't Repeat Yourself) principle across register-heavy parsing loops.
  - Reduces manual label/jump errors and improves source maintainability.

## 2. O(1) Algorithm Dispatch: Jump Tables

- **Current Implementation:** The `parse_algorithm` routine evaluates a sequential chain of `cmp` and `je` instructions to dispatch the selected scheduling policy ($O(N)$ comparisons).
- **Optimization:** Replace conditional branch chains with an indexed 8-byte address jump table (`jmp *algo_table(,%rax,8)`).
- **Architectural Benefit:**
  - Guarantees $O(1)$ constant-time dispatch regardless of the number of scheduling policies.
  - Eliminates branch misprediction penalties during initialization.

## 3. Memory Safety: Output Buffer Boundary Checking

- **Current Implementation:** The `append_output` routine assumes the 4096-byte `.bss` output buffer (`output_buf`) is sufficient for the entire simulation trace.
- **Optimization:** Insert a bounds guard prior to buffer writes (`cmp $4096, %rcx; jge flush_or_error`).
- **Architectural Benefit:**
  - Prevents `.bss` buffer overflows and segmentation faults on long-running process traces.

## 4. Cache Locality: Process Control Block (PCB) Data Packing

- **Current Implementation:** Process attributes use uniform 8-byte quadword (`quad`) slots even for single-byte identifiers (`proc_id`) and bounded integer fields.
- **Optimization:** Pack PCB arrays using narrow data widths (`.byte` for IDs and priorities, `.word` for burst/arrival times).
- **Architectural Benefit:**
  - Shrinks the `.bss` memory footprint by up to 75%.
  - Significantly improves L1 data cache line utilization during inner-loop process table scans.

## 5. Call Overhead Elimination: Routine Inlining

- **Current Implementation:** Small utility routines such as `append_output` are invoked on every simulated time tick via `call`/`ret` instructions.
- **Optimization:** Inline tick-logging logic directly into the inner scheduling loops or expand via assembler macros.
- **Architectural Benefit:**
  - Eliminates repetitive stack pointer (`%rsp`) adjustments and return-address pipeline stalls across thousands of simulation ticks.

## 6. OS Interface Abstraction: Syscall Wrappers

- **Current Implementation:** Linux kernel syscall registers (`%rax`, `%rdi`, `%rsi`, `%rdx`) are populated inline before each `syscall` instruction.
- **Optimization:** Define structured `.macro WRITE_FD` and `.macro SYS_EXIT` wrappers.
- **Architectural Benefit:**
  - Isolates ABI register clobbering (`%rcx`, `%r11`) and highlights core scheduling control flow.

## 7. Bitwise Round-Robin Queue Wrapping

- **Current Implementation:** The Round-Robin ready queue wraps head and tail indices using explicit bounds comparisons and conditional jumps.
- **Optimization:** Size the circular queue to a power of two (e.g., 16 slots) and wrap indices via bitwise masking (`and $15, %reg`).
- **Architectural Benefit:**
  - Replaces conditional branch instructions with a single-cycle bitwise ALU operation.
