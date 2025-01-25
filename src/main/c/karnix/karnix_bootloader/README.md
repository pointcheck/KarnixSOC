# Karnix SoC Bootloader

A simple bootloader that is getting embedded into synthesizable RAM during hardware build and what gets control on CPU power-up. It checks Magic word (0x12300013) at given hardcoded offsets in NOR flash. It long-jumps to the first one found. If nothing found, it just reports to UART0 and hangs (8N1, 115200). That's all what it does.

Later we may add some more functionality to this bootloader, like selecting particular address to jump to, or reading second-stage bootloader from SDIO or USB flash.

Karnix SoC Bootloader is implemented using Software model #1, for that it uses ```karnix_soc/src/crt_ram.S``` and its own linker script that is basically slightly modified ```karnix_soc/src/linker_ram.ld```. It requires and fits into only 4KB of RAM beginning at 0x80000000.
 
## How to embed Bootloader into SoC

### Easiest way:

Go to hardware build working directory and edit ```Makefile``` there. Set or uncomment macro at the top of the file ```RAMPROG = karnix_bootloader``` then rebuild the whole thing with  ```make generate compile```.

### For pros:

First, compile bootloader from this directory using ```make```. You will get the following files in your ```./build``` directory:

```
-rw-rw-r-- 1 rz rz  2736 Jan 25 00:16 karnix_bootloader.hexx
-rw-rw-r-- 1 rz rz 15124 Jan 25 00:16 karnix_bootloader_ram.asm
-rwxrwxr-x 1 rz rz  1216 Jan 25 00:16 karnix_bootloader_ram.bin*
-rwxrwxr-x 1 rz rz 12964 Jan 25 00:16 karnix_bootloader_ram.elf*
-rw-rw-r-- 1 rz rz  3497 Jan 25 00:16 karnix_bootloader_ram.hex
-rw-rw-r-- 1 rz rz  6652 Jan 25 00:16 karnix_bootloader_ram.map
-rwxrwxr-x 1 rz rz  3848 Jan 25 00:16 karnix_bootloader_ram.v*
```

If you are using Verilog, there's ```karnix_bootloader_ram.v``` for you to include into your Verilog code.

Also there's ```karnix_bootloader.hexx``` file which can be used to initialize synthesizable RAM with the following syntax

For Verilog:
```
 initial begin
    $readmemh("src/main/c/karnix/karnix_bootloader/build/karnix_bootloader.hexx", mem);
  end
```

assuming that ```mem``` register array is 32 bits wide.


For SpinalHDL:
```
    val ram = Axi4SharedOnChipRam(
      dataWidth = 32,
      byteCount = onChipRamSize,
      idWidth = 4
    )
    HexTools.initRam(ram.ram, "src/main/c/karnix/karnix_bootloader/build/karnix_bootloader.hexx", 0x80000000l)
```

Please take a look in Karnix SoC HDL file ```./src/main/scala/vexriscv/demo/KarnixSOC.scala```

CAUTION! Please do not confuse **.hexx** and .hex files. The .hexx is a trivial hexadecimal text format, while .hex is quite complex Intel HEX. For ```HexTools.initRam()``` you need the first one! 


--
Regards,

Ruslan Zalata

 
