/*
 * Copyright 2023 (C) Alexander Vysokovskikh
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define HAVE_RISCV_MEMCPY
#define HAVE_RISCV_MEMSET
#define HAVE_RISCV_MEMMOVE
//#define HAVE_RISCV_MEMMOVE_HALF_WORDS

//#define _memcpy memcpy
//#define _memset memset
//#define _memmove memmove

#if __riscv_xlen == 32
# define REG_S sw
# define REG_L lw
# define SZREG 4
#elif __riscv_xlen == 64
# define REG_S sd
# define REG_L ld
# define SZREG 8
#else
# error unsupported __riscv_xlen
#endif

.section .text, "ax", @progbits
    # we want to reduce the size of the code, so in order
    # to comply with the compressed ISA specification,
    # we will use preferably s0-s1, a0-a5 registers.

#ifdef HAVE_RISCV_MEMCPY
.globl memcpy
.type memcpy, @function
    #
    # void *memcpy(void *dst, void *src, size_t sz)
    #
    # Copies sz bytes from memory area src to memory area dst.
    # The memory areas must not overlap. Uses load/stores of XLEN.
    # For mutual misaligned buffers does byte-by-byte coping.
memcpy:
    # save initial dst value
    mv t2, a0

    # threshold for byte-by-byte copying
    li a3, 2 * SZREG
    bltu a2, a3, .Lmemcpy_bops

    # the src and dst buffers must have the same
    # alignment for load/store operations
    andi a3, a0, SZREG-1
    andi a4, a1, SZREG-1
    bne a3, a4, .Lmemcpy_bops
    beqz a3, .Lmemcpy_main

    # handle head misalignments
    addi a3, a3, -SZREG
    add a2, a2, a3
    sub a3, a0, a3
0:  lb a5, 0(a1)
    sb a5, 0(a0)
    addi a1, a1, 1
    addi a0, a0, 1
    blt a0, a3, 0b

    # copy 16/8/4/2/1*SZREG per one cycle iteration
.Lmemcpy_main:
    # according to convention
    # s0, s1 must be stored by callee
    mv t0, s0
    mv t1, s1

#ifndef __riscv_abi_rve
    li a7, 16*SZREG
    mv a3, a7
    bltu a2, a7, 7f
#else
    j 6f
#endif
1:  REG_L a4,  8*SZREG(a1)
    REG_L a5,  9*SZREG(a1)
    REG_L s0, 10*SZREG(a1)
    REG_L s1, 11*SZREG(a1)
    REG_S a4,  8*SZREG(a0)
    REG_S a5,  9*SZREG(a0)
    REG_S s0, 10*SZREG(a0)
    REG_S s1, 11*SZREG(a0)
    REG_L a4, 12*SZREG(a1)
    REG_L a5, 13*SZREG(a1)
    REG_L s0, 14*SZREG(a1)
    REG_L s1, 15*SZREG(a1)
    REG_S a4, 12*SZREG(a0)
    REG_S a5, 13*SZREG(a0)
    REG_S s0, 14*SZREG(a0)
    REG_S s1, 15*SZREG(a0)
2:  REG_L a4,  4*SZREG(a1)
    REG_L a5,  5*SZREG(a1)
    REG_L s0,  6*SZREG(a1)
    REG_L s1,  7*SZREG(a1)
    REG_S a4,  4*SZREG(a0)
    REG_S a5,  5*SZREG(a0)
    REG_S s0,  6*SZREG(a0)
    REG_S s1,  7*SZREG(a0)
3:  REG_L a4,  2*SZREG(a1)
    REG_L a5,  3*SZREG(a1)
    REG_S a4,  2*SZREG(a0)
    REG_S a5,  3*SZREG(a0)
4:  REG_L s0,  1*SZREG(a1)
    REG_S s0,  1*SZREG(a0)
5:  REG_L s1,  0*SZREG(a1)
    REG_S s1,  0*SZREG(a0)
    add a0, a0, a3
    add a1, a1, a3
    sub a2, a2, a3
#ifndef __riscv_abi_rve
6:  bgeu a2, a7, 1b
7:  srli a3, a7, 1
#else
6:  li a3, 16*SZREG
    bgeu a2, a3, 1b
7:  srli a3, a3, 1
#endif
    bgeu a2, a3, 2b
    srli a3, a3, 1
    bgeu a2, a3, 3b
    srli a3, a3, 1
    bgeu a2, a3, 4b
    srli a3, a3, 1
    bgeu a2, a3, 5b

    # restore s0, s1
    mv s1, t1
    mv s0, t0

    # handle tail misalignment
    # byte-by-byte copying
.Lmemcpy_bops:
    beqz a2, 1f
    add a2, a2, a0
0:  lb a4, 0(a1)
    sb a4, 0(a0)
    addi a1, a1, 1
    addi a0, a0, 1
    bltu a0, a2, 0b

    # return initial a0
1:  mv a0, t2
    ret
    .size memcpy, . - memcpy
#endif /* HAVE_RISCV_MEMCPY */

