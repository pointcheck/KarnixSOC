# This is an example of simple program for Karnix SOC.

This example can be compiled both for Software model #1 (RAM) and model #2 (XIP).

When compiled for RAM, all the program segments (.text, .rodata, .data, .stack and .heap) will be put one after another in same memory region. By default it's 0x80003000 as prescribed ```linker_ram.ld```. In this model program can be loaded and ran at any address in RAM or SRAM meaning its size (including required heap) fully fits in the memory region. If it does not fit in synthesizable RAM, one can load it into SRAM at 0x90000000 without recompilation.

When compiled for XIP, things work differenty. In this mode the binary data of .text, .rodata and .data segments will be placed same way one after another in .bin file, but when running the code will expect to be loaded at a fixed location at 0xA0100000 followed by .rodata which corresponds to 0x100000 offset in NOR flash. The .data, .stack and .heap are expected to be located starting at 0x80003000 address in RAM. To put .data at location different than .text and .rodata, a special startup procedure ```data_init``` inside ```crt_xip.S``` is called to copy modifiable data from XIP location to RAM location. This allows running program and keep its read-only data (constanst and strings) in NOR flash while only writable data, stack and heap will be moved to RAM. A negative consequence is that if you want to store binary file at different location in NOR flash you will need to modify ORIGIN in linker_xip.ld file and recompile the program.
 
## What example does

This example does the following:

1. Intitializes .bss data segment in RAM, copies .data segment to RAM (XIP only) and overrides ```trap_entry``` point to install its own function ```irqCallback()```.
2. Configures PLIC, sets Timer1 and Timer2 periodic interrupts, prints some info in IRQ handle when timer interrupts occur.
3. Uses global and local arrays to print some text to test access to .data and .stack.
4. Initializes heap by calling to ```init_sbrk()``` and uses ```malloc()``` to test it.
5. Initializes LIBC's ```impure_data``` structure by adjusting pointers to ```struct _reent``` and to fake file stream sructures.
6. Prints global pointers.
7. Calls ```printf()``` as one of the most heavy LIBC function to demonstrate LIBC is ready and working.
8. Returns to Monitor with some result code.

## How to build

