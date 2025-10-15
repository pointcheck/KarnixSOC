# Simple program to test USB 1.0 implementation 

Just compile it using `make` command, then upload XiP binary at 0xA0100000 address: 

```
$ openFPGALoader -f -o 0xA0100000 karnix_usb10_test_xip.bin
```

Connect terminal to debug serial port. If Monitor is present in QSPI Flash, you will get to
its prompt. Use `call 0xa0100000` command to call to the application. Otherwise application
will be started automatically by Bootloader.

This simple application will produce the following output if no USB device connected:

```
MONITOR[0x80000000]-> call 0xa0100000
call: addr = 0xa0100000, argn = 1
Welcome to Karnix USB 1.0 Test. Copyright (C) 2024-2025, Fabmicro, LLC.
Build #0011 at Oct 13 2025 19:02:21. Main addr: 0xa0100d58

USB1 enabled
USB bus scan: reg_sys_counter = 1000... failed with ret = -1
USB bus scan: reg_sys_counter = 2000... failed with ret = -1
USB bus scan: reg_sys_counter = 3000... failed with ret = -1
```


Soon as USB 1.0 device detected and initialized, the following messages will be shown:

```
usb10_init: Device detected: VID/PID = 0x0079/0x0006, class/subclass = 0x00/0x00, bcdUSB = 0x0100
usb10_init: Address = 1, Config wTotal = 41, Interface Class/Subclass/Proto = 3/0/0, EPAddress = 0x81, Interval = 10 ms, MaxPacket = 8
USB1: new device addr = 1, VID/PID = 0x0079/0x0006, class/subclass/proto = 3/0/0, EndpointAddress = 1, Interval = 10, MaxPower = 500 mA
USB1 (1:1) data received: 7F 7F 00 80 80 0F 00 00 , class = 3/0/0, RX_STATUS: 0x4A6E4B58, RX_STATUS2: 0x00004A6E, STATUS: 0x04000001
USB1 (1:1) data received: 7F 7F 00 80 80 0F 00 00 , class = 3/0/0, RX_STATUS: 0x4A6EC358, RX_STATUS2: 0x00004A6E, STATUS: 0x04000001
USB1 (1:1) data received: 7F 7F 00 80 80 0F 00 00 , class = 3/0/0, RX_STATUS: 0x4A6E4B58, RX_STATUS2: 0x00004A6E, STATUS: 0x04000001
...
```


# Received Data Format for Joystick (Gamepad)

The "data received" shows the 8 bytes of data received from the device. In the above example class/subclass/proto = 3/0/0 is game pad. Format of its data block is the following:

Byte[0]: Left joy X. 0x7F is middle position, 0x00 is leftmost position, 0xFF is rightmost position. Value in between is analog input. 
Byte[1]: Left Joy Y. 0x7F is middle position, 0x00 is upmost and 0xFF is downmost position.
Byte[2]: Reserved
Byte[3]: Reserved
Byte[4]: Reserved
Byte[5]: Right Joy angle: 0x0F - centered 0x1F - forward, 0x4F - backward, etc.
Byte[6]: Buttons pressed bit field. 
Byte[7]: Reserved


# Received Data Format for Mouse

Once mouse is connected, the following messages will be shown:

```
usb10_init: Device detected: VID/PID = 0x046D/0xC077, class/subclass = 0x00/0x00, bcdUSB = 0x0200
usb10_init: Address = 1, Config wTotal = 34, Interface Class/Subclass/Proto = 3/1/2, EPAddress = 0x81, Interval = 10 ms, MaxPacket = 4
USB1: new device addr = 1, VID/PID = 0x046D/0xC077, class/subclass/proto = 3/1/2, EndpointAddress = 1, Interval = 10, MaxPower = 100 mA
```

While moving mouse of pressing buttons leads to the following messages:

```
USB1 (1:1) data received: 01 E6 1C 00 , class = 3/1/2, RX_STATUS: 0x1017C338, RX_STATUS2: 0x00001017, STATUS: 0x04000001
USB1 (1:1) data received: 01 F6 0E FF , class = 3/1/2, RX_STATUS: 0x355A4B38, RX_STATUS2: 0x0000355A, STATUS: 0x04000001
USB1 (1:1) data received: 01 F4 15 00 , class = 3/1/2, RX_STATUS: 0x45B1C338, RX_STATUS2: 0x000045B1, STATUS: 0x04000001
USB1 (1:1) data received: 01 F9 0A 00 , class = 3/1/2, RX_STATUS: 0xB6284B38, RX_STATUS2: 0x0000B628, STATUS: 0x04000001
USB1 (1:1) data received: 01 FF 01 01 , class = 3/1/2, RX_STATUS: 0x470EC338, RX_STATUS2: 0x0000470E, STATUS: 0x04000001
USB1 (1:1) data received: 01 FD FE 01 , class = 3/1/2, RX_STATUS: 0x77EE4B38, RX_STATUS2: 0x000077EE, STATUS: 0x04000001
```

The data format is as follows:

Byte[0]: Buttons pressed bit field
Byte[1]: X axis displacement
Byte[2]: Y axis displacement
Byte[3]: Scroll wheel displacement


# Received Data Format for Keyboard 

When keyboard is connected and no buttons pressed, the following messages will be shown:

