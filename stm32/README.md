# STM32F746 port of e86r, an opensource 80486 IBM PC emulator

![schematics](e86r746.png)

### Compiling

Keil μVision 5 is needed to compile the project.

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
