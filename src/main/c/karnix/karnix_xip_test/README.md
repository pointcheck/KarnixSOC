# Test of NOR flash memory interface in Karnix SOC

## Brief overview

Karnix board is equipped with Winbond W25Q128 flash memory chip with QSPI interface. To interact with that, Karnix SOC has a QSPI controller with bearst reads, writes and erase of flash memory regions. The NOR flash is mapped to 0xA0000000 address space which allows direct execution of code from flash (XiP).

This directory contains a simple test program that does:
- erases 8MB of upper flash memory region
- fills it with NOP (ADDI x0, x0, 0) instructions
- jumps to 0xA0800000 address to execute NOPs right from NOR flash memory. 

It provides output with some benchmarks onto UART0. Connect to debug port /dev/ttyUSB1 (or /dev/ttyU1 on FreeBSD) on Karnix board with your terminal to follow the process.

Note, that erase and write operations to NOR flash take LOTs of time, hence device should be polled for BUSY flash before each write or erase. Please check into karnix_soc/qspi.h for available CTRLs.

## Testing results 

On the terminal you will see something like this:

```
...
Erasing NOR sector at: 0xa0fea000 (4074)... Done! NOR Status Reg 1 = 0x0000002c
Erasing NOR sector at: 0xa0feb000 (4075)... Done! NOR Status Reg 1 = 0x0000002c
Erasing NOR sector at: 0xa0fec000 (4076)... Done! NOR Status Reg 1 = 0x0000002c
Erasing NOR sector at: 0xa0fed000 (4077)... Done! NOR Status Reg 1 = 0x0000002c
Erasing NOR sector at: 0xa0fee000 (4078)... Done! NOR Status Reg 1 = 0x0000002c
Erasing NOR sector at: 0xa0fef000 (4079)... Done! NOR Status Reg 1 = 0x0000002c
NOR erase of 8323072 bytes took 125328059 us
Programming NOR at: 0xa0800000, size: 0x007f0000
NOR write of 8323072 bytes took 57933174 us
Checking what has been written... Matched. sum0 = 0x82ddc06f, sum1 = 0x82ddc06f
Running XiP test: jump to 0xa0800000 (data: 0x00000013)
NOR test executed jump to 0xa0800000, time = 585219
NOR test executed jump to 0xa0800000, time = 585218
NOR test executed jump to 0xa0800000, time = 585219
NOR test executed jump to 0xa0800000, time = 585219
...
```

The above means excuting 8323072/4 = 2080768 instructions takes 585219 uS, in other words 3555537 ops per second. This is quite slow, but bearable. :)

Erasing + Programming of 8323072 bytes took 183261233 uS, which is 45416 bytes/sec. And that is really SLOW.

