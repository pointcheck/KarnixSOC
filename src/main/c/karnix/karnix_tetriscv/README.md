# TetRISC-V is a simple Tetris game for Karnix SoC

This game is a demonstration of graphics capabilities of synthesizable CGA-like video adapter inside Karnix SoC. The game implements following video effects:

1. Switching palette on each HSYNC signal thus allowing more than 4 colours at once on the screen. Note, CGA has 4 colours by default in graphics mode and rainbow-like color for text in not possible.

2. Sequencially switching between text and graphics mode allows combining text and graphics on the screen. Resulting refresh rate is 59/2 = 29 Hz, which potentially can hurt eyes of finicky user. 

3. Smooth text scrolling.

The game also playbacks math synthesized music on AUDIODAC0 that is routed to Audio Jack. Note, this may hurt ears of demanding user as well.

[Video demonstration on Youtube](https://www.youtube.com/watch?v=WuLQwD38eDc)

## How to build

Run ```make``` command from current directory. Resulting files for both RAM and XIP models fill be put in ```./build``` as follows:

```
-rw-rw-r-- 1 rz rz  62100 Jan 24 01:16 karnix_tetriscv.hexx
-rw-rw-r-- 1 rz rz 360446 Jan 24 01:16 karnix_tetriscv_ram.asm
-rwxrwxr-x 1 rz rz  27600 Jan 24 01:16 karnix_tetriscv_ram.bin*
-rwxrwxr-x 1 rz rz  89868 Jan 24 01:16 karnix_tetriscv_ram.elf*
-rw-rw-r-- 1 rz rz  77698 Jan 24 01:16 karnix_tetriscv_ram.hex
-rw-rw-r-- 1 rz rz 120943 Jan 24 01:16 karnix_tetriscv_ram.map
-rwxrwxr-x 1 rz rz  86292 Jan 24 01:16 karnix_tetriscv_ram.v*
-rw-rw-r-- 1 rz rz 361190 Jan 24 23:15 karnix_tetriscv_xip.asm
-rwxrwxr-x 1 rz rz  27628 Jan 24 23:15 karnix_tetriscv_xip.bin*
-rwxrwxr-x 1 rz rz  91916 Jan 24 23:15 karnix_tetriscv_xip.elf*
-rw-rw-r-- 1 rz rz  77759 Jan 24 23:15 karnix_tetriscv_xip.hex
-rw-rw-r-- 1 rz rz 121084 Jan 24 23:15 karnix_tetriscv_xip.map
-rwxrwxr-x 1 rz rz  86366 Jan 24 23:15 karnix_tetriscv_xip.v*
```

## How to run binary built for RAM

1. You will need to boot Karnix board into Monitor. Read more about [Karnix Monitor](../karnix_monitor) and how to install it.

2. Connect to debug UART of the board using terminal (minicom or cu).

3. Upload ```karnix_tetriscv_ram.hex``` to location at 0x80003000 using ```ihex``` command:

```
MONITOR[0x80000000]-> ihex 0x80003000
/// ihex: addr = 0x80003000, press Ctrl-C to break.
```

Here I'm using ```cu```-s ```~>``` command to send text file:
```
~>Local file name? karnix_tetriscv_ram.hex
/// ihex: bytes_read = 27596, location = 0x80003000, crc32 = 0x7b42902c (cksum -o 3)
/// ihex: data from file: origin = 0x80000000, entry = 0x80000000
```

4. Now send ```call 0x80003000``` command to give execution control to loaded code.

On the terminal you will see something like:

```
MONITOR[0x80003000]-> call 0x80003000
/// call: addr = 0x80003000, argn = 1

*** Init heap:
init_sbrk done!
heap_start: 0x8000a670, heap_end: 0x80015000, sbrk_heap_end: 0x8000a670

*** Adjusting global REENT structure:
_impure_ptr: 0x800099dc, _global_impure_ptr: 0x800099dc, fake_stdout: 0x80009848

TetRISC-V for Karnix SoC. Build 00002 on Jan 27 2025 at 22:34:24
Copyright (C) 2024-2025 Fabmicro, LLC., Tyumen, Russia.

=== Configuring ===

Press '*' to reset config.....
eeprom_probe(0x50) done
config_load() CRC16 mismatch: 0xB001 != 0xFFFF
Defaults loaded by EEPROM CRC ERROR!
=== Hardware init ===
Filling SRAM at: 0x90000000, size: 524288 bytes...
Checking SRAM at: 0x90000000, size: 524288 bytes...
Enabling SRAM...
SRAM at 0x90000000 is enabled!
CGA init done
UART0 init done
TIMER0 init done
TIMER1 init done
audiodac_init: divider = 227
audiodac0_isr: tx ring buffer underrun!
audiodac0_start_playback: done, fifo depth is 2048 samples
AUDIODAC0 init done
Video double-buffers allocated
=== Hardware init done ===
```

## How to run binary built for XIP 

To run from XIP you have to have other Bootloader or bot Bootloader and Monitor installed.

1. Upload ```karnix_tetriscv_xip.bin``` file (yes it's .bin, not .hex) to NOT flash to one of the offsets recognized by Bootloader. In example:

```
rz@butterfly:~ % openFPGALoader -f -o 0x100000 karnix_tetriscv_xip.bin
```

2. If no Monitor installed, Bootloader will automatically find and jump to TetRISC-V code.

3. If there's Monitor, connect to debug UART using terminal and send command ```call 0xA0100000``` to give execution to TetRISC-V code.

Note, the addreess 0xA0100000 is where NOR offset 0x100000 is mapped to in SoC's global address space.


## How to play

Enjoy playing Tetris by using four on-board keys to move tetrominos.

 
--
Regards,

Ruslan Zalata
