#include "stdafx.h"
#include "memdescr.h"
#include "cpu.h"
#include "interrupts.h"
#include "vga.h"

unsigned int ss_mask = 0xFFFFu;
unsigned int ss_inv_mask = 0xFFFF0000u;

int fetching = 0;

unsigned int *dir = (unsigned int *)0;

unsigned int gdtbase()
{
	unsigned int res;
	get_phys_addr(gdt_base, &res);
	return res;
}

unsigned int idtbase()
{
	unsigned int res;
	res = idt_base;
	return res;
}

unsigned int ldtbase()
{
	unsigned int res;
	get_phys_addr(ldt_base, &res);
	return res;
}

unsigned int lin(selector_t *s, unsigned int addr)
{
	if (!s->big)
		addr &= 0xFFFFu;
	return s->base + addr;
}

void ex1(int c, int e)
{
	ex(c, e);
}

int get_phys_addr(unsigned int addr, unsigned int *phys)
{
	unsigned int user;
	if (paging)
	{
		user = cs.dpl == 3;
		unsigned int e = dir[addr >> 22u];
		if (!(e & 1))
		{
			cr[2] = addr;
			ex1(EX_PAGE, (user ? 4 : 0));
			return 0;
		}
		
		if ((!(e & 4)) && user)
		{
			cr[2] = addr;
			ex1(EX_PAGE, 1 + 4);
			return 0;
		}
		
		unsigned int *page = (unsigned int *)&ram[e & 0xFFFFF000u];
		unsigned int pe = page[(addr >> 12u) & 0x3FF];
		if (!(pe & 1))
		{
			cr[2] = addr;
			ex1(EX_PAGE, (user ? 4 : 0));
			return 0;
		}
		
		if ((!(pe & 4)) && user)
		{
			cr[2] = addr;
			ex1(EX_PAGE, 1 + 4);
			return 0;
		}
		
		*phys = (pe & 0xFFFFF000u) | (addr & 0xFFFu);
		dir[addr >> 22u] |= 32; // accessed
		page[(addr >> 12u) & 0x3FF] |= 32;
		return 1;
	}
	*phys = addr;
	return 1;
}

int get_phys_addr_write(unsigned int addr, unsigned int *phys)
{
	unsigned int user;
	if (paging)
	{
		user = cs.dpl == 3;
		unsigned int e = dir[addr >> 22u];
		if (!(e & 1))
		{
			cr[2] = addr;
			ex1(EX_PAGE, 2 + (user ? 4 : 0));
			return 0;
		}
		
		if (!(e & 2))
		{
			//if (user)
			{
				cr[2] = addr;
				ex1(EX_PAGE, 1 + 2 + (user ? 4 : 0));
				return 0;
			}
		}
		
		if ((!(e & 4)) && user)
		{
			cr[2] = addr;
			ex1(EX_PAGE, 1 + 2 + 4);
			return 0;
		}
		
		unsigned int *page = (unsigned int *)&ram[e & 0xFFFFF000u];
		unsigned int pe = page[(addr >> 12u) & 0x3FF];
		if (!(pe & 1))
		{
			cr[2] = addr;
			ex1(EX_PAGE, 2 + (user ? 4 : 0));
			return 0;
		}
		
		if (!(pe & 2))
		{
			//if (user)
			{
				cr[2] = addr;
				ex(EX_PAGE, 1 + 2 + (user ? 4 : 0));
				return 0;
			}
		}
		
		
		if ((!(pe & 4)) && user)
		{
			cr[2] = addr;
			ex(EX_PAGE, 1 + 2 + 4);
			return 0;
		}
		
		*phys = (pe & 0xFFFFF000u) | (addr & 0xFFFu);
		dir[addr >> 22u] |= 32; // accessed
		page[(addr >> 12u) & 0x3FF] |= 32 | 64; // and dirty
		return 1;
	}
	*phys = addr;
	return 1;
}

int readphys8fast(unsigned int addr, unsigned char *v)
{
	*v = ram[addr];
	return 1;
}

