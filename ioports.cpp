#include "stdafx.h"
#include "cpu.h"
#include "memdescr.h"
#include "interrupts.h"
#include "vga.h"
#include "disk.h"
#include "ioports.h"
#include "pic_pit.h"
#include "cmos.h"
#include "keybmouse.h"

unsigned char ports[65536];

unsigned char portread8(unsigned short port)
{
	GP((cr[0] & 1) && (cpl > IOPL), 0);

	// VGA goes first to speed up games
	if ((port >= 0x3B0) && (port <= 0x3DF))
		return vga_portread(port);

	// IDE HDD
	if (((port >= 0x1F0) && (port <= 0x1F7)) || ((port >= 0x3F0) && (port <= 0x3F7)))
		return ide_read(port);

	switch (port)
	{
		case 0x20:
		case 0x21:
		case 0xA0:
		case 0xA1:
			return pic_read(port);
		case 0x40:
		case 0x41:
		case 0x42:
		case 0x43:
			return pit_read(port);
		case 0x61:
		case 0x62:
		case 0x63:
		case 0x64:
		case 0x3f8:
		case 0x3fd:
		case 0x3FA:
		case 0x3FE:
			return keybmouse_portread(port);
		case 0x70:
		case 0x71:
			return cmos_read(port);
		case 0x92:
			// A20
			return a20 ? 2 : 0;
		case 0x201:
			// Joystick
			return 0xFF;
		case 0x2f8:
			return 0xFF;
	}

	return ports[port];
}

unsigned short portread16(unsigned short port)
{
	GP((cr[0] & 1) && (cpl > IOPL), 0);

	unsigned short res;
	res = portread8(port);
	res |= portread8(port + 1) << 8u;
	return res;
}

unsigned int portread32(unsigned short port)
{
	GP((cr[0] & 1) && (cpl > IOPL), 0);

	unsigned short res;
	res = portread8(port);
	res |= portread8(port + 1) << 8u;
	res |= portread8(port + 2) << 16u;
	res |= portread8(port + 3) << 24u;
	return res;
}

void portwrite8(unsigned short port, unsigned char v)
{
	GPV((cr[0] & 1) && (cpl > IOPL), 0);

	if ((port >= 0x3B0) && (port <= 0x3DF))
	{
		vga_portwrite(port, v);
		return;
	}
	if (((port >= 0x1F0) && (port <= 0x1F7)) || ((port >= 0x3F0) && (port <= 0x3F7)))
	{
		ide_write(port, v);
		return;
	}
	ports[port] = v;
	switch (port)
	{
		case 0x20:
		case 0x21:
		case 0xA0:
		case 0xA1:
			pic_write(port, v);
			break;
		case 0x40:
		case 0x41:
		case 0x42:
		case 0x43:
			pit_write(port, v);
			break;
		case 0x60:
		case 0x3fc:
			keybmouse_portwrite(port, v);
			break;
		case 0x70:
		case 0x71:
			cmos_write(port, v);
			break;
		case 0x92:
			a20 = (v & 0x02) != 0;
			a20mask = a20 ? 0xFFFFFFFFu : 0xFFFFFu;
			break;
		case 0xBE:
			vmode = v;
			break;
	}
}

void portwrite16(unsigned short port, unsigned short v)
{
	GPV((cr[0] & 1) && (cpl > IOPL), 0);

	portwrite8(port, (unsigned char)v);
	portwrite8(port + 1, v >> 8u);
}

void portwrite32(unsigned short port, unsigned int v)
{
	GPV((cr[0] & 1) && (cpl > IOPL), 0);

	portwrite8(port, (unsigned char)v);
	portwrite8(port + 1, v >> 8u);
	portwrite8(port + 2, v >> 16u);
	portwrite8(port + 3, v >> 24u);
}
