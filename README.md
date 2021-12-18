# e86r
Software 80486 IBM PC emulator for Windows and STM32 microcontrollers.

### Tested software
* MSDOS 3.3 / 4.0 / 6.22
* Windows 3.0 in real / standard mode
* Historical Linux 0.01
* Minix 2.0.x in real / 16-bit pmode / 32-bit pmode
* himem.sys
* fakecd

### Some tested games
* Many old CGA games
* Dune 2
* Warcraft 1
* DOOM 1 / 2
* Heroes of Might and Magic 1 / 2
* Transport Tycoon
* Prince of Persia
* Civilization
* Adventures of Dizzy
* Captain Dynamo
* Supaplex
* UFO (X-Com) 1
* Wings of Fury
* Wolfenstein 3D

### BIOS
Please use this compact BIOS:

https://github.com/b-dmitry1/BIOS

![e86r](https://github.com/b-dmitry1/e86r/blob/main/e86r.jpg)

### Compiling on Windows

Please use Visual Studio 2012 or later version.

### Porting

There are several WinAPI calls in "main.cpp".
They must be replaced with a target platform's equivalent functions.

Platform graphic functions:

    void hw_set_palette(unsigned char index, unsigned char r, unsigned char g, unsigned char b);
    void set_pixel_2x2(int x, int y, unsigned int color);
    void set_pixel_2x1(int x, int y, unsigned int color);
    void set_pixel_1x2(int x, int y, unsigned int color);
    void set_pixel(int x, int y, unsigned int color);

Platform disk functions:

    void hw_read_floppy(int disk, unsigned char *buffer, unsigned int lba, unsigned int count);
    void hw_write_floppy(int disk, const unsigned char *buffer, unsigned int lba, unsigned int count);
    void hw_read_hdd(int disk, unsigned char *buffer, unsigned int lba, unsigned int count);
    void hw_write_hdd(int disk, const unsigned char *buffer, unsigned int lba, unsigned int count);
    
Platform emulation error report function:

    void shutdown();

To tell an emulator that key is pressed / released or mouse is moved use this functions:

    void keydown(int xtcode);
    void keyup(int xtcode);
    void mousereport(int x, int y, int buttons);

Emulation speed for STM32F429 @ 180 MHz and STM32F746 @ 192 MHz and L1 cache enabled (746):
* Old CGA games - 30+ FPS - playable
* Dune 2 - from 3 to 11 FPS - playable
* Prince of Persia - from 15 to 20 FPS - playable
* DOOM 1 - from 0.8 to 2.3 FPS - not playable
* Transport Tycoon - from 0.1 to 0.2 FPS - not playable

If you want to speed up an emulation on STM32 try to disable MMU support in memdescr.cpp file and remove all the CPU time-consuming things like RTOS or USB.

STM32 port files published here: https://github.com/b-dmitry1/e86r/tree/main/stm32

### Using disk images
A path to your BIOS and disk image files can be set in the "loop" function located in the "main.cpp" file.

The "diskimages" folder contains "freedos.zip" archive with a clean
installation of FreeDOS 1.3 and DosMinix 2.0.4.

Just unzip this image and point e86r to it.

To boot Minix launch "c:\m.bat" file. User: "root", no password.

Image params: 504 MB, 1023 cylinders, 16 heads, 63 sectors.

The "empty.zip" file contains a clean 504 MB HDD image and a clean 1.44 MB floppy image.
You can use any PC emulator to install an OS on it.

### Booting historical Linux 0.01
You will need FDD and HDD i386 images called linuxfd.img and hd_oldlinux.img.

In the main.cpp file define a FDD and HDD images:

    fdd[0] = fopen("linuxfd.img", "rb+");
    disk_set_fdd(0, 80, 2, 18);
    
    hdd[0] = fopen("hd_oldlinux.img", "rb+");
    disk_set_hdd(0, 1023, 4, 20);
    hdd[1] = fopen("hd_oldlinux.img", "rb+");
    disk_set_hdd(1, 1023, 4, 20);

Change BIOS file name to bios_fdd.bin or tell your BIOS to boot from floppy:

    // Loading "bios.bin" (size = 8 KB) to 0xF0000 and 0xFE000
    f = fopen("bios.bin", "rb");

### Known problems
* V86 mode work incorrectly so Windows 95 will fail to start.
* Protected mode emulation is incomplete. EMM386, Windows 3.1, Windows 95, Linux and some pmode games will crash.
* VGA virtual resolution (panning) calculation may be wrong for some games.
* No FPU emulation. If you need Linux please compile it with a soft emulation of FPU.
* FreeDOS runs slower than MS-DOS, and Windows cannot use its himem.sys for some reason.

The project is provided "as is" without any warranty. Use at your own risk.

Please tell me if you find some bug or if you plan to port it to another platform.
