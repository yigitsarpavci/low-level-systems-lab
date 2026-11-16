# RISC-V Cache & Memory Hierarchy Analysis (CORE-V Wally RV32GC)

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Architecture: RISC-V](https://img.shields.io/badge/Architecture-RISC--V%20RV32GC-orange.svg)](#default-rv32gc-cache-parameters)
[![RTL: CORE-V Wally](https://img.shields.io/badge/RTL-CORE--V%20Wally%20Caches-green.svg)](#l2-cache-architecture--rtl-design-choices)
[![Course: CMPE 344](https://img.shields.io/badge/Course-CMPE%20344%20Boğaziçi-purple.svg)](#overview)

Microarchitectural analysis of the Harvard L1 Instruction (`I$`) and Data (`D$`) cache subsystem, write-back coherence mechanisms, and unified L2 cache hierarchy design on the **CORE-V Wally (`rv32gc`)** processor. Developed as part of the **CMPE 344 (Computer Organization)** curriculum at **Boğaziçi University**.

---

## Table of Contents
- [Overview](#overview)
- [Default RV32GC Cache Parameters](#default-rv32gc-cache-parameters)
- [Cache Capacity & Line Fill Analysis](#cache-capacity--line-fill-analysis)
- [Hit/Miss Resolution & Write-Back Consistency](#hitmiss-resolution--write-back-consistency)
- [L2 Cache Architecture & RTL Design Choices](#l2-cache-architecture--rtl-design-choices)
- [Project Structure](#project-structure)
- [Full Technical Report](#full-technical-report)
- [License](#license)
- [Related Projects](#related-projects)

---

## Overview

CORE-V Wally implements a split **Harvard L1 cache architecture**: the **Instruction Fetch Unit (`src/ifu/ifu.sv`)** accesses a read-only 4-way set-associative **L1 Instruction Cache (`I$`)**, while the **Load/Store Unit (`src/lsu/lsu.sv`)** accesses a write-back, write-allocate 4-way set-associative **L1 Data Cache (`D$`)**. Both L1 caches interface with the System-on-Chip `uncore` over an AMBA 3 AHB-Lite bus arbitrated by the **External Bus Unit (`src/ebu/ebu.sv`)**.

---

## Default RV32GC Cache Parameters

Defined in `/opt/openhwgroup/cvw/config/rv32gc/config.vh` (lines 125–131):

| Line | Parameter | RTL Literal | Default Value | Description |
| :---: | :--- | :--- | :--- | :--- |
| 125 | `DCACHE_NUMWAYS` | `32'd4` | 4 ways | Associativity of the L1 D\$ cache |
| 126 | `DCACHE_WAYSIZEINBYTES` | `32'd4096` | 4096 B (4 KB) | Capacity of a single way in the L1 D\$ cache |
| 127 | `DCACHE_LINELENINBITS` | `32'd512` | 512 bits (64 B) | Cache line (block) size for the L1 D\$ cache |
| 128 | `ICACHE_NUMWAYS` | `32'd4` | 4 ways | Associativity of the L1 I\$ cache |
| 129 | `ICACHE_WAYSIZEINBYTES` | `32'd4096` | 4096 B (4 KB) | Capacity of a single way in the L1 I\$ cache |
| 130 | `ICACHE_LINELENINBITS` | `32'd512` | 512 bits (64 B) | Cache line (block) size for the L1 I\$ cache |
| 131 | `CACHE_SRAMLEN` | `32'd128` | 128 | Number of rows/width in the cache data SRAM macros |

---

## Cache Capacity & Line Fill Analysis

- **L1 I\$ and D\$ Total Capacity:**
  $$\text{I\$ Size} = \text{D\$ Size} = 4\text{ ways} \times 4096\text{ B/way} = \mathbf{16{,}384\text{ B} = 16\text{ KB}}$$
  Each way contains $\text{NUMSETS} = 4096 / 64 = 64\text{ sets}$ ($256$ cache lines of $64\text{ B}$ across all 4 ways).
- **Instructions Updated per I\$ Fill:**
  $$\frac{512\text{ bits/line}}{32\text{ bits/instruction}} = \mathbf{16\text{ standard (32-bit) RISC-V instructions}}$$
  *(With the `rv32gc` Compressed `C` extension enabled, up to $512 / 16 = 32$ compressed 16-bit instructions fit in one cache line).*
- **32-Bit Words Brought per D\$ Fill:**
  $$\frac{512\text{ bits/line}}{32\text{ bits/word}} = \mathbf{16\text{ words (32-bit)}}$$
  Transferred via a 16-beat AHB burst (`BEATSPERLINE = 512 / 32 = 16`) in `ahbcacheinterface.sv` into the 512-bit `FetchBuffer`.

---

## Hit/Miss Resolution & Write-Back Consistency

- **Address Partitioning (`PA_BITS = 34` in `Sv32`):**
  - **Offset (`[5:0]`, 6 bits):** Selects the byte/word within the 64-byte line.
  - **Set Index (`[11:6]`, 6 bits):** Indexes 1 of 64 sets across all 4 ways in parallel.
  - **Tag (`[33:12]`, 22 bits):** Compared against `ReadTag` in each way (`src/cache/cacheway.sv`).
- **Hit vs. Miss (`src/cache/cacheway.sv`, `src/cache/cachefsm.sv`):**
  - **Cache Hit:** Occurs when $\texttt{ValidWay}[w] == 1 \land \texttt{ReadTag}[w] == \texttt{PAdr}[33:12]$ for any way $w$, completing in 1 cycle in `STATE_ACCESS` (`CacheStall = 0`).
  - **Cache Miss:** Occurs when all ways in the indexed set are either invalid ($\texttt{ValidWay} == 0$, compulsory miss) or hold mismatched tags (conflict/capacity miss), stalling the pipeline (`CacheStall = 1`) while `cacheLRU.sv` selects a replacement victim.
- **Write-Back vs. Write-Through:**
  - Wally's D\$ implements **write-back with write-allocate**: store hits update only the L1 D\$ SRAM and set `DirtyBits = 1` (`SetDirty = 1`), deferring main-memory writes until a dirty victim line is evicted (`STATE_WRITEBACK` before `STATE_FETCH`) or an explicit flush (`FlushCache`, `cbo.flush`, `cbo.clean`) occurs.

---

## L2 Cache Architecture & RTL Design Choices

1. **Unified L2 Topology & Interconnect Placement (`wallypipelinedcore.sv`, `ebu.sv`):** Shared unified L2 (`READ_ONLY_CACHE = 0`) placed either as a 512-bit wide cache-bus arbiter before a single `ahbcacheinterface` or on the AHB-Lite bus between `ebu.sv` and `uncore.sv`.
2. **Configuration & SRAM Banking (`config.vh`, `cacheway.sv`, `cacheLRU.sv`):** Adding `L2CACHE_SUPPORTED`, `L2CACHE_NUMWAYS` (8–16 ways), `L2CACHE_WAYSIZEINBYTES` (16–32 KB/way), and `L2CACHE_LINELENINBITS` (512 bits).
3. **PIPT vs. L1 VIPT Indexing (`src/cache/cache.sv`):** While L1 uses 12-bit untranslated `NextSet[11:0]` (fitting within the 4 KB `Sv32` page offset), L2 ways exceed 4 KB (`SETTOP > 12`) and must index sets strictly from the physical address `PAdr[SETTOP-1:OFFSETLEN]` (**PIPT**).
4. **MMIO Bypass & HPTW (`ifu.sv`, `lsu.sv`, `pmachecker.sv`):** Uncached MMIO requests (`~CacheableM`, `BusRW`, `BusAtomic`, `BusCMOZero`) bypass L2, while Hardware Page Table Walker (`SelHPTW`) reads are cached in L2.
5. **Inclusion Policy & `fence.i` / `Zicbom` Coherence (`cachefsm.sv`):** Write-back, non-inclusive (NINE) vs. inclusive eviction back-invalidation, and forwarding `CMOpM[3:0]` operations through L2.
6. **Multi-Level Stall & CSR Counters (`hazard.sv`, `csrc.sv`):** Propagating L2 miss stalls through `IFUStallF`/`LSUStallM` and wiring `L2CacheAccess`/`L2CacheMiss` to `csrc.sv`.

---

## Project Structure

```text
06-riscv-cache-hierarchy-analysis/
├── LICENSE                 # MIT License
├── Makefile                # LaTeX report build automation
├── README.md               # Architectural overview and parameter reference
└── report/
    ├── report.pdf          # 6-page technical report
    └── report.tex          # LaTeX source (Charter + Inconsolata)
```

---

## Full Technical Report

The complete 6-page technical report with step-by-step derivations, write-policy comparisons, and CORE-V Wally RTL code references is available at [`report/report.pdf`](report/report.pdf).

---

## License

This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for details.

---

## Related Projects

- **[RISC-V-Pipeline-Hazard-Analysis](../05-riscv-pipeline-hazard-analysis)**: Cycle-by-cycle 5-stage RISC-V pipeline waveform and hazard analysis using Verilator and GTKWave.
- **[CoreMark-Performance-Profiling](../04-coremark-performance-profiling)**: Empirical EEMBC CoreMark v1.0 compiler optimization and RISC-V ISA extension benchmarking suite.
- **[Solar-Escape-RISCV-Navigator](../02-bare-metal-riscv-simulator)**: Bare-metal RISC-V (RV32I) assembly simulation for fuel-constrained westward autonomous navigation on Spike.
- **[SchedSim-CPU-Scheduler](../01-x86-64-cpu-scheduler)**: Zero-libc CPU scheduling simulator written in pure x86-64 GNU Assembly using direct Linux kernel syscalls.

---

*Developed by Yiğit Sarp Avcı*
