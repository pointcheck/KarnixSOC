# example_ram is an example of RAM located program for Karnix SOC.

This example uses [karnix_soc/src/linker.ld](../karnix_soc/src/linker.ld) linker script and [karnix_soc/src/crt.S](../karnix_soc/src/crt.S) as startup code, which both implement Software Model #1 (see top README.md for more info about software models). In this model program can be loaded in any location in RAM. In case of Karnix SoC it can be either in synthesized RAM begnning at 0x80000000 (72 KB) or external SDRAM at 0x90000000 (512 KB) soldered on Karnix board. To load example_ram binary to RAM one needs a help of Monitor or a Bootloader. 

## What it does

This example does the following:

1. Intitializes ```.bss``` data segment and overrides ```trap_entry``` point to install its own function ```irqCallback()```.
2. Setups PLIC, Timer1 and Timer2 periodic interrupts, prints some info in IRQ handle for timer interrupts.
3. Uses global and local array to print some text.
4. Initializes heap by calling to ```init_sbrk()``` and uses ```malloc()``` to test it.
5. Initializes stdlib's (LIBC) ```impure_data``` structure by adjusting pointers to ```struct _reent``` and to fake file stream sructures.
6. Prints global pointers.
7. Call ```printf()``` as one of the most heavy stdlib function to demonstrate stdlib is ready and workng.
8. Returns to Monitor with some result code.

## How to build

