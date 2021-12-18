#include "stdafx.h"
#include "cpu.h"
#include "pic_pit.h"

pic_t pic;
pic_t pic2;

pit_t pit = {0, 0, 0, 0, 0, 0, 0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFFFFu};

int test_irq = 0;
int current_irq = -1;

void irq(int n)
{
	irqs |= (1 << n);
}

int get_next_irq_vector()
{
	if (current_irq >= 0)
		return -1;

	test_irq++;
	if (test_irq > 15)
		test_irq = 0;

	irqs &= (1 << 14) | (1 << 4) | (1 << 1) | (1 << 0);

	if (irqs & (1 << test_irq) & (~(pic.imr | (pic2.imr << 8))) & (~(pic.isr | (pic2.isr << 8))))
	{
		if (test_irq < 8)
		{
			if ((pic.icw[2] & 0xF8) == 0)
				return -1;
		}
		else
		{
			if ((pic2.icw[2] & 0xF8) == 0)
				return -1;
		}

		current_irq = test_irq;

		irqs &= ~(1 << current_irq);
		
		if (current_irq < 8)
		{
			pic.isr |= 1 << (current_irq & 7);
			return (pic.icw[2] & 0xF8) + (current_irq & 7);
		}
		else
		{
			pic2.isr |= 1 << (current_irq & 7);
			return (pic2.icw[2] & 0xF8) + (current_irq & 7);
		}
	}

	return -1;
}

void pit_step()
{
	if (pit.value[0] < 50)
	{
		pit.value[0] = pit.preset[0];
		irq(0);
	}
	else
		pit.value[0] -= 50;

	if (pit.value[1] < 10)
		pit.value[1] = pit.preset[1];
	else
		pit.value[1] -= 10;

	if (pit.value[2] < 10)
		pit.value[2] = pit.preset[2];
	else
		pit.value[2] -= 10;	
}

unsigned char pic_read(int port)
{
	switch (port)
	{
		case 0x20:
			if (pic.readmode == 0)
				return pic.irr;
			return pic.isr;
		case 0x21:
			return pic.imr;
		case 0xA0:
			if (pic2.readmode == 0)
				return pic2.irr;
			return pic2.isr;
		case 0xA1:
			return pic2.imr;
	}
	return 0;
}

void pic_write(int port, unsigned char value)
{
	pic_t *p = port >= 0xA0 ? &pic2 : &pic;
	unsigned int base = port >= 0xA0 ? 8 : 0;

	switch (port)
	{
		case 0x20:
		case 0xA0:
			switch (value & 0x18)
			{
				case 0x00:
					// OCW2
					switch (value >> 5)
					{
						case 0:
							// Rotate in automatic EOI mode (clear)
							break;
						case 1:
							// Non-specific EOI
							if (current_irq >= 0)
							{
								if ((current_irq & 8) == base)
								{
									p->isr &= ~(1 << ((current_irq - base) & 7));
									current_irq = -1;
								}
							}
							break;
						case 2:
							// Nop
							break;
						case 3:
							// Specific EOI command
							// irqs &= ~(1 << (base + (value & 7)));
							if (current_irq == (base + (value & 7)))
							{
								p->isr &= ~(1 << (value & 7));
								current_irq = -1;
							}
							break;
						case 4:
							// Rotate in automatic EOI mode (set)
							break;
						case 5:
							// Rotate on non-specific EOI command
							break;
						case 6:
							// Set priority command
							break;
						case 7:
							// Rotate on specific EOI command
							break;
					}
					break;
				case 0x08:
					// OCW3
					if (value & 2)
						p->readmode = value & 1;
					break;
				case 0x10:
					// ICW1
					p->icwstep = 1;
					// p->imr = 0;
					p->icw[p->icwstep++] = value;
					break;
			}
			break;
		case 0x21:
		case 0xA1:
 			switch (p->icwstep)
			{
				case 0:
					// OCW1
					p->imr = value;
					break;
				case 2:
					p->icw[p->icwstep] = value;
					switch (p->icw[1] & 3)
					{
						case 0:
							// cascade, no icw4
						case 1:
							// cascade, icw4
							p->icwstep = 3;
							break;
						case 2:
							// single, no icw4
							p->icwstep = 0;
							break;
						case 3:
							// single, icw4
							p->icwstep = 4;
							break;
					}
					break;
				case 3:
					p->icw[p->icwstep] = value;
					if (p->icw[1] & 1)
						p->icwstep = 4;
					else
						p->icwstep = 0;
					break;
				case 4:
					p->icw[p->icwstep] = value;
					p->icwstep = 0;
					break;
			}
			break;
	}
}

unsigned char pit_read(int port)
{
	switch (port)
	{
		case 0x40:
		case 0x41:
		case 0x42:
			switch (pit.access[port & 3])
			{
				case 0:
					pit.toggle[port & 3] = !pit.toggle[port & 3];
					return (unsigned char)pit.value[port & 3];
				case 1:
					return (unsigned char)pit.value[port & 3];
				case 2:
					return (unsigned char)(pit.value[port & 3] >> 8);
				case 3:
					pit.toggle[port & 3] = !pit.toggle[port & 3];
					if (pit.toggle[port & 3])
						return (unsigned char)pit.value[port & 3];
					return (pit.value[port & 3] >> 8);
			}
			break;
	}
	return 0;
}

void pit_write(int port, unsigned char value)
{
	unsigned char n;
	switch (port)
	{
		case 0x40:
			n = port & 3;
			switch (pit.access[n])
			{
				case 1:
					pit.preset[n] = (pit.preset[n] & 0xFF00) | value;
					break;
				case 2:
					pit.preset[n] = (pit.preset[n] & 0x00FF) | (value << 8);
					break;
				case 3:
					pit.toggle[n] = !pit.toggle[n];
					if (pit.toggle[n])
						pit.preset[n] = (pit.preset[n] & 0xFF00) | value;
					else
						pit.preset[n] = (pit.preset[n] & 0x00FF) | (value << 8);
					break;
			}
			if (pit.preset[n] == 0)
				pit.preset[n] = 0xFFFF;
			break;
		case 0x43:
			n = value >> 6;
			pit.access[n] = (value >> 4) & 3;
			if (pit.access[n] == 0)
				pit.toggle[n] = 0;
			break;
	}
}
