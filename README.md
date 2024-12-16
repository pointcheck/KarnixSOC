# Karnix synthesizable System-on-Chip based on VexRiscv RISC-V soft-core

This repository is an extension of [VexRiscv](https://github.com/SpinalHDL/VexRiscv) original repo. Thanks should be directed to its creator Charles Papon (Dolu1990) first. Karnix SOC is heavily based on original Briey SoC.

## SoC features

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

## Directory structure

Hence this is an extension of VexRiscv, the whole directory structure stays same as in original respository. Added code lays in:

[./src/main/scala/vexriscv/demo/KarnixSOC.scala](src/main/scala/vexriscv/demo/KarnixSOC.scala) - Karnix SOC written in SpinalHDL.
[./src//main/scala/mylib/](src/main/scala/mylib/) - Karnix SOC hardware components also written in SpinalHDL.
[./scripts/KarnixSOC/ECP5-25F_karnix_board/](scripts/KarnixSOC/ECP5-25F_karnix_board/) - Working directory containing Makefile to build Karnix SOC for Karnix board with Lattice ECP5 25F FPGA chip.
[./src/main/c/karnix/](src/main/c/karnix/) - C code with Karnix SOC Hardware Abstraction Layer, ROM monitor, Tetris game [TetRISC-V](src/main/c/karnix/tetriscv/), test hardware code and more.

Please check corresponding README.md for more details.

## Build

1. To generate and synthesize Karnix SoC go to working directory, edit Makefile to set default RAMPROG it should be built with, then issue ```make``` command like in the following:

```
cd ./scripts/KarnixSOC/ECP5-25F_karnix_board 
vi Makefile
make generate compile
```

It will consequently run:
- Build of C code for given RAMPROG
- Scala to compile HDL code written in SpinalHDL
- resulting SpinalHDL to generate Verilog code of the SoC
- Yosys/NextPNR to synthesize Verilog into bitstream 

On success the bitstream will be available in ```bin/KarnixSOCTopLevel_25F.bit```

It will include RAMPROG program stored in on-board synthesizable RAM. Default is [karnix_monitor](src/main/c/karnix/karnix_monitor/).

2. Upload bitstream to Karnix board using openFPGALoader tool as follows:

```
openFPGALoader -f bin/KarnixSOCTopLevel_25F.bit
```

3. Connect your terminal to debug UART port /dev/ttyUSB1 (/dev/ttyU1 on FreeBSD) using 115200 baud. 

4. Press RESET button and enjoy!

## More details

Studying SpinalHDL, playing with VexRiscv and with Yosys toolchain resulted in creation of Karnix FPGA board and Karnix SOC. I published a couple of articles that describe many of the things I learnt, all in Russian language:

[Разработка цифровой аппаратуры нетрадиционным методом: Yosys, SpinalHDL, VexRiscv ч.1](https://habr.com/ru/articles/801191/)
[Разработка цифровой аппаратуры нетрадиционным методом: Yosys, SpinalHDL, VexRiscv ч.2](https://habr.com/ru/articles/802127/)
[Разработка цифровой аппаратуры нетрадиционным методом: CGA видеоадаптер на SpinalHDL](https://habr.com/ru/articles/855718/)