int readphys16fast(unsigned int addr, unsigned short *v)
{
	*v = *(unsigned short *)&ram[addr];
	return 1;
}

int readphys32fast(unsigned int addr, unsigned int *v)
{
	*v = *(unsigned int *)&ram[addr];
	return 1;
}

int readphys8(unsigned int addr, unsigned char *v)
{
	addr &= a20mask;
	if ((addr & 0xFFFF0000) == 0xA0000)
	{
		*v = vga_memread(addr);
		return 1;
	}
	if (addr >= RAM_SIZE)
		*v = 0xff;
	else
		*v = ram[addr];
	return 1;
}

int readphys16(unsigned int addr, unsigned short *v)
{
	addr &= a20mask;
	if ((addr & 0xFFFF0000) == 0xA0000)
	{
		*v = vga_memread(addr);
		*v |= vga_memread(addr + 1) << 8u;
		return 1;
	}
	if (addr >= RAM_SIZE)
		*v = 0xffffu;
	else
		*v = *(unsigned short *)&ram[addr];
	return 1;
}

int readphys32(unsigned int addr, unsigned int *v)
{
	addr &= a20mask;
	if ((addr & 0xFFFF0000) == 0xA0000)
	{
		*v = vga_memread(addr);
		*v |= vga_memread(addr + 1) << 8u;
		*v |= vga_memread(addr + 2) << 16u;
		*v |= vga_memread(addr + 3) << 24u;
		return 1;
	}
	if (addr >= RAM_SIZE)
		*v = 0xffffffffu;
	else
		*v = *(unsigned int *)&ram[addr];
	return 1;
}

int writephys8fast(unsigned int addr, unsigned char v)
{
	addr &= a20mask;
	ram[addr] = v;
	return 1;
}

int writephys16fast(unsigned int addr, unsigned short v)
{
	addr &= a20mask;
	*(unsigned short *)&ram[addr] = v;
	return 1;
}

int writephys32fast(unsigned int addr, unsigned int v)
{
	addr &= a20mask;
	*(unsigned int *)&ram[addr] = v;
	return 1;
}

int writephys8(unsigned int addr, unsigned char v)
{
	addr &= a20mask;
	if ((addr & 0xFFFF0000) == 0xA0000)
	{
		vga_memwrite(addr, v);
		return 1;
	}
	if (addr >= RAM_SIZE)
		return 1;
	ram[addr] = v;
	return 1;
}

int writephys16(unsigned int addr, unsigned short v)
{
	addr &= a20mask;
	if ((addr & 0xFFFF0000) == 0xA0000)
	{
		vga_memwrite(addr, (unsigned char)v);
		vga_memwrite(addr + 1, v >> 8);
		return 1;
	}
	if (addr >= RAM_SIZE)
		return 1;
	*(unsigned short *)&ram[addr] = v;
	return 1;
}

int writephys32(unsigned int addr, unsigned int v)
{
	addr &= a20mask;
	if ((addr & 0xFFFF0000) == 0xA0000)
	{
		vga_memwrite(addr, v);
		vga_memwrite(addr + 1, v >> 8);
		vga_memwrite(addr + 2, v >> 16u);
		vga_memwrite(addr + 3, v >> 24u);
		return 1;
	}
	if (addr >= RAM_SIZE)
		return 1;
	*(unsigned int *)&ram[addr] = v;
	return 1;
}

int read8fast(unsigned int addr, unsigned char *v)
{
	unsigned int p;
	addr &= a20mask;
	if (!get_phys_addr(addr, &p))
		return 0;
	return readphys8fast(p, v);
}

