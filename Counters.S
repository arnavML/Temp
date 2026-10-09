.macro	PUSH reg
	addi sp,sp,-4
	sw \reg,0(sp)
.endm

.macro	POP reg
	lw	\reg,0(sp)
	addi	sp,sp,4
.endm


.text
.globl mcycle, minstret, mhpmcounter3, mhpmcounter4, mhpmcounter5, mhpmcounter6, mhpmcounter7, mhpmcounter8, mhpmcounter9, clear_counters


mcycle: /* reads the registers values and returns them as 64 bit unsigned long long*/
	csrr a0, mcycle
	csrr a1, mcycleh
    ret

minstret:
	csrr a0, minstret
	csrr a1, minstreth
	ret

mhpmcounter3:
	csrr a0, mhpmcounter3
	csrr a1, mhpmcounter3h
	ret

mhpmcounter4:
	csrr a0, mhpmcounter4
	csrr a1, mhpmcounter4h
	ret

mhpmcounter5:
	csrr a0, mhpmcounter5
	csrr a1, mhpmcounter5h
	ret
	
mhpmcounter6:
	csrr a0, mhpmcounter6
	csrr a1, mhpmcounter6h
	ret

mhpmcounter7:
	csrr a0, mhpmcounter7
	csrr a1, mhpmcounter7h
	ret

mhpmcounter8:
	csrr a0, mhpmcounter8
	csrr a1, mhpmcounter8h
	ret

mhpmcounter9:
	csrr a0, mhpmcounter9
	csrr a1, mhpmcounter9h
	ret

clear_counters: /* sets all registers to 0 */
    csrw mcycle,       zero
    csrw mcycleh,      zero

    csrw minstret,     zero
    csrw minstreth,    zero

    csrw mhpmcounter3, zero
    csrw mhpmcounter3h, zero

    csrw mhpmcounter4, zero
    csrw mhpmcounter4h, zero

    csrw mhpmcounter5, zero
    csrw mhpmcounter5h, zero

    csrw mhpmcounter6, zero
    csrw mhpmcounter6h, zero

    csrw mhpmcounter7, zero
    csrw mhpmcounter7h, zero

    csrw mhpmcounter8, zero
    csrw mhpmcounter8h, zero

    csrw mhpmcounter9, zero
    csrw mhpmcounter9h, zero

    ret