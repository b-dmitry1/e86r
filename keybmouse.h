#pragma once

#define MOUSE_MS			1
#define MOUSE_SYSTEMS		2

#define MOUSE_PROTOCOL		MOUSE_MS

void check_keyb();
void check_mouse();

unsigned char keybmouse_portread(unsigned short port);
void keybmouse_portwrite(unsigned short port, unsigned char value);

void keydown(int key);
void keyup(int key);
void mousereport(int x, int y, int b);
