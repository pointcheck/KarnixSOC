.global crtStart
.global main
.global irqCallback
.global uart_write

    .section	.start_jump,"ax",@progbits
crtStart:
  csrc mstatus, 0x8 // Disable interrupts

  // take address of trap_entry relative to PC and store it to mtvec
  la t1,trap_entry
  csrw mtvec, t1

  la t1,crtInit // takes address relative to PC
  jr t1 // jump crtInit
  nop

.section .text

.global  trap_entry
.align 5
trap_entry:
  sw x1,  - 1*4(sp)
  sw x5,  - 2*4(sp)
  sw x6,  - 3*4(sp)
  sw x7,  - 4*4(sp)
  sw x10, - 5*4(sp)
  sw x11, - 6*4(sp)
  sw x12, - 7*4(sp)
  sw x13, - 8*4(sp)
  sw x14, - 9*4(sp)
  sw x15, -10*4(sp)
  sw x16, -11*4(sp)
  sw x17, -12*4(sp)
  sw x28, -13*4(sp)
  sw x29, -14*4(sp)
  sw x30, -15*4(sp)
  sw x31, -16*4(sp)
  addi sp,sp,-16*4
  call irqCallback
  lw x1 , 15*4(sp)
  lw x5,  14*4(sp)
  lw x6,  13*4(sp)
  lw x7,  12*4(sp)
  lw x10, 11*4(sp)
  lw x11, 10*4(sp)
  lw x12,  9*4(sp)
  lw x13,  8*4(sp)
  lw x14,  7*4(sp)
  lw x15,  6*4(sp)
  lw x16,  5*4(sp)
  lw x17,  4*4(sp)
  lw x28,  3*4(sp)
  lw x29,  2*4(sp)
  lw x30,  1*4(sp)
  lw x31,  0*4(sp)
  addi sp,sp,16*4
  mret
  .text


crtInit:
  .option push
  .option norelax

  // Preserve old context: SP, GP and RA
  mv t1, sp
  la sp, _stack_start 
  addi sp, sp, -12
  sw t1, 8(sp)
  sw gp, 4(sp)
  sw ra, 0(sp)
  la gp, __global_pointer$
  .option pop

data_init:
  la a0, _data_start
  la a1, _data_end
  la a2, _rodata_end
data_loop:
  beq a0, a1, data_done
  lw t1, 0(a2)
  sw t1, 0(a0)
  add a2,a2,4
  add a0,a0,4
  j data_loop
data_done:

bss_init:
  la a0, _bss_start
  la a1, _bss_end
bss_loop:
  beq a0,a1,bss_done
  sw zero,0(a0)
  add a0,a0,4
  j bss_loop
bss_done:

ctors_init:
  la a0, _ctors_start
  addi sp,sp,-4
ctors_loop:
  la a1, _ctors_end
  beq a0,a1,ctors_done
  lw a3,0(a0)
  add a0,a0,4
  sw a0,0(sp)
  jalr  a3
  lw a0,0(sp)
  j ctors_loop
ctors_done:
  addi sp,sp,4

  li a0, 0x880     //880 enable timer + external interrupts
  csrw mie,a0
  //li a0, 0x1808     //1808 enable interrupts
  li a0, 0x3808     //2808 - enable FPU, enable Machine mode, enable Machine interrupts
  csrw mstatus,a0

  call main

return:
  // Restore old context: SP, GP and RA
  lw ra,0(sp)
  lw gp,4(sp)
  lw sp,8(sp)
  ret 