int read16fast(unsigned int addr, unsigned short *v)
{
	unsigned int p;
	addr &= a20mask;
	if ((addr & 0xFFF) <= 0xFFE)
	{
		if (!get_phys_addr(addr, &p))
			return 0;
		if (!readphys16fast(p, v))
			return 0;
		if (DEBUG && (!fetching))
		{
			if (DEBUGMEM)
				D("\nRD 0x%.4X: %.4X", addr, *v);
		}
		return 1;
	}
	unsigned char b[2];
	if (!read8fast(addr, &b[0]))
		return 0;
	if (!read8fast(addr + 1, &b[1]))
		return 0;

	*v = b[0] + (b[1] << 8);

	if (DEBUG && (!fetching))
	{
		if (DEBUGMEM)
			D("\nRD 0x%.4X: %.4X", addr, *v);
	}

	return 1;
}

int read32fast(unsigned int addr, unsigned int *v)
{
	unsigned int p;
	addr &= a20mask;
	if ((addr & 0xFFF) <= 0xFFC)
	{
		if (!get_phys_addr(addr, &p))
			return 0;
		if (!readphys32fast(p, v))
			return 0;
		if (DEBUG && (!fetching))
		{
			if (DEBUGMEM)
				D("\nRD 0x%.4X: %.8X", addr, *v);
		}
		return 1;
	}
	unsigned char b[4];
	if (!read8fast(addr, &b[0]))
		return 0;
	if (!read8fast(addr + 1, &b[1]))
		return 0;
	if (!read8fast(addr + 2, &b[2]))
		return 0;
	if (!read8fast(addr + 3, &b[3]))
		return 0;

	*v = b[0] + (b[1] << 8) + (b[2] << 16u) + (b[3] << 24u);

	if (DEBUG && (!fetching))
	{
		if (DEBUGMEM)
			D("\nRD 0x%.4X: %.8X (page boundary)", addr, *v);
	}

	return 1;
}

int read8(unsigned int addr, unsigned char *v)
{
	if (!paging && 0)
	{
		if ((addr & 0xFFFF0000) == 0xA0000)
		{
			*v = vga_memread(addr);
			return 1;
		}
		*v = *(unsigned char *)&ram[addr];
		return 1;
	}
	unsigned int p;
	if (!get_phys_addr(addr, &p))
		return 0;
	return readphys8(p, v);
}

int read16(unsigned int addr, unsigned short *v)
{
	if (!paging && 0)
	{
		if ((addr & 0xFFFF0000) == 0xA0000)
		{
			*v = vga_memread(addr);
			*v |= vga_memread(addr + 1) << 8u;
			return 1;
		}
		*v = *(unsigned short *)&ram[addr];
		return 1;
	}
	unsigned int p;
	if ((addr & 0xFFF) <= 0xFFE)
	{
		if (!get_phys_addr(addr, &p))
			return 0;
		if (!readphys16(p, v))
			return 0;
		if (DEBUG && (!fetching))
		{
			if (DEBUGMEM)
				D("\nRD 0x%.4X: %.4X", addr, *v);
		}
		return 1;
	}
	unsigned char b[2];
	if (!read8(addr, &b[0]))
		return 0;
	if (!read8(addr + 1, &b[1]))
		return 0;

	*v = b[0] + (b[1] << 8);

	if (DEBUG && (!fetching))
	{
		if (DEBUGMEM)
			D("\nRD 0x%.4X: %.4X", addr, *v);
	}

	return 1;
}

int read32(unsigned int addr, unsigned int *v)
{
	if (!paging && 0)
	{
		if ((addr & 0xFFFF0000) == 0xA0000)
		{
			*v = vga_memread(addr);
			*v |= vga_memread(addr + 1) << 8u;
			*v |= vga_memread(addr + 2) << 16u;
			*v |= vga_memread(addr + 3) << 24u;
			return 1;
		}
		*v = *(unsigned int *)&ram[addr];
		return 1;
	}
	unsigned int p;
	if ((addr & 0xFFF) <= 0xFFC)
	{
		if (!get_phys_addr(addr, &p))
			return 0;
		if (!readphys32(p, v))
			return 0;
		if (DEBUG && (!fetching))
		{
			if (DEBUGMEM)
				D("\nRD 0x%.4X: %.8X", addr, *v);
		}
		return 1;
	}
	unsigned char b[4];
	if (!read8(addr, &b[0]))
		return 0;
	if (!read8(addr + 1, &b[1]))
		return 0;
	if (!read8(addr + 2, &b[2]))
		return 0;
	if (!read8(addr + 3, &b[3]))
		return 0;

	*v = b[0] + (b[1] << 8) + (b[2] << 16u) + (b[3] << 24u);

	if (DEBUG && (!fetching))
	{
		if (DEBUGMEM)
			D("\nRD 0x%.4X: %.8X (page boundary)", addr, *v);
	}

	return 1;
}

