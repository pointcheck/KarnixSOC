# ADNS-3080 testing program for Karnix SoC

ADNS-3080 is an IC that has integrated ultra-fast speed camera sensor and a compute module that uses camera images to calculate displacement of sensor body by X and Y. This type of sensors are typically used in modern optical mice and UI navigation. This tool allows to interact with such IC sensor: init, read calculated data, as well as to retrieve raw image data and display it on CGA video.

ADNS-3080 sensor should be connected to SPI0 of the KarnixSoC using the following pinout on Karnix board:

```
"io_spi0_sclk" SITE "R5";   # SPI0 SCLK/GPIO_24
"io_spi0_miso" SITE "T4";   # SPI0 MISO/GPIO_25
"io_spi0_mosi" SITE "M5";   # SPI0 MOSI/GPIO_26
"io_spi0_ss[0]" SITE "N5";  # SPI0 CSn0/GPIO_27
"io_spi0_ss[1]" SITE "T2";  # SPI0 CSn1/GPIO_19
"io_gpio[4]" SITE "K4";     # ADNS-3080 PDN/GPIO_04
"io_gpio[5]" SITE "K5";     # ADNS-3080 RESET/GPIO_05
```

ss[0] and ss[1] allows connecting to sensors to same SPI0 interface.


## How to run binary built for XIP 

1. You will need to boot Karnix board into Monitor. Read more about [Karnix Monitor](../karnix_monitor) and how to install it.

2. Connect to debug UART of the board using terminal (minicom or cu).

3. Upload ```adns3080_test_xip.bin``` to NOR flash address at 0x100000

```
openFPGALoader -f -o 0x100000 adns3080_test_xip.bin
```

4. Now connect to Monitor using ```cu``` or ```minicom``` terminal emnulator and send ```call 0xa0100000``` command to give execution control code at NOR flash location 0x100000 where ```adns3080_test_xip.bin``` resides.

On the terminal you will see something like:

```
MONITOR[0x80000000]-> call 0xa0100000
/// call: addr = 0xa0100000, argn = 1

*** Init heap:
init_sbrk done!
heap_start: 0x80003d60, heap_end: 0x80012000, sbrk_heap_end: 0x80003d60

*** Adjusting global REENT structure:
_impure_ptr: 0x8000300c, stdout: 0x80003490

ADNS-3080 for Karnix SoC. Build 00004 on May 26 2025 at 22:21:17
Copyright (C) 2025 Fabmicro, LLC., Tyumen, Russia.

=== Hardware init ===
CGA init done
UART0 init done
TIMER0 init done
adns3080_init: divider = 30
adns3080_init: id = 0x00000017, ver = 0x000000fb
adns3080_init: ret = 0x17FB
=== Hardware init done ===
ADNS3080: motion = 00, delta_x = 0, delta_y = 0, squal = 43
ADNS3080: motion = 00, delta_x = 0, delta_y = 0, squal = 39
ADNS3080: motion = 00, delta_x = 0, delta_y = 0, squal = 41
ADNS3080: motion = 00, delta_x = 0, delta_y = 0, squal = 41
ADNS3080: motion = 00, delta_x = 0, delta_y = 0, squal = 39
ADNS3080: motion = 00, delta_x = 0, delta_y = 0, squal = 38
ADNS3080: motion = 00, delta_x = 0, delta_y = 0, squal = 40
ADNS3080: motion = 00, delta_x = 0, delta_y = 0, squal = 42
ADNS3080: motion = 00, delta_x = 0, delta_y = 0, squal = 39
ADNS3080: motion = 00, delta_x = 0, delta_y = 0, squal = 40
...
```
 
--
Regards,

Ruslan Zalata
