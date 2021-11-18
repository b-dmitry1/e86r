# e86r
Software 80486 PC emulator for Windows

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

### Compiling

Please use Visual Studio 2012 or later version

### Missing files
(will be uploaded later after some refactoring)
* disk.cpp / disk.h
* main.cpp
* VS project

### Known problems
* V86 mode work incorrectly so Windows 95 will fail to start
* Protected mode emulation incomplete. Windows 3.1, Windows 95, Linux and some pmode games will crash
* Interrupt and HDD controller incomplete. Some software will crash
* VGA virtual resolution (panning) calculation may be wrong for some games
