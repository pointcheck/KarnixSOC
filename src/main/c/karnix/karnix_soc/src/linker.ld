/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.
*/
OUTPUT_FORMAT("elf32-littleriscv", "elf32-littleriscv", "elf32-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(crtStart)

MEMORY {

  /* Uncomment one of the below RAM regions: */

  /* Synthesized RAM */
  RAM      (rwx): ORIGIN = 0x80000000, LENGTH = 72k

  /* Synthesized RAM with 1K offset for bootloader/monitor */
  /* RAM      (rwx): ORIGIN = 0x80001000, LENGTH = 71k */

  /* SRAM */
  /* RAM      (rwx): ORIGIN = 0x90000000, LENGTH = 512k */
}

_stack_size = DEFINED(_stack_size) ? _stack_size : 4096;
_ram_heap_size = DEFINED(_ram_heap_size) ? _ram_heap_size : 0;


SECTIONS {

  ._vector ORIGIN(RAM): {
    *crt.o(.start_jump);
    *crt.o(.text);
  } > RAM

  .text : {
    . = ALIGN(4);
    *(.text);
    *(.text.*);
    end = .;
  } > RAM

  .ctors :
  {
    . = ALIGN(4);
    _ctors_start = .;
    KEEP(*(.init_array*))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    . = ALIGN(4);
    _ctors_end = .;
    PROVIDE ( END_OF_SW_IMAGE = . );
  } > RAM

  .rodata :
  {
    . = ALIGN(4);
    *(.rdata)
    *(.rodata)
    *(.rodata.*)
    *(.gnu.linkonce.r.*)
    *(.srodata.cst16)
    *(.srodata.cst8)
    *(.srodata.cst4)
    *(.srodata.cst2)
    *(.srodata .srodata.*)
    *(.eh_frame .eh_frame.*) /* C++ exception handling and backtrace */
    . = ALIGN(4);
    PROVIDE(_GOT = .);
    *(.got) /* global offset pointers for dynamic linker */
    *(.got.plt) /* global function pointers for dynamic linker */
    . = ALIGN(4);
    /* Provide pointer to reference impure_data (struct _reent) used in LIBC */
    PROVIDE(_IMPURE_DATA = .);
    *(.data.impure_data)
  } > RAM

  .data :
  {
    . = ALIGN(4);
    PROVIDE (_data_start = .);
    *(.data)
    *(.data.*)
    . = ALIGN(8);
    PROVIDE( __global_pointer$ = . + 0x800 );
    *(.gnu.linkonce.d.*)
    *(.sdata .sdata.*)
    *(.sdata2 .sdata2.*)
    *(.gnu.linkonce.s.*)
    . = ALIGN(4);
    PROVIDE(_data_end = .);
  } > RAM

  .bss (NOLOAD) : {
    . = ALIGN(4);
    /* This is used by the startup in order to initialize the .bss secion */
    _bss_start = .;
    *(.sbss*)
    *(.gnu.linkonce.sb.*)
    *(.bss .bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN(4);
    _bss_end = .;
  } > RAM

  .noinit (NOLOAD) : {
      . = ALIGN(4);
      *(.noinit .noinit.*)
      . = ALIGN(4);
  } > RAM

  ._stack (NOLOAD):
  {
    . = ALIGN(16);
    PROVIDE(_stack_end = .);
    . = . + _stack_size;
    . = ALIGN(16);
    PROVIDE(_stack_start = .);
  } > RAM

  ._ram_heap (NOLOAD):
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    PROVIDE ( _ram_heap_start = .);
    . = . + _ram_heap_size;
    /* PROVIDE ( _heap_end = ALIGN(ORIGIN(RAM) + LENGTH(RAM) - 8 ,8) ); */
    PROVIDE ( _ram_heap_end = ALIGN(ORIGIN(RAM) + LENGTH(RAM) ,8) );
  } > RAM

}