```
usb10_init: Device detected: VID/PID = 0x046D/0xC31C, class/subclass = 0x00/0x00, bcdUSB = 0x0110
usb10_init: Address = 1, Config wTotal = 59, Interface Class/Subclass/Proto = 3/1/1, EPAddress = 0x81, Interval = 10 ms, MaxPacket = 8
USB1: new device addr = 1, VID/PID = 0x046D/0xC31C, class/subclass/proto = 3/1/1, EndpointAddress = 1, Interval = 10, MaxPower = 90 mA
USB1 (1:1) data received: 00 00 00 00 00 00 00 00 , class = 3/1/1, RX_STATUS: 0xF4BF4B58, RX_STATUS2: 0x0000F4BF, STATUS: 0x04000001
USB1 (1:1) data received: 00 00 00 00 00 00 00 00 , class = 3/1/1, RX_STATUS: 0xF4BFC358, RX_STATUS2: 0x0000F4BF, STATUS: 0x04000001
USB1 (1:1) data received: 00 00 00 00 00 00 00 00 , class = 3/1/1, RX_STATUS: 0xF4BF4B58, RX_STATUS2: 0x0000F4BF, STATUS: 0x04000001
```

Now when buttons pressed their corresponding Usage ID codes will be put into stack of currently pressed buttons, like:

```
USB1 (1:1) data received: 00 00 04 00 00 00 00 00 , class = 3/1/1, RX_STATUS: 0x70BE4B58, RX_STATUS2: 0x000070BE, STATUS: 0x04000001
USB1 (1:1) data received: 00 00 04 00 00 00 00 00 , class = 3/1/1, RX_STATUS: 0x70BEC358, RX_STATUS2: 0x000070BE, STATUS: 0x04000001
USB1 (1:1) data received: 00 00 04 00 00 00 00 00 , class = 3/1/1, RX_STATUS: 0x70BE4B58, RX_STATUS2: 0x000070BE, STATUS: 0x04000001
USB1 (1:1) data received: 00 00 04 00 00 00 00 00 , class = 3/1/1, RX_STATUS: 0x70BEC358, RX_STATUS2: 0x000070BE, STATUS: 0x04000001
USB1 (1:1) data received: 00 00 04 05 00 00 00 00 , class = 3/1/1, RX_STATUS: 0x70724B58, RX_STATUS2: 0x00007072, STATUS: 0x04000001
USB1 (1:1) data received: 00 00 04 05 00 00 00 00 , class = 3/1/1, RX_STATUS: 0x7072C358, RX_STATUS2: 0x00007072, STATUS: 0x04000001
USB1 (1:1) data received: 00 00 04 05 00 00 00 00 , class = 3/1/1, RX_STATUS: 0x70724B58, RX_STATUS2: 0x00007072, STATUS: 0x04000001
USB1 (1:1) data received: 00 00 04 05 00 00 00 00 , class = 3/1/1, RX_STATUS: 0x7072C358, RX_STATUS2: 0x00007072, STATUS: 0x04000001
USB1 (1:1) data received: 00 00 04 05 06 00 00 00 , class = 3/1/1, RX_STATUS: 0xF8724B58, RX_STATUS2: 0x0000F872, STATUS: 0x04000001
USB1 (1:1) data received: 00 00 04 05 06 00 00 00 , class = 3/1/1, RX_STATUS: 0xF872C358, RX_STATUS2: 0x0000F872, STATUS: 0x04000001
USB1 (1:1) data received: 00 00 04 05 06 00 00 00 , class = 3/1/1, RX_STATUS: 0xF8724B58, RX_STATUS2: 0x0000F872, STATUS: 0x04000001
USB1 (1:1) data received: 00 00 04 05 06 00 00 00 , class = 3/1/1, RX_STATUS: 0xF872C358, RX_STATUS2: 0x0000F872, STATUS: 0x04000001
USB1 (1:1) data received: 00 00 04 05 06 00 00 00 , class = 3/1/1, RX_STATUS: 0xF8724B58, RX_STATUS2: 0x0000F872, STATUS: 0x04000001
USB1 (1:1) data received: 00 00 04 05 06 00 00 00 , class = 3/1/1, RX_STATUS: 0xF872C358, RX_STATUS2: 0x0000F872, STATUS: 0x04000001
USB1 (1:1) data received: 00 00 05 06 00 00 00 00 , class = 3/1/1, RX_STATUS: 0xA1374B58, RX_STATUS2: 0x0000A137, STATUS: 0x04000001
USB1 (1:1) data received: 00 00 05 06 00 00 00 00 , class = 3/1/1, RX_STATUS: 0xA137C358, RX_STATUS2: 0x0000A137, STATUS: 0x04000001
USB1 (1:1) data received: 00 00 05 06 00 00 00 00 , class = 3/1/1, RX_STATUS: 0xA1374B58, RX_STATUS2: 0x0000A137, STATUS: 0x04000001
USB1 (1:1) data received: 00 00 05 00 00 00 00 00 , class = 3/1/1, RX_STATUS: 0xA1BFC358, RX_STATUS2: 0x0000A1BF, STATUS: 0x04000001
USB1 (1:1) data received: 00 00 05 00 00 00 00 00 , class = 3/1/1, RX_STATUS: 0xA1BF4B58, RX_STATUS2: 0x0000A1BF, STATUS: 0x04000001
USB1 (1:1) data received: 00 00 00 00 00 00 00 00 , class = 3/1/1, RX_STATUS: 0xF4BFC358, RX_STATUS2: 0x0000F4BF, STATUS: 0x04000001
```

The above means that 'A' key (code 0x04) was pressed, then 'B' (code 0x05) followed and finally 'C' (code 0x06) was pressed and all three keys were kept pressed and held for a while. Once any of the keys is released its code is removed from the stack.

When any number of modifier keys are pressed, they are reflected in byte[0] bit field.