Just use ```make``` command in **karnix/example/** directory, it will produce both RAM and XIP version. Or use distinctive ```make ram``` or ```make xip``` targets.

```
rz@devbox:~/KarnixSOC/src/main/c/karnix/example$ make
...
Memory region         Used Size  Region Size  %age Used
             RAM:        7328 B        60 KB     11.93%
/opt/riscv64/bin/riscv64-unknown-elf-objcopy -O ihex build/example_ram.elf build/example_ram.hex
/opt/riscv64/bin/riscv64-unknown-elf-objcopy -O binary build/example_ram.elf build/example_ram.bin
/opt/riscv64/bin/riscv64-unknown-elf-objdump -S -d build/example_ram.elf > build/example_ram.asm
/opt/riscv64/bin/riscv64-unknown-elf-objcopy -O verilog build/example_ram.elf build/example_ram.v
hexdump -v -e '/4 "%08X\n"' < build/example_ram.bin > build/example.hexx 

...

Memory region         Used Size  Region Size  %age Used
           FLASH:        5228 B        15 MB      0.03%
             RAM:        2240 B        60 KB      3.65%
/opt/riscv64/bin/riscv64-unknown-elf-objcopy -O ihex build/example_xip.elf build/example_xip.hex
/opt/riscv64/bin/riscv64-unknown-elf-objcopy -O binary build/example_xip.elf build/example_xip.bin
/opt/riscv64/bin/riscv64-unknown-elf-objdump -S -d build/example_xip.elf > build/example_xip.asm
/opt/riscv64/bin/riscv64-unknown-elf-objcopy -O verilog build/example_xip.elf build/example_xip.v
make[2]: Leaving directory '/home/rz/RISCV/KarnixSOC/src/main/c/karnix/example'
make[1]: Leaving directory '/home/rz/RISCV/KarnixSOC/src/main/c/karnix/example'
```

The resulting files will be in **karnix/example/build**:

```
-rw-rw-r-- 1 rz rz 11700 Jan 26 23:40 example.hexx
-rw-rw-r-- 1 rz rz 74902 Jan 26 23:40 example_ram.asm
-rwxrwxr-x 1 rz rz  5200 Jan 26 23:40 example_ram.bin*
-rwxrwxr-x 1 rz rz 14768 Jan 26 23:40 example_ram.elf*
-rw-rw-r-- 1 rz rz 14715 Jan 26 23:40 example_ram.hex
-rw-rw-r-- 1 rz rz 45195 Jan 26 23:40 example_ram.map
-rwxrwxr-x 1 rz rz 16300 Jan 26 23:40 example_ram.v*
-rw-rw-r-- 1 rz rz 75752 Jan 26 23:40 example_xip.asm
-rwxrwxr-x 1 rz rz  5228 Jan 26 23:40 example_xip.bin*
-rwxrwxr-x 1 rz rz 18024 Jan 26 23:40 example_xip.elf*
-rw-rw-r-- 1 rz rz 14784 Jan 26 23:40 example_xip.hex
-rw-rw-r-- 1 rz rz 45336 Jan 26 23:40 example_xip.map
-rwxrwxr-x 1 rz rz 16386 Jan 26 23:40 example_xip.v*
``` 

You will be needing ```example_ram.bin``` or ```example_ram.hex```, both can be loaded into RAM and ran by Monitor. You also may be interested in looking through resulting assembly code which resides in ```example_ram.asm```.

## How to run RAM version

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

You may use ESC or Ctrl-U to clear input line from garbage.


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
/// ihex: bytes_read = 5200, location = 0x90000000, crc32 = 0x883f0535 (cksum -o 3)
/// ihex: data from file: origin = 0x80000000, entry = 0x80000000
```

You may compare CRC32 checksums if you like. On FreeBSD use ```cksum -o 3 example_ram.hex```, then convert decimal result to hex.

For ```minicom``` users, sending ASCII file can be done by pressing ```Ctrl-A, S```, select ```ascii``` when select file to send.

5. Call the program with ```call *``` command. You can use any arbitrary address instead of '*', which means last used address.

```
MONITOR[0x90000000]-> call *
/// call: addr = 0x90000000, argn = 1
```

Monitor reports calling address and number of passing text arguments, as in the above.

Note that we call program to actual location (0x90000000) it was loaded at, not to the origin it was compiled with. That's because Software model #1 allows this.

Right after ```call``` command the output from ```example_ram``` program starts printing to the terminal:

```
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
SP: 0x90001c74, _stack_start: 0x90001ca0, _bss_start: 0x90001450
_bss_end: 0x9000149c, _ram_heap_start: 0x90001ca0, _ram_heap_end: 0x90012000

*** Setting up PLIC and timers:
PLIC->ENABLE: 0x00000018, PLIC->EDGE: 0xffffffe0, PLIC->POLARITY: 0x0000007f

*** Init heap:
init_sbrk done!
heap_start: 0x90001ca0, heap_end: 0x90012000, sbrk_heap_end: 0x90001ca0

*** Adjusting global REENT structure:
_impure_ptr: 0x900013e8, _global_impure_ptr: 0x900013e8, fake_stdout: 0x900013c8

*** Checking malloc:
allocated 1024 bytes of mem at: 0x90001ca8
zeroing 1024 bytes of mem at: 0x90001ca8
allocated 1024 bytes of mem at: 0x900020b0
zeroing 1024 bytes of mem at: 0x900020b0
allocated 1024 bytes of mem at: 0x900024b8
zeroing 1024 bytes of mem at: 0x900024b8
allocated 1024 bytes of mem at: 0x900028c0
zeroing 1024 bytes of mem at: 0x900028c0
allocated 1024 bytes of mem at: 0x90002cc8
zeroing 1024 bytes of mem at: 0x90002cc8
allocated 1024 bytes of mem at: 0x900030d0
zeroing 1024 bytes of mem at: 0x900030d0
allocated 1024 bytes of mem at: 0x900034d8
zeroing 1024 bytes of mem at: 0x900034d8
allocated 1024 bytes of mem at: 0x900038e0
zeroing 1024 bytes of mem at: 0x900038e0
allocated 1024 bytes of mem at: 0x90003ce8
zeroing 1024 bytes of mem at: 0x90003ce8
allocated 1024 bytes of mem at: 0x900040f0
zeroiTIMER1 IRQ
ng 1024 bytes of mem at: 0x900040f0

*** Calling LIBC function...
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
```

6. Once program is over you will get back to the Monitor prompt. Resulting code (0x11223344) and total execution time (4153857) will be shown as below:

```
/// call: ret = 0x11223344 (287454020), exec time = 4153857 millisecs
MONITOR[0x90000000]->
```

Note 1. On return from ```call```, Monitor always restores its context as well as trap handler and PLIC state. You do not have to do this in your program.

Note 2. You can store your RAM compiled programs to NOR flash using ```openFPGALoader -o <offset>```, then use ```copy <to> <from> <size>``` Monitor command to copy program from NOR Flash to RAM before calling it. Calling (executing) from NOR flash won't work in this software model, read below how to run XIP version. 


## How to run XIP version

1. Copy resulting ```example_xip.bin``` (not ```example_xip.hex``` !!!) to your working machine if your are developing remotely.

2. Connect your Karnix board and store this binary file to NOR flash at offset 0x100000:

```
rz@butterfly:~ % openFPGALoader -f -o 0x100000 example_xip.bin
```

3. Now boot Karnix board preloaded with Monitor (karnix_monitor), connect terminal to get to Monitor command like.

4. Send ```st 0``` command to Monitor to disable printing annoying IRQ and buffer statistics.

5. Send ```call 0xA0100000``` command to jump to ```example_xip``` code, like below:

```
MONITOR[0x80000000]-> call 0xa0100000
/// call: addr = 0xa0100000, argn = 1

= = = HELLO WORLD = = =

...blah-blah-blah

= = = Returning to Monitor = = =

/// call: ret = 0x11223344 (287454020), exec time = 4254887 millisecs
MONITOR[0xa0100000]-> 
```

Printings from ```example_xip``` should be similar to ```example_ram``` except that global variables will have different values and locations.

Note, that address 0xA0000000 is where NOR flash begins. So if you load XIP binary into NOR flash at offset 0x100000 the resulting address will appear as 0xA0100000 which we used in ```call``` command above. Putting XIP binary at different offset required modifying origin in ```linker_xip.ld``` and recompiling binary.

## Final

Use this ```example``` as template for your programs.

Good luck!

--
Regards,

Ruslan Zalata