int write8fast(unsigned int addr, unsigned char v)
{
	unsigned int p;
	if (!get_phys_addr_write(addr, &p))
		return 0;
	return writephys8fast(p, v);
}

int write16fast(unsigned int addr, unsigned short v)
{
	if (DEBUGMEM)
		D("\nWR 0x%.4X: %.4X", addr, v);
	unsigned int p;
	if ((addr & 0xFFF) <= 0xFFE)
	{
		if (!get_phys_addr_write(addr, &p))
			return 0;
		return writephys16fast(p, v);
	}
	if (!write8fast(addr, (unsigned char)v))
		return 0;
	if (!write8fast(addr + 1, v >> 8))
		return 0;
	return 1;
}

int write32fast(unsigned int addr, unsigned int v)
{
	if (DEBUGMEM)
		D("\nWR 0x%.4X: %.8X", addr, v);
	unsigned int p;
	if ((addr & 0xFFF) <= 0xFFC)
	{
		if (!get_phys_addr_write(addr, &p))
			return 0;
		return writephys32fast(p, v);
	}
	if (!write8fast(addr, v))
		return 0;
	if (!write8fast(addr + 1, v >> 8))
		return 0;
	if (!write8fast(addr + 2, v >> 16))
		return 0;
	if (!write8fast(addr + 3, v >> 24))
		return 0;
	return 1;
}

int write8(unsigned int addr, unsigned char v)
{
	if (!paging && 0)
	{
		if ((addr & 0xFFFF0000) == 0xA0000)
		{
			vga_memwrite(addr, v);
			return 1;
		}
		*(unsigned char *)&ram[addr] = v;
		return 1;
	}
	unsigned int p;
	if (!get_phys_addr_write(addr, &p))
		return 0;
	return writephys8(p, v);
}

int write16(unsigned int addr, unsigned short v)
{
	if (!paging && 0)
	{
		if ((addr & 0xFFFF0000) == 0xA0000)
		{
			vga_memwrite(addr, (unsigned char)v);
			vga_memwrite(addr + 1, v >> 8);
			return 1;
		}
		*(unsigned short *)&ram[addr] = v;
		return 1;
	}
	if (DEBUGMEM)
		D("\nWR 0x%.4X: %.4X", addr, v);
	unsigned int p;
	if ((addr & 0xFFF) <= 0xFFE)
	{
		if (!get_phys_addr_write(addr, &p))
			return 0;
		return writephys16(p, v);
	}
	if (!write8(addr, (unsigned char)v))
		return 0;
	if (!write8(addr + 1, v >> 8))
		return 0;
	return 1;
}

int write32(unsigned int addr, unsigned int v)
{
	if (!paging && 0)
	{
		if ((addr & 0xFFFF0000) == 0xA0000)
		{
			vga_memwrite(addr, v);
			vga_memwrite(addr + 1, v >> 8);
			vga_memwrite(addr + 2, v >> 16u);
			vga_memwrite(addr + 3, v >> 24u);
			return 1;
		}
		*(unsigned int *)&ram[addr] = v;
		return 1;
	}
	if (DEBUGMEM)
		D("\nWR 0x%.4X: %.8X", addr, v);
	unsigned int p;
	if ((addr & 0xFFF) <= 0xFFC)
	{
		if (!get_phys_addr_write(addr, &p))
			return 0;
		return writephys32(p, v);
	}
	if (!write8(addr, v))
		return 0;
	if (!write8(addr + 1, v >> 8))
		return 0;
	if (!write8(addr + 2, v >> 16))
		return 0;
	if (!write8(addr + 3, v >> 24))
		return 0;
	return 1;
}

