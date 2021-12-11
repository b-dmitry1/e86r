#ifndef VGA_H
#define VGA_H

#define SCREEN_WIDTH		640
#define SCREEN_HEIGHT		480

extern int vmode;

void update_screen();

void set_pixel_2x2(int x, int y, unsigned int color);
void set_pixel_2x1(int x, int y, unsigned int color);
void set_pixel_1x2(int x, int y, unsigned int color);
void set_pixel(int x, int y, unsigned int color);

void vga_portwrite(unsigned short port, unsigned char value);
unsigned char vga_portread(unsigned short port);
unsigned char vga_memread(unsigned int addr);
void vga_memwrite(unsigned int addr, unsigned char value);

#endif
