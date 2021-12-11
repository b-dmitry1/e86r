# e86r
Software 80486 PC emulator for Windows.

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

16-bit special version of this emulator work very well on a STM32F746 microcontroller board
and this version should work too after some changes made related to input and output.
Emulation speed is enough for old games on STM32 @ 216 MHz.

### Using disk images
A path to your BIOS and disk image files can be set in the "loop" function located in the "main.cpp" file.

The "diskimages" folder contains "freedos.zip" archive with a clean
installation of FreeDOS 1.3 and DosMinix 2.0.4.

Just unzip this image and point e86r to it.

To boot Minix launch "c:\m.bat" file. User: "root", no password.

Image params: 504 MB, 1023 cylinders, 16 heads, 63 sectors.

The "empty.zip" file contains a clean 504 MB HDD image and a clean 1.44 MB floppy image.
You can use any PC emulator to install an OS on it.

### Known problems
* V86 mode work incorrectly so Windows 95 will fail to start.
* Protected mode emulation is incomplete. EMM386, Windows 3.1, Windows 95, Linux and some pmode games will crash.
* Interrupt and HDD controllers are incomplete. Some software will crash.
* VGA virtual resolution (panning) calculation may be wrong for some games.
* No FPU emulation. If you need Linux please compile it with a soft emulation of FPU.
* FreeDOS runs slower than MS-DOS, and Windows cannot use its himem.sys for some reason.

The project is provided "as is" without any warranty. Use at your own risk.

Please tell me if you find some bug or if you plan to port it to another platform.