int read8(selector_t *s, unsigned int addr, unsigned char *v)
{
	if (!a32)
		;//addr &= 0xFFFFu;
	if (!s->big)
		;//addr &= 0xFFFFu;
	if (!s->present)
	{
		if (pmode)
		{
			ex(EX_SEGMENT_NOT_PRESENT, s->value);
			return 0;
		}
		*v = 0xff;
		return 1;
	}
	return read8(s->base + addr, v);
}

int read16(selector_t *s, unsigned int addr, unsigned short *v)
{
	if (!a32)
		;//addr &= 0xFFFFu;
	if (!s->big)
		;//addr &= 0xFFFFu;
	if (!s->present)
	{
		if (pmode)
		{
			ex(EX_SEGMENT_NOT_PRESENT, s->value);
			return 0;
		}
		*v = 0xffffu;
		return 1;
	}
	return read16(s->base + addr, v);
}

int read32(selector_t *s, unsigned int addr, unsigned int *v)
{
	if (!a32)
		;//addr &= 0xFFFFu;
	if (!s->big)
		;//addr &= 0xFFFFu;
	if (!s->present)
	{
		if (pmode)
		{
			ex(EX_SEGMENT_NOT_PRESENT, s->value);
			return 0;
		}
		*v = 0xffffffffu;
		return 1;
	}
	return read32(s->base + addr, v);
}

int write8(selector_t *s, unsigned int addr, unsigned char v)
{
	if (!a32)
		;//addr &= 0xFFFFu;
	if (!s->big)
		;//addr &= 0xFFFFu;
	if (!s->present)
	{
		if (pmode)
		{
			ex(EX_SEGMENT_NOT_PRESENT, s->value);
			return 0;
		}
		return 1;
	}
	return write8(s->base + addr, v);
}

int write16(selector_t *s, unsigned int addr, unsigned short v)
{
	if (!a32)
		;//addr &= 0xFFFFu;
	if (!s->big)
		;//addr &= 0xFFFFu;
	if (!s->present)
	{
		if (pmode)
		{
			ex(EX_SEGMENT_NOT_PRESENT, s->value);
			return 0;
		}
		return 1;
	}
	return write16(s->base + addr, v);
}

int write32(selector_t *s, unsigned int addr, unsigned int v)
{
	if (!a32)
		;//addr &= 0xFFFFu;
	if (!s->big)
		;//addr &= 0xFFFFu;
	if (!s->present)
	{
		if (pmode)
		{
			ex(EX_SEGMENT_NOT_PRESENT, s->value);
			return 0;
		}
		return 1;
	}
	return write32(s->base + addr, v);
}

int get_descr(descr_t *d, unsigned short value, int no_exceptions)
{
	unsigned int *a = (unsigned int *)d, p = (value & 0xFFF8);
	if (value & 0x04)
	{
		// ldt
		if (value >= ldt_limit)
		{
			if (!no_exceptions)
				ex(EX_GP, value);
			return 0;
		}
		p += ldt_base;
	}
	else
	{
		// gdt
		if (value >= gdt_limit)
		{
			if (!no_exceptions)
				ex(EX_GP, value);
			return 0;
		}
		p += gdt_base;
	}
	if (!read32fast(p, &a[0]))
		return 0;
	if (!read32fast(p + 4, &a[1]))
		return 0;
	if (d->type & 0x10)
		d->type |= 0x01;
	return 1;
}

unsigned int get_limit(descr_t *d)
{
	unsigned int res;
	res = d->limit0_15 + d->limit16_19 * 65536u;
	if (d->gran4k)
		res = res * 4096u + 4095;
	return res;
}

unsigned int get_base(descr_t *d)
{
	return d->base0_15 + d->base16_23 * 65536u + d->base24_31 * 16777216u;
}

