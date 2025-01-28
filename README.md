# Karnix synthesizable System-on-Chip based on VexRiscv RISC-V soft-core

This repository is an extension of [VexRiscv](https://github.com/SpinalHDL/VexRiscv) original repo. Thanks should be directed to its creator Charles Papon (Dolu1990) first. Karnix SOC is heavily based on original Briey SoC.

## Karnix SoC features

Karnix SOC (former BrieyForKarnix SoC) is based on VexRiscv RV32IMFAC ISA with MMU support in the following configurations:

- 2K I$ and 2K D$ caches
- Dynamic branch prediction
- MUL/DIV
- FPU (single precision)
- Atomic extension
- MMU
- AXI4 and APB3 buses
- 72 KB of on-chip synthesizable RAM
- Two 32 bit timers
- 1-us machine timer
- PWM timer
- Watchdog timer
- Two UARTs
- PLIC controller with 32 IRQ channels
- FastEthernet controller connecter to RJ-45 on Karnix board
- SRAM controller connected to K6R4016V1D 16x256 Kbit (512 KB) chip on Karnix board
- I2C controller connected to EEPROM on Karnix board
- SPI controller connected to AD5328 on Karnix board used as AudioDAC
- SPI controller connected to external GPIO header
- CGA-like video adapter with HDMI interface on Karnix board
- QSPI controller connector to Winbond Q25W128 NOR flash
- (optional) HUB13/75 LED matrix controller connected to GPIO header

Fmax = 60 MHz on Lattice ECP5 25F grade 7

## Hardware Requirements

To use this repositiry one needs a board with Lattice ECP5 FPGA chip. Karnix SoC was developped to be run on [Karnix ASB-254](https://github.com/Fabmicro-LLC/Karnix_ASB-254) OSHW board, but it's easy to adopt it to any other board with same FPGA chip.

If you are interested in porting this SoC to other boards, or to other FPGAs, please let me know.

## Software Requirements

To use this repository one needs to have these tools installed:

1. Yosys toolchain, also known as [OSS CAD Suite](https://github.com/YosysHQ/yosys) provided by YosysHQ. They have [pre-built binary packs](https://github.com/YosysHQ/oss-cad-suite-build) for all major OSes.

2. One of C compilers for RISC-V architecture. I'm using [bre-built GCC toolchain from SiFive](https://github.com/sifive/freedom-tools/releases).

3. ```sbt``` build tool for Scala. Get it from your Linux/FreeBSD repository (```apt-get install sbt```).

## Directory structure

Hence this is an extension of VexRiscv, the whole directory structure stays same as in original respository. Added code resides in:

- [./src/main/scala/vexriscv/demo/KarnixSOC.scala](src/main/scala/vexriscv/demo/KarnixSOC.scala) - Karnix SOC written in SpinalHDL.
- [./src/main/scala/mylib/](src/main/scala/mylib/) - Karnix SOC hardware components also written in SpinalHDL.
- [./scripts/KarnixSOC/ECP5-25F_karnix_board/](scripts/KarnixSOC/ECP5-25F_karnix_board/) - Working directory containing Makefile to build Karnix SOC for Karnix board with Lattice ECP5 25F FPGA chip.
- [./src/main/c/karnix/](src/main/c/karnix/) - C code with Karnix SOC Hardware Abstraction Layer, Bootloader, Monitor, Tetris game [TetRISC-V](src/main/c/karnix/karnix_tetriscv/), examples, test hardware code and more.

Please check corresponding READMEs for more details.

## How to build

1. To generate and synthesize Karnix SoC go to working directory, edit Makefile to set default RAMPROG it should be built with, then issue ```make``` command like in the following:

```
cd ./scripts/KarnixSOC/ECP5-25F_karnix_board 
vi Makefile
make generate compile
```

It will consequently run:
- Build of C code for given RAMPROG
- Scala sbt to compile HDL code written in SpinalHDL
- resulting SpinalHDL to generate Verilog code of the SoC
- Yosys/NextPNR to synthesize Verilog into bitstream 

On success the bitstream will be available in ```bin/KarnixSOCTopLevel_25F.bit```

It will include RAMPROG program stored in on-board synthesizable RAM. Default is [karnix_bootloader](src/main/c/karnix/karnix_bootloader/) which scans NOR flash for other programs and run the first one found.

2. Upload bitstream to Karnix board using ```openFPGALoader``` tool as follows:

```
openFPGALoader -f bin/KarnixSOCTopLevel_25F.bit
```

3. Connect your terminal to debug UART port /dev/ttyUSB1 (/dev/ttyU1 on FreeBSD) using 115200 baud.

4. Press RESET button and enjoy!


## Karnix SoC HAL, tests and C code examples

The following C code is currently available:

  * [karnix_soc/](src/main/c/karnix/karnix_soc/) - Hardware Abstraction Layer. A set of C headers and C code to ease hardware programming.
  * [karnix_monitor/](src/main/c/karnix/karnix_monitor/) - Monitor. A utility that allows debugging hardware and executing other programs.
  * [example/](src/main/c/karnix/example/) - An example shows how to build for for RAM and XIP.
  * [karnix_tetriscv/](src/main/c/karnix/karnix_tetriscv/) - TetRISC-V classic Tetris game.
  * [karnix_xip_test/](src/main/c/karnix/karnix_xip_test/) - Test for QSPI/XiP controller.
  * [karnix_lwip/](src/main/c/karnix/karnix_lwip/) - Adopted LiPW TCP/IP library.


## Karnix SoC address space

As a 32-bit general purpose RISC-V CPU, VexRiscV can address memory in range of 0x00000000 - 0xFFFFFFFF, that is 4GB. Within this range different types of memory and hardware I/O registers are located:

  * 0x80000000 - 0x8FFFFFFF (256 MB) is allocated to synthesizable RAM. This RAM usaully assembled of distributed BRAM blocks of an FPGA chip. By default, Karnix SoC running on Karnix board equipped with Lattice ECP5-25F FPGA chip has 72 KB or synthesizable RAM. This RAM can vary in size depending on available BRAMs which are also used by other hardware blocks. In example, disabling CGA video will release 20KB of BRAM which can be used to increase synthesizable RAM size respectively. RAM is freely readable and freely writable area working at same clock as CPU, i.e. a 32-bit word of data can be read or written in one CPU clock cycle. 

  * 0x90000000 - 0x9FFFFFFF (256 MB) is allocated to external SRAM. Karnix board has 512 KB SRAM chip K6R4016V1D mapped to this region. This is freely readable and freely writable area. Since external chip has 16-bit bus and is not as fast as CPU, accessing single 32-bit word in SRAM takes 4 clocks. Although external SRAM is much slower than RAM, it's possible to read/write and execute code in it without any drawbacks, the Data and Instruction Caches will take care of making access as fast as possible. 

  * 0xA0000000 - 0xAFFFFFFF (256 MB) is allocated to external flash memory, NAND or NOR. Karnix board comes with Winbond Q25W128 flash chip that has 16 MB of NOR falash memory accessible over serial Quad-SPI (QSPI) intrface. The first 896 KB (0xA0000000 - 0xA00DFFFF) of this NOR flash is used for storing FPGA bitstream which is getting uploaded to FPGA chip during startup (CONFIG phase) and should not be used. The rest flash memory region 128KB + 15 MB (0xA00E0000 - 0xA0FFFFFF) is accessible for read and write operations for user. It should be noted, that before writing (programming) to NOR flash, a sector to be written should be erased and WE flag has to be set (see [karnix_soc/src/qspi.h](src/main/c/karnix/karnix_soc/src/qspi.h) for corresponding CTRLs). In other words, writing to NOR flash is a bit tricky. Reading NOR flash is freely available as well are it is possible to execute code right from NOR flash (XiP feature is supported by QSPI controller). Also please note, that reading QSPI NOR flash is more than 15 times slower than RAM, yet both Data and Instruction Cache here to help. 

  * 0xF0000000 - 0xFFFFFFFF (256 MB) is allocated to hardware I/O registers. Dealing with hardware is performed by manipulating bits, reading and writing corresponding registers in this region. For more details on list of I/O registeres refer to [karnix_soc/src/soc.h](src/main/c/karnix/karnix_soc/src/soc.h). 


## Karnix SoC Software

Knowingly, almost every program running on a CPU consists of machine code, the data it works with and program stack for local (temporary) variables. The data are variables of different types, they can be unintialized, initialized with zeros or initialized with some values. Variables can be read-only (constants and/or text strings) or read-write. Variables have to occupy some space in memory and should be made addressable by CPU as well as the executing code. It's ok to say that every program can be split into a number of memory regions/segments each with its own properties and purpose. The following segmentation convention is widely used:

  * Code (or text) segment, usually named as ".text", contains executable machine code. This segment is usually read-execute-only, self-modifiable programs are out of scope here.
  * Read-only variables containing unmodifiable constants and/or text strings usually named as ".rodata" (sometimes ".rdata").
  * Readable and writable pre-initialized variables usually put in memory segment named ".data".
  * Readable and writable zero-initialized or uninitialized variables are put in memory segment named ".bss".
  * Program stack named as ".stack".
  * Program heap for dynamic memory allocation (malloc) to work, named ".heap".

Segments .bss, .stack and .heap usually do not contain any data and are initialized during startup (in crt.S) using free address space above .data. All other segments have their dedicated location in executable binary file and degicated ORIGIN - a starting address in machine mamory that is used by code to access data in given segment. It should be noted, that "location" in file and execution "ORIGIN" are usually not the same, especially for programs located and executed right from NOR flash (or from other read-only media).

Thus, a number of packing and execution schemes can take place which we will call Software Models.  

### Software Model #1 - Flat RAM binary

Let's call it "flat binary" when all the above segments constituting a program are located in same memory region (RAM), segments .text, .rodata and .data are compiled and put in a single binary file and also follow each others in order like:
  - .text
  - .rodata
  - .data
  - .bss (initialized duging startup)
  - .stack (set during startup)
  - .heap (set during startup)

Pros:
  * Fixed offset between code and data allows placing binary blob at any location in RAM/SRAM diregaring the "origin".
  * No special care should be taken to copy/reallocate .data or .stack segments. Data are always there at location binary file eas loaded at and stack is right above the data. 

Cons:
  * The whole binary blob size is limited to RAM (or SRAM) size, which imposes limitation on max program size.
  * Putting binary in RAM or SRAM without Monitor or an operating system is not easy task.

One of the ways of putting binary in SoC's RAM is to re-synthesize whole SoC from HDL source codes. During that new HEX file referenced by ```RAMPROG``` variable in [Makefile](scripts/KarnixSOC/ECP5-25F_karnix_board/Makefile) will be used to initialize synthesizable RAM, hence the binary will be integrated into resulting bitstream for FPGA.

For Lattice ECP5 FPGAs it is possible, although with some limitations, to use ```ecpbram``` tool to re-integrate (substitute) new binary HEX into existing bitstream file.


Software Model #1 is simple and easy to use if your binary fits in RAM and when you have all the HDL build tools installed and ready. It is suitable for debugging hardware or for small applications that do not allow users to change or select programs the SoC is running, i.e. you provide it as monolith bitstream assembled for given FPGA/board.

This model is implemented by using [karnix_soc/src/linker_ram.ld](src/main/c/karnix/karnix_soc/src/linker_ram.ld)

Note. Another way of loading binary into RAM is to use some kind of bootloader, monitor or even operating system. With means of [Karnix Monitor](src/main/c/karnix/karnix_monitor) it's possible to upload binary to any address in RAM using IntelHEX or ZModem protocol, or to copy it from NOR address space to RAM with ```copy``` command, then call it for execution. See details on how to use Monitor in its README file.

### Software Model #2 - Fixed address XiP binary

Within this model it's possible to put binary containing all the above segments in NOR flash memory at a fixed address, say at 0xA0100000, and execute code right from there. In this case the startup code should take care of reallocating .data segment to RAM, as well as initializing .bss and setting up stack pointer.

Pros:
  * Saves RAM by keeping constants (.rodata) in NOR flash.
  * Allows separation of program from hardware bitstream, i.e. independent upload of programs into NOR flash.
  * Allows binary with large .text segements, as big as NOR flash free space allows.
  * Allows keeping many programs in NOR flash, and select between them with some sophisticated bootloader.
  
Cons:
  * Not possible to place binary to a different location in NOR flash other than compile-time "origin".
  * Requires [karnix_bootloader](src/main/c/karnix/karnix_bootloader) pre-built into bitstream to jump to program entry point during startup. Alternatively it's possible to re-configure SoC file [KarnixSOC.scala](src/main/scala/vexriscv/demo/KarnixSOC.scala) to set ```resetVector``` to given address which not recommended.

Uploading program binary independently of bitstream is possible using stand-alone ```openFPGALoader``` tool, like below:

```$ openFPGALoader -f -o 0x100000 someprog.bin``` 

Here ```-o 0x100000``` defines offset relative to NOR flash internal address. This corresponds to 0xA0100000 memory region in Karnix SoC.

Note, that NOR flash space offset 0x0E0000 - 0x100000 is reserved for Monitor, please avoid using it for your programs unless you write your own monitor or an operating system. :)

Software Model #2 is implemented by using [karnix_soc/src/linker_xip.ld](src/main/c/karnix/karnix_soc/src/linker_xip.ld)


### Software Model #3 - Position Independent XiP binary with fixed RAM access

In theory it's possible to write code in a way that all accesses to data segments will use hard-coded addresses not tied to PC. In ARM, a special compiler option ```-mno-pic-data-is-text-relative``` serves this purpose. Unfortunately, in case of RISC-V neither GCC, nor LLVM supports this feature at the moment, hence it's impossible to write such code in any higher level programming languages. Although it is possible to write such code in assembly, both compilers will generate and optimize code that accesses data based on PC, which makes such model impractical to use. 

Pros:
  * Allows many large binaries to be put in same NOR flash at different locations disregarding their compile-time "origin".

Technically this model should be implemented by using [karnix_soc/src/linker_xip.ld](src/main/c/karnix/karnix_soc/src/linker_xip.ld), yet we have to wait till compilers get fixed.

### Software Model #4 - ELF executable

ELF executables are usually built following "flat RAM" model with a fixed program "origin" usually set at 0x00000000. Although it's technically possible to implement ELF loader in Monitor (and even in Bootloader), to fully facilitate ELF one needs an operating system with virtual memory support and that is not trivial.
 

### How to program for Karnix SoC

A README.md file from [karnix/example](src/main/c/karnix/example) has gory details on how to build ```example``` program both for RAM and XIP execution models and how to run it.

## More details on this project

Studying SpinalHDL, playing with VexRiscv and with Yosys toolchain resulted in creation of Karnix FPGA board and Karnix SOC. I published a couple of articles that describe many of the things I learnt, all in Russian language:

- [Разработка цифровой аппаратуры нетрадиционным методом: Yosys, SpinalHDL, VexRiscv ч.1](https://habr.com/ru/articles/801191/)
- [Разработка цифровой аппаратуры нетрадиционным методом: Yosys, SpinalHDL, VexRiscv ч.2](https://habr.com/ru/articles/802127/)
- [Разработка цифровой аппаратуры нетрадиционным методом: CGA видеоадаптер на SpinalHDL](https://habr.com/ru/articles/855718/)


-- 
Regards,

Ruslan Zalata