Just use ```make``` command in **karnix/example_ram/** directory:

```
rz@devbox:~/KarnixSOC/src/main/c/karnix/example_ram$ make
...
Memory region         Used Size  Region Size  %age Used
             RAM:       13504 B        72 KB     18.32%
/opt/riscv64/bin/riscv64-unknown-elf-objdump -S -d build/example_ram.elf > build/example_ram.asm
/opt/riscv64/bin/riscv64-unknown-elf-objcopy -O ihex --change-section-address *-0x80000000 build/example_ram.elf build/example_ram.hex
/opt/riscv64/bin/riscv64-unknown-elf-objcopy -O binary build/example_ram.elf build/example_ram.bin
```

The resulting files will be in **karnix/example_ram/build**:

```
rz@devbox:~/KarnixSOC/src/main/c/karnix/example_ram$ ls -l build/
total 320
drwxrwxr-x 3 rz rz   4096 Jan 13 21:41 __
-rw-rw-r-- 1 rz rz      2 Jan 13 21:41 build_number
-rw-rw-r-- 1 rz rz 149891 Jan 13 21:41 example_ram.asm
-rwxrwxr-x 1 rz rz   9320 Jan 13 21:41 example_ram.bin
-rwxrwxr-x 1 rz rz  19944 Jan 13 21:41 example_ram.elf
-rw-rw-r-- 1 rz rz  26266 Jan 13 21:41 example_ram.hex
-rw-rw-r-- 1 rz rz  20970 Jan 13 21:41 example_ram.hexx
-rw-rw-r-- 1 rz rz  75827 Jan 13 21:41 example_ram.map
drwxrwxr-x 2 rz rz   4096 Jan 13 21:41 src
``` 

You will be needing ```example_ram.bin``` or ```example_ram.hex```, both can be loaded into RAM and ran by Monitor. You also may be interested in looking through resulting assembly code which resides in ```example_ram.asm```.

## How to run

1. Copy resulting ```example_ram.hex``` to your working machine if your are developing remotely.

2. Connect and boot Karnix board preloaded with Monitor (karnix_monitor). You can flash the board with Monitor using:

```
rz@butterfly:~ % openFPGALoader -f KarnixSOCTopLevel_25F-karnix_monitor.bit
```

Note, the bitstream file ```KarnixSOCTopLevel_25F-karnix_monitor.bit``` can be found in this same repository in hardware build directory.

3. Connect terminal to the board on DEBUG serial port /dev/ttyU1 (/dev/ttyUSB1 on Linux) using ```minicom``` or ```cu```. Since I'm a FreeBSD user I'll use ```cu``` as terminal, like this:

```
rz@butterfly:~ % sudo cu -l /dev/ttyU1 -s 115200
```

Hit enter and you should get the prompt like:

```
MONITOR[0x80000000]-> 
```

You may use ESC or Ctrl-U to clear input line.


For ```minicom``` users:

```
rz@butterfly:~ % sudo minicom -D /dev/ttyU1 -b 115200
```

3. Send ```st 0``` command to Monitor to disable printing annoying IRQ and buffer statistics:

```
MONITOR[0x80000000]-> st 0
reg_sys_print_stats = 0
```

4. Now upload your ```example_ram.hex``` at SRAM location 0x90000000 using ```ihex``` command:

```
MONITOR[0x80000000]-> ihex 0x90000000
/// ihex: addr = 0x90000000, press Ctrl-C to break.
```
Now I type ```~>``` to get to cu's command mode to send text file:
```
~>Local file name? example_ram.hex
```

Once sending is over the following statistics will be provided by Monitor:

```
/// ihex: bytes_read = 9320, addr = 0x90000000, entry = 0x10000000, crc32 = 0x9eeafe14 (cksum -o 3)
```

You may compare CRC32 checksums if you like. On FreeBSD use ```cksum -o 3 example_ram.hex```, then convert decimal result to hex.

For ```minicom``` users, sending ASCII file can be done by pressing ```Ctrl-A, S```, select ```ascii``` when select file to send.

5. Call the program with ```call *``` command. You can use any arbitrary address instead of '*', which means last used address.

```
MONITOR[0x90000000]-> call *
/// call: addr = 0x90000000, argn = 1
```

Monitor reports calling address and number of passing text arguments, as in the above.

Same moment output from ```example_ram``` program starts printing to the terminal:

```
TIMER0 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER0 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ

= = = HELLO WORLD = = =
*** Use global read-write data:
ABC0
ABC1
ABC2
ABC3
ABC4
ABC5
ABC6
ABC7
ABC8
ABC9

*** Print current context:
SP: 90003484
_stack_start: 900034C0
_bss_start: 90002468
_bss_end: 900024B4
_ram_heap_start: 900034C0
_ram_heap_end: 90012000

*** Init heap:
init_sbrk done!
heap_start: 900034C0
heap_end: 90012000
sbrk_heap_end: 900034C0

*** Adjusting global REENT structure:
_impure_ptr: 900023FC
_global_impure_ptr: 900023FC
fake_stdout: 900023A0

*** Checking malloc:
_sbrk() request, incr: 00000000
_sbrk() prev_heap_end: 900034C0
_sbrk() request, incr: 00000408
_sbrk() prev_heap_end: 900034C0
allocated mem at: 900034C8
_sbrk() request, incr: 00000408
_sbrk() prev_heap_end: 900038C8
allocated mem at: 900038D0
_sbrk() request, incr: 00000408
_sbrTIMER0 IRQ
k() prevTIMER1 IRQ
_heap_end: 90003CD0
allocated mem at: 90003CD8
_sbrk() request, incr: 00000408
_sbrk() prev_heap_end: 900040D8
allocated mem at: 900040E0
_sbrk() request, incr: 00000408
_sbrk() prev_heap_end: 900044E0
allocated mem at: 900044E8
_sbrk() request, incr: 00000408
_sbrk() prev_heap_end: 900048E8
allocated mem at: 900048F0
_sbrk() request, incr: 00000408
_sbrk() prev_heap_end: 90004CF0
allocated mem at: 90004CF8
_sbrk() request, incr: 00000408
_sbrk() prev_heap_end: 900050F8
allocated mem at: 90005100
_sbrk() request, incr: 00000408
_sbrk() prev_heap_end: 90005500
allocated mem at: 90005508
_sbrk() request, incr: 00000408
_sbrk() prev_heap_end: 90005908
allocated mem at: 90005910

*** Calling LIBC function...
_sbrk() request, incr: 000001B4
_sbrk() prev_heap_end: 90005D10
_sbrk() request, incr: 00000408
_sbrk() prev_heap_end: 90005EC4
Hello from LIBC! Yoohoo!

= = = Returning to Monitor = = =

TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER0 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER1 IRQ
TIMER0 IRQ
TIMER1 IRQ
/// call: ret = 0x11223344, exec time = 4153857 millisecs
MONITOR[0x90000000]-> 
```

6. Once program is over you will get back to the Monitor prompt. Resulting code (0x11223344) and total execution time (4153857) will be shown as in above.

Note, on return from ```call```, Monitor always restores its context as well as trap handler and PLIC state. You do not have to do this in your program.

Use this ```example_ram``` as template for your programs.

PS: You can store your programs to NOR flash using ```openFPGALoader -o <offset>```, then use ```copy <to> <from> <size>``` Monitor command to copy your program from NOR Flash to RAM before calling it. Calling (executing) from NOR flash won't work in this software model, check [example_xip](../example_xip/) for that. 

Good luck!

--
Regards,

Ruslan Zalata