#ifdef HAVE_RISCV_MEMSET
.globl memset
.type memset, @function
    #
    # void *memset(void *dst, int ch, size_t sz)
    #
    # Function fills the first sz bytes of the memory
    # area pointed to by dst with the constant byte ch.
    # Uses stores operations of XLEN (register) size.
memset:
    # quit if sz is zero
    beqz a2, 9f

    # will return a0 untouched, further a5 = dst
    mv a5, a0

    # threshold for byte-by-byte operations
    li a3, 2 * SZREG
    bltu a2, a3, .Lmemset_bops

    # is dst aligned to register size
    andi a3, a5, SZREG-1
    beqz a3, .Lmemset_main

    # handle head misalignment
    addi a3, a3, -SZREG
    add a2, a2, a3
    sub a3, a5, a3
0:  sb a1, 0(a5)
    addi a5, a5, 1
    blt a5, a3, 0b

.Lmemset_main:
    # zero set byte
    beqz a1, 1f

    # propagate set value to whole register
    //zext.b a1, a1
    andi a1, a1, 255
    slli a3, a1, 8
    or a1, a1, a3
    slli a3, a1, 16
    or a1, a1, a3
#if __riscv_xlen == 64
    slli a3, a1, 32
    or a1, a1, a3
#endif

1:  li a4, 32*SZREG
    mv a3, a4
    bltu a2, a4, 7f

    # stores 32/16/8/4/2/1*SZREG per one cycle iteration
0:  REG_S a1, 16*SZREG(a5)
    REG_S a1, 17*SZREG(a5)
    REG_S a1, 18*SZREG(a5)
    REG_S a1, 19*SZREG(a5)
    REG_S a1, 20*SZREG(a5)
    REG_S a1, 21*SZREG(a5)
    REG_S a1, 22*SZREG(a5)
    REG_S a1, 23*SZREG(a5)
    REG_S a1, 24*SZREG(a5)
    REG_S a1, 25*SZREG(a5)
    REG_S a1, 26*SZREG(a5)
    REG_S a1, 27*SZREG(a5)
    REG_S a1, 28*SZREG(a5)
    REG_S a1, 29*SZREG(a5)
    REG_S a1, 30*SZREG(a5)
    REG_S a1, 31*SZREG(a5)
1:  REG_S a1,  8*SZREG(a5)
    REG_S a1,  9*SZREG(a5)
    REG_S a1, 10*SZREG(a5)
    REG_S a1, 11*SZREG(a5)
    REG_S a1, 12*SZREG(a5)
    REG_S a1, 13*SZREG(a5)
    REG_S a1, 14*SZREG(a5)
    REG_S a1, 15*SZREG(a5)
2:  REG_S a1,  4*SZREG(a5)
    REG_S a1,  5*SZREG(a5)
    REG_S a1,  6*SZREG(a5)
    REG_S a1,  7*SZREG(a5)
3:  REG_S a1,  2*SZREG(a5)
    REG_S a1,  3*SZREG(a5)
4:  REG_S a1,  1*SZREG(a5)
5:  REG_S a1,  0*SZREG(a5)
    add a5, a5, a3
    sub a2, a2, a3
6:  bgeu a2, a4, 0b
    beqz a2, 9f
7:  srli a3, a4, 1
    bgeu a2, a3, 1b
    srli a3, a3, 1
    bgeu a2, a3, 2b
    srli a3, a3, 1
    bgeu a2, a3, 3b
    srli a3, a3, 1
    bgeu a2, a3, 4b
    srli a3, a3, 1
    bgeu a2, a3, 5b

    # handle tail misalignment
.Lmemset_bops:
    add a2, a2, a5
0:  sb a1, 0(a5)
    addi a5, a5, 1
    bltu a5, a2, 0b
9:  ret
    .size memset, . - memset
#endif /* HAVE_RISCV_MEMSET */

#ifdef HAVE_RISCV_MEMMOVE
.globl memmove
.type memmove, @function
    #
    # void *memmove(void *dst, void *src, size_t sz)
    #
    # Function copies sz bytes from memory area src to memory area dst.
    # The memory areas may overlap. Copies using 8/4/2/1 bytes load/stores
memmove:
    # save a0, s1
    mv t2, a0
    mv t1, s1

    # threshold for byte operations
    li a5, 2 * SZREG

    # find out mutual buffer alignment
#if __riscv_xlen == 64
    andi a3, a0, 7
    andi a4, a1, 7
    li s1, 8
    beq a3, a4, 1f
    andi a3, a3, 3
    andi a4, a4, 3
#else
    andi a3, a0, 3
    andi a4, a1, 3
#endif
    li s1, 4
    beq a3, a4, 1f
