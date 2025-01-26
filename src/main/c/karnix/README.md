# Karnix SoC Software

## HAL, tests and example C code

The following C code is currently available:

  * [karnix_soc/](karnix_soc/) - Hardware Abstraction Layer. A set of C headers and C code to ease hardware programming, along with CRTs and linker scripts.
  * [karnix_bootloader/](karnix_bootloader/) - Trivial Bootloader for Karnix SoC. Embed it into synthesizable RAM of your SoC.
  * [karnix_monitor/](karnix_monitor/) - Monitor. A utility that allows debugging hardware and executing other programs. Put in NOR flash at offset **0xe0000**.
  * [example/](example/) - An example program demonstrates how to build for SoC and initialize LIBC. Builds both for RAM and XIP models.
  * [karnix_tetriscv/](karnix_tetriscv/) - TetRISC-V classic Tetris game. Builds for RAM and XIP.
  * [karnix_xip_test/](karnix_xip_test/) - Test for QSPI/XiP controller. Builds for RAM only. Put in RAM during hardware build.
  * [karnix_lwip/](karnix_lwip/) - Adopted LiPW TCP/IP library. Used by Monitor.


Please refer to [main README.md](../../../../README.md) for details on Karnix SoC memory address space and software models.

--
Regards,

Ruslan Zalata