unsigned int get_base_phys(descr_t *d)
{
	unsigned int res;
	res = d->base0_15 + d->base16_23 * 65536u + d->base24_31 * 16777216u;
	get_phys_addr(res, &res);
	return res;
}

int get_gate(gate_t *g, unsigned char n)
{
	unsigned int *a = (unsigned int *)g, p = idt_base + n * 8;
	if (n * 8u >= idt_limit)
	{
		ex(EX_GP, 0);
		return 0;
	}
	if (!read32fast(p, &a[0]))
		return 0;
	if (!read32fast(p + 4, &a[1]))
		return 0;
	return 1;
}

int set_selector(selector_t *s, unsigned short value, int all)
{
	descr_t d;
	if ((!pmode) || (r.eflags & F_VM))
	{
		s->value = value;
		s->base = value * 16;

		if (all)
		{
			s->big = 0;
			s->dpl = 3;
			s->present = 1;
			s->limit = 0xFFFFu;
			s->type = 0;
			ss_mask = 0xFFFFu;
			ss_inv_mask = ~ss_mask;
			s->mask = 0xFFFFu;
		}

		if (s == &cs)
		{
			if (r.eflags & F_VM)
				cpl = 3;
			else
				cpl = 0;
		}

		return 1;
	}

	if (value == 0)
	{
		s->value = 0;
		s->base = 0;
		s->big = 0;
		s->dpl = 0;
		s->limit = 0;
		s->present = 0;
		s->type = 0;
		s->mask = 0xFFFFFFFFu;
		return 1;
	}

	if (!get_descr(&d, value))
		return 0;

	switch (d.type)
	{
		case DESCR_DATA_DN_W:
		case DESCR_DATA_DN_W_A:
		case DESCR_DATA_UP_W:
		case DESCR_DATA_UP_W_A:
			if (s == &cs)
			{
				GP(1, value);
			}
			break;
		case DESCR_DATA_DN:
		case DESCR_DATA_DN_A:
		case DESCR_DATA_UP:
		case DESCR_DATA_UP_A:
			if (s == &ss)
			{
				GP(1, value);
			}
			if (s == &cs)
			{
				GP(1, value);
			}
			break;
		case DESCR_CODE:
		case DESCR_CODE_A:
		case DESCR_CODE_C:
		case DESCR_CODE_C_A:
		case DESCR_CODE_R:
		case DESCR_CODE_R_A:
		case DESCR_CODE_C_R:
		case DESCR_CODE_C_R_A:
			if (s == &ss)
			{
				GP(1, value);
			}
			break;
		default:
			{
				GP(1, value);
			}
			break;
	}

	s->value = value;
	s->base = get_base(&d);
	s->big = d.big;
	s->dpl = d.dpl;
	s->limit = get_limit(&d);
	s->present = d.present;
	s->type = d.type;
	s->mask = d.big ? 0xFFFFFFFu : 0xFFFFu;

	// !!!!!!!!!!!!!!!!!!!!!!!!!
	if (s == &cs)
		cpl = value & 3;//d.dpl;

	if (s == &ss)
	{
		ss_mask = s->big ? 0xFFFFFFFFu : 0xFFFFu;
		ss_inv_mask = ~ss_mask;
	}

	return 1;
}

int push16(unsigned short value)
{
	if (!paging && 0)
	{
		r.esp = ((r.esp - 2) & ss_inv_mask) | ((r.esp - 2) & ss_mask);
		*(unsigned short *)&ram[ss.base + (r.esp & ss_mask)] = value;
		return 1;
	}
	
	if (!write16fast(ss.base + ((r.esp - 2) & ss_mask), value))
		return 0;
	r.esp = ((r.esp - 2) & ss_inv_mask) | ((r.esp - 2) & ss_mask);
	return 1;
}

int push32(unsigned int value)
{
	if (!paging && 0)
	{
		r.esp = ((r.esp - 4) & ss_inv_mask) | ((r.esp - 4) & ss_mask);
		*(unsigned int *)&ram[ss.base + (r.esp & ss_mask)] = value;
		return 1;
	}
	if (!write32fast(ss.base + ((r.esp - 4) & ss_mask), value))
		return 0;
	r.esp = ((r.esp - 4) & ss_inv_mask) | ((r.esp - 4) & ss_mask);
	return 1;
}