#ifdef HAVE_RISCV_MEMMOVE_HALF_WORDS
    andi a3, a3, 1
    andi a4, a4, 1
    li s1, 2
    beq a3, a4, 1f
#endif
    li s1, 0

    # copy from the end if dst > src
1:  bltu a1, a0, .Lmemmove_r

    # byte copy if sz is less than threshold
    bltu a2, a5, .Lmemmove_1b

    # byte copy if src and dst are mutual unaligned
    beqz s1, .Lmemmove_1b

    # at this point:
    # s1 = 8/4/2
    # a4 = head misaligned bytes

    beqz a4, 1f

    # handle head misalignment by byte copying
    sub a4, s1, a4
    sub a2, a2, a4
    add a4, a4, a0
0:  lb a3, 0(a1)
    sb a3, 0(a0)
    addi a1, a1, 1
    addi a0, a0, 1
    bltu a0, a4, 0b

    # calculate last address and tail misaligned bytes number
1:  addi a4, s1, -1
    xori a4, a4, -1
    and a4, a2, a4
    sub a2, a2, a4
    add a4, a4, a0

    # use 8/4/2 byte load/store instructions if buffers are
    # mutually aligned on 8/4/2 byte boundary respectively.
#if __riscv_xlen == 64
    li a5, 8
    bne s1, a5, 1f
0:  ld a3, 0(a1)
    sd a3, 0(a0)
    add a1, a1, s1
    add a0, a0, s1
    bltu a0, a4, 0b
    j .Lmemmove_1b
#endif
1:  li a5, 4
    bne s1, a5, 1f
0:  lw a3, 0(a1)
    sw a3, 0(a0)
    add a1, a1, s1
    add a0, a0, s1
    bltu a0, a4, 0b
#ifdef HAVE_RISCV_MEMMOVE_HALF_WORDS
    j .Lmemmove_1b
1:  li a5, 2
    bne s1, a5, 1f
0:  lh a3, 0(a1)
    sh a3, 0(a0)
    add a1, a1, s1
    add a0, a0, s1
    bltu a0, a4, 0b
#endif

    # byte copy
.Lmemmove_1b:
1:  beqz a2, .Lmemmove_end
    add a2, a2, a0
0:  lb a3, 0(a1)
    sb a3, 0(a0)
    addi a1, a1, 1
    addi a0, a0, 1
    bltu a0, a2, 0b

.Lmemmove_end:
    # restore saved registers
    mv s1, t1
    mv a0, t2
    ret

.Lmemmove_r:
    # start from the end: src += sz, dst += sz
    add a0, a0, a2
    add a1, a1, a2

    # here: a5 = threshold for byte-by-byte copying
    bltu a2, a5, .Lmemmove_r1b

    # byte copy if src and dst are mutual unaligned
    beqz s1, .Lmemmove_r1b

    # fix head misaligned bytes
    add a4, a4, a2
    addi a3, s1, -1
    and a4, a4, a3

    # s1 = 8/4/2
    # a4 = head misaligned bytes
    beqz a4, 1f

    # handle head misalignment
    sub a2, a2, a4
    sub a5, a0, a4
0:  addi a1, a1, -1
    addi a0, a0, -1
    lb a3, 0(a1)
    sb a3, 0(a0)
    bgtu a0, a5, 0b

    # calculate last address and tail misaligned bytes number
1:  addi a4, s1, -1
    xori a4, a4, -1
    and a4, a2, a4
    sub a2, a2, a4
    sub a4, a0, a4

    # use 8/4/2 byte load/store instructions if buffers are
    # mutually aligned on 8/4/2 byte boundary respectively.
#if __riscv_xlen == 64
    li a5, 8
    bne s1, a5, 1f
0:  sub a1, a1, s1
    sub a0, a0, s1
    ld a3, 0(a1)
    sd a3, 0(a0)
    bltu a4, a0, 0b
    j .Lmemmove_r1b
#endif
1:  li a5, 4
    bne s1, a5, 1f
0:  sub a1, a1, s1
    sub a0, a0, s1
    lw a3, 0(a1)
    sw a3, 0(a0)
    bltu a4, a0, 0b
#ifdef HAVE_RISCV_MEMMOVE_HALF_WORDS
    j .Lmemmove_r1b
1:  li a5, 2
    bne s1, a5, 1f
0:  sub a1, a1, s1
    sub a0, a0, s1
    lh a3, 0(a1)
    sh a3, 0(a0)
    bltu a4, a0, 0b
#endif

    # byte copy
.Lmemmove_r1b:
1:  beqz a2, .Lmemmove_end
    sub a5, a0, a2
0:  addi a1, a1, -1
    addi a0, a0, -1
    lb a3, 0(a1)
    sb a3, 0(a0)
    bgtu a0, a5, 0b
    j .Lmemmove_end
    .size memmove, . - memmove
#endif /* HAVE_RISCV_MEMMOVE */
