#include "stdafx.h"
#include "cpu.h"
#include "memdescr.h"
#include "interrupts.h"
#include "ioports.h"
#include "pic_pit.h"
#include "keybmouse.h"

SmallBuffer keybuf;
SmallBuffer mousebuf;

int mouse_x = 0, new_mouse_x = 0;
int mouse_y = 0, new_mouse_y = 0;
int mouse_b = 0, new_mouse_b = 0;

unsigned char regs16550[8] = {0};

int scancode(int key)
{
	switch (key)
	{
		case '1': return 2;
		case '2': return 3;
		case '3': return 4;
		case '4': return 5;
		case '5': return 6;
		case '6': return 7;
		case '7': return 8;
		case '8': return 9;
		case '9': return 10;
		case '0': return 11;
		case 'A': return 0x1E;
		case 'B': return 0x30;
		case 'C': return 0x2E;
		case 'D': return 0x20;
		case 'E': return 0x12;
		case 'F': return 0x21;
		case 'G': return 0x22;
		case 'H': return 0x23;
		case 'I': return 0x17;
		case 'J': return 0x24;
		case 'K': return 0x25;
		case 'L': return 0x26;
		case 'M': return 0x32;
		case 'N': return 0x31;
		case 'O': return 0x18;
		case 'P': return 0x19;
		case 'Q': return 0x10;
		case 'R': return 0x13;
		case 'S': return 0x1F;
		case 'T': return 0x14;
		case 'U': return 0x16;
		case 'V': return 0x2F;
		case 'W': return 0x11;
		case 'X': return 0x2D;
		case 'Y': return 0x15;
		case 'Z': return 0x2C;
		case VK_SPACE: return 0x39;
		case VK_RETURN: return 0x1C;
		case VK_SUBTRACT: case VK_OEM_MINUS: return 0x0C;
		case VK_ADD: case VK_OEM_PLUS: return 0x0D;
		case VK_BACK: return 0x0E;
		case VK_ESCAPE: return 0x01;
		case VK_UP: case VK_NUMPAD8: return 0x48;
		case VK_DOWN: case VK_NUMPAD2: return 0x50;
		case VK_LEFT: case VK_NUMPAD4: return 0x4B;
		case VK_RIGHT: case VK_NUMPAD6: return 0x4D;
		case VK_SHIFT: return 0x2A;
		case VK_OEM_1: return 0x27; // ;
		case VK_OEM_2: return 0x35; // /
		case VK_OEM_3: return 0x29; // ~
		case VK_OEM_4: return 0x1A; // [
		case VK_OEM_5: return 0x2B; // \ 
		case VK_OEM_6: return 0x1B; // ]
		case VK_OEM_7: return 0x28; // '
		case VK_OEM_PERIOD: return 0x34;
		case VK_OEM_COMMA: return 0x33;
		case VK_PRIOR: case VK_NUMPAD9: return 0x49;
		case VK_NEXT: case VK_NUMPAD3: return 0x51;
		case VK_HOME: case VK_NUMPAD7: return 0x47;
		case VK_END: case VK_NUMPAD1: return 0x4F;
		case VK_MULTIPLY: return 0x37;
		case VK_DIVIDE: return 0x35;
		case VK_DELETE: case VK_DECIMAL: return 0x53;
		case VK_CONTROL: return 0x1D;
		case VK_MENU: return 0x38;
		case VK_TAB: return 0x0F;
		case VK_F1: return 0x3B;
		case VK_F2: return 0x3C;
		case VK_F3: return 0x3D;
		case VK_F4: return 0x3E;
		case VK_F5: return 0x3F;
		case VK_F6: return 0x40;
		case VK_F7: return 0x41;
		case VK_F8: return 0x42;
		case VK_F9: return 0x43;
		case VK_F10: return 0x44;
	}
	return 0;
}

void keydown(int key)
{
	key = scancode(key);
	if (key <= 0)
		return;
	keybuf.put(key);
}

void keyup(int key)
{
	key = scancode(key);
	if (key <= 0)
		return;
	key |= 0x80;
	keybuf.put(key);
}

void mousereport(int x, int y, int b)
{
	new_mouse_x = x;
	new_mouse_y = y;
	new_mouse_b = b;
}

int getkey()
{
	return keybuf.get();
}