int pop16(unsigned short *value)
{
	if (!paging && 0)
	{
		*value = *(unsigned short *)&ram[ss.base + (r.esp & ss_mask)];
		r.esp = ((r.esp + 2) & ss_inv_mask) | ((r.esp + 2) & ss_mask);
		return 1;
	}

	if (!read16fast(ss.base + (r.esp & ss_mask), value))
		return 0;
	r.esp = ((r.esp + 2) & ss_inv_mask) | ((r.esp + 2) & ss_mask);
	return 1;
}

int pop32(unsigned int *value)
{
	if (!paging && 0)
	{
		*value = *(unsigned int *)&ram[ss.base + (r.esp & ss_mask)];
		r.esp = ((r.esp + 4) & ss_inv_mask) | ((r.esp + 4) & ss_mask);
		return 1;
	}
	if (!read32fast(ss.base + (r.esp & ss_mask), value))
		return 0;
	r.esp = ((r.esp + 4) & ss_inv_mask) | ((r.esp + 4) & ss_mask);
	return 1;
}

int set_tss(unsigned short value)
{
	descr_t d;

	if (!get_descr(&d, value))
		return 0;

	if (!d.present)
	{
		ex(EX_SEGMENT_NOT_PRESENT, value);
		return 0;
	}

	switch (d.type)
	{
		case DESCR_286_TSS_AVAIL:
		case DESCR_286_TSS_BUSY:
			tss286 = 1;
			break;
		case DESCR_386_TSS_AVAIL:
		case DESCR_386_TSS_BUSY:
			tss286 = 0;
			break;
		default:
			ex(EX_GP, value);
			return 0;
	}

	tss = value;
	tssbase = get_base(&d);
	tsslimit = get_limit(&d);
	return 1;
}

int fetch8(unsigned char *b)
{
	if (!paging && 0)
	{
		*b = ram[cs.base + r.eip];
		r.eip++;
		return 1;
	}	

	if (DEBUG)
		fetching = 1;

	int res;
	if (cs.big)
	{
		res = read8fast(cs.base + r.eip, b);
		r.eip += 1;
	}
	else
	{
		res = read8(&cs, r.ip, b);
		r.ip += 1;
	}

	if (DEBUG)
		fetching = 0;

	return res;
}

int fetch16(unsigned short *b)
{
	if (!paging && 0)
	{
		*b = *(unsigned short *)&ram[cs.base + r.eip];
		r.eip += 2;
		return 1;
	}	

	if (DEBUG)
		fetching = 1;

	int res;
	if (cs.big)
	{
		res = read16fast(cs.base + r.eip, b);
		r.eip += 2;
	}
	else
	{
		res = read16(&cs, r.ip, b);
		r.ip += 2;
	}

	if (DEBUG)
		fetching = 0;

	return res;
}

int fetch32(unsigned int *b)
{
	if (!paging && 0)
	{
		*b = *(unsigned int *)&ram[cs.base + r.eip];
		r.eip += 4;
		return 1;
	}	

	if (DEBUG)
		fetching = 1;

	int res;
	if (cs.big)
	{
		res = read32fast(cs.base + r.eip, b);
		r.eip += 4;
	}
	else
	{
		res = read32(&cs, r.ip, b);
		r.ip += 4;
	}

	if (DEBUG)
		fetching = 0;

	return res;
}

int fetch8s(int *b)
{
	unsigned char a;
	if (!fetch8(&a))
		return 0;
	*b = (char)a;
	return 1;
}

int fetch16s(int *b)
{
	unsigned short a;
	if (!fetch16(&a))
		return 0;
	*b = (short)a;
	return 1;
}

int fetch32s(int *b)
{
	unsigned int a;
	if (!fetch32(&a))
		return 0;
	*b = (int)a;
	return 1;
}
