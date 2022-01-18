# STM32F746 port of e86r, an opensource 80486 IBM PC emulator

![schematics](/pictures/e86r746.png)

### Compiling

The project is configured for:

* Keil μVision 5
* arm-none-eabi-g++

Compile script "1.bat" for Windows GNU compiler is included. To use it just fix a path to arm-none-eabi-g++.exe and STM32 Cube Programmer in this file.

If you plan to compile on a Linux machine you can easily convert "1.bat" to "1.sh". Or create a simple makefile.

### Disk image

You can use Rufus tool to write an image on SD-card.

With a disk write function enabled please do not use a cheap SD-cards with a low write resource! The project has only been tested with Transcend, Samsung, and Kingston memory cards.

Maximum supported SD-card size is 32 GB.

Maximum supported HDD image size is 503 MB.

### Adding USB support

There are 2 USB host ports in a STM32F746 series microcontroller.
You can use 1 of them for keyboard and 1 for mouse.

Please recreate / modify project file in Keil μVision IDE or use a third-party libraries to add RTOS support and USB functionality.

USB keyboard report functions:

    void keydown(int xtcode);
    void keyup(int xtcode);

USB mouse report functions:

    void mousereport(int x, int y, int buttons);

### Using WaveShare's Core7XXI board

Instead of creating a custom PCB you can use WaveShare's Core7XXI board.

In this case you'll need to add SD-card adapter, VGA connector, wires and at least 9 resistors like shown on a schematics drawing.

__Please note that original Core7XXI board is shipped with only 8 megabytes of SDRAM installed!__

The emulator is configured to be used with a 32 megabyte SDRAM chip.
You should replace 8 MB chip with a 32 MB one and reconfigure BIOS, SDRAM controller, and framebuffer locations in "board.h" and "main.cpp" files
if you want to use only 8 MB of SDRAM. Only 6 MB of extended memory will be available.

![Core 7XX board](/pictures/Core7XX.jpg)

### Known problems
* Slow emulation of protected mode.
* Same problems as in a PC version of e86r.

### Disclaimer

The project is provided "as is" without any warranty. Use at your own risk.

Please tell me if you find some bug or if you plan to port it to another platform.