void check_keyb()
{
	int ch;
	if (!(irqs & 2))
	{
		ch = getkey();
		if (ch > 0)
		{
			ports[0x60] = ch;
			irq(1);
		}
	}
}

void check_mouse()
{
	int mouse_speed, high, a, b, mdx, mdy;
	
	if (((mouse_x != new_mouse_x) || (mouse_y != new_mouse_y) || (mouse_b != new_mouse_b)) && (mousebuf.count() == 0))
	{
		if (new_mouse_x > 639)
			new_mouse_x = 639;
		if (new_mouse_y > 480 - 1)
			new_mouse_y = 480 - 1;

		mdx = new_mouse_x - mouse_x;
		mdy = new_mouse_y - mouse_y;
		mouse_b = new_mouse_b;

		mouse_speed = 2;
		high = 0;
		if (mdx < -mouse_speed)
			mdx = -mouse_speed;
		if (mdx > mouse_speed)
			mdx = mouse_speed;
		if (mdy < -mouse_speed)
			mdy = -mouse_speed;
		if (mdy > mouse_speed)
			mdy = mouse_speed;
		mouse_x += (char)mdx;
		mouse_y += (char)mdy;
		a = (unsigned char)(char)mdx;
		b = (unsigned char)(char)mdy;
					
#if (MOUSE_PROTOCOL == MOUSE_MS)
		high = (a >> 6) & 3;
		high |= ((b >> 6) & 3) << 2;
		mousebuf.put(0x40 | high | (mouse_b & 1 ? 0x20 : 0) | (mouse_b & 2 ? 0x10 : 0));
		mousebuf.put(a & 0x3F);
		mousebuf.put(b & 0x3F);
#else
		// Mouse Systems
		mousebuf.put(0x80 | (mouse_b & 1 ? 0 : 4) | 2 | (mouse_b & 2 ? 0 : 1));
		mdy = -mdy;
		mousebuf.put(mdx);
		mousebuf.put(mdy);
		mousebuf.put(0);
		mousebuf.put(0);
#endif
	}

	if (!(irqs & (1 << 4)))
	{
		if (mousebuf.count())
			irq(4);
	}
}

unsigned char keybmouse_portread(unsigned short port)
{
	unsigned char v;

	static unsigned char port61toggle = 0;
	port61toggle++;

	switch (port)
	{
		case 0x61:
			return 0x01 | (port61toggle & 0x30);
		case 0x62:
		case 0x63:
			return 0xFD;
		case 0x64:
			return 0x1C | (irqs & 2 ? 1 : 0);
		case 0x3f8:
			v = mousebuf.get();
			// if (mousebuf.count())
				// irq(4);
			return v;
		case 0x3fd:
			return mousebuf.count() ? 1 : 0;
		case 0x3FA:
			return (3 << 1) + (mousebuf.count() ? 0 : 1);
		case 0x3FE:
			return 0;//0xFF;
	}

	if ((port & 0x3f8) == 0x3f8)
		return regs16550[port & 7];

	return 0;
}

void keybmouse_portwrite(unsigned short port, unsigned char value)
{
	static int keyb_enabled = 1;

	switch (port)
	{
		case 0x60:
			switch (value)
			{
				case 0xad: // disable keyboard
					keyb_enabled = 0;
					irqs &= ~2;
					break;
				case 0xae: // enable keyboard
					keyb_enabled = 1;
					break;
				case 0x20: // read mode
				case 0x60: // write mode
				case 0xed: // leds
				case 0xee: // check
				case 0xf2: // id
				case 0xf3: // rate
				case 0xf4: // enable scan
				case 0xf5: // disable scan
				case 0xf6: // reset
				case 0xff: // reset
					while (keybuf.count())
						keybuf.get();
					// irq(2);
					keybuf.put(0xfa);
					break;
				case 0xAA: // test 1
					while (keybuf.count())
						keybuf.get();
					keybuf.put(0x55);
					break;
				case 0xAB: // test 2
					while (keybuf.count())
						keybuf.get();
					keybuf.put(0x00);
					break;
				default:
					while (keybuf.count())
						keybuf.get();
					// irq(2);
					keybuf.put(0xfa);
					break;
			}
			break;
		case 0x3fc:
			while (mousebuf.count())
				mousebuf.get();
#if (MOUSE_PROTOCOL == MOUSE_MS)
			mousebuf.put('M');
#endif
			break;
	}
}
