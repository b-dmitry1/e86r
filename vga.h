#ifndef VGA_H
#define VGA_H

#include "stdafx.h"

#define SCREEN_WIDTH		640
#define SCREEN_HEIGHT		480

extern int vmode;

void update_screen(HDC hdc);

void vga_portwrite(unsigned short port, unsigned char value);
unsigned char vga_portread(unsigned short port);
unsigned char vga_memread(unsigned int addr);
void vga_memwrite(unsigned int addr, unsigned char value);

#endif
