# ==============================================================================
# CORE-V Wally Pipelined Datapath & Hazard Verification (RV32I)
# ==============================================================================
# Course:       CMPE 344 - Computer Organization (Boğaziçi University)
# Author:       Yiğit Sarp Avcı
# Architecture: RISC-V RV32I (5-Stage Pipelined Datapath: F, D, E, M, W)
# Target:       CORE-V Wally RTL (Verilator Simulation + GTKWave VCD Trace)
# Memory Map:   ROM @ 0x80000000 (.text, HTIF), RAM @ 0x80010000 (.data)
#
# Scenario:
#   Bare-metal RV32I microarchitectural verification testbench evaluating
#   data forwarding and pipeline interlock mechanisms on CORE-V Wally:
#     1. Non-stalling RAW Hazard (W -> E Bypass): `lw x6, 0(x5)` separated by a
#        single `nop` from `add x8, x6, x6`, resolved with zero stall penalty via
#        ForwardAE = 2'b01 and ForwardBE = 2'b01 (`ResultW` bypass path).
#     2. Load-Use Data Hazard (Stall + Flush): `lw x7, 4(x5)` immediately
#        followed by `add x9, x7, x7`, triggering StallF=1, StallD=1, and
#        FlushE=1 for one cycle before W -> E forwarding completes execution.
# ==============================================================================

.globl _start

#.section .bss

.section .data
    arr: .word 1111, 2222, 3333, 4444, 5555, 6666
.section .text

_start:
    la x5, arr
    nop
    nop
    nop
    lw x6, 0(x5)
    nop
    add x8, x6, x6

################################################################################# 
# DO NOT EDIT ABOVE THIS LINE.

    lw x7, 4(x5)        # Load arr[1] (2222) from memory into x7
    add x9, x7, x7      # Immediately use x7 in the next instruction (load-use hazard)

# DO NOT EDIT BELOW THIS LINE.
################################################################################# 
    j END

END:    
    li   a7, 10       
    j .
