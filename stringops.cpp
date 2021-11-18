#include "stdafx.h"
#include "cpu.h"
#include "memdescr.h"
#include "alu.h"
#include "ioports.h"

int movsb()
{
	unsigned char v;
	if (a32)
	{
		if (!read8(sel, r.esi, &v))
			return 0;
		if (!write8(&es, r.edi, v))
			return 0;
		if (r.eflags & F_D)
		{
			r.esi -= sizeof(v);
			r.edi -= sizeof(v);
		}
		else
		{
			r.esi += sizeof(v);
			r.edi += sizeof(v);
		}
	}
	else
	{
		if (!read8(sel, r.si, &v))
			return 0;
		if (!write8(&es, r.di, v))
			return 0;
		if (r.eflags & F_D)
		{
			r.si -= sizeof(v);
			r.di -= sizeof(v);
		}
		else
		{
			r.si += sizeof(v);
			r.di += sizeof(v);
		}
	}
	return 1;
}

int movsw()
{
	unsigned short v;
	if (a32)
	{
		if (!read16(sel, r.esi, &v))
			return 0;
		if (!write16(&es, r.edi, v))
			return 0;
		if (r.eflags & F_D)
		{
			r.esi -= sizeof(v);
			r.edi -= sizeof(v);
		}
		else
		{
			r.esi += sizeof(v);
			r.edi += sizeof(v);
		}
	}
	else
	{
		if (!read16(sel, r.si, &v))
			return 0;
		if (!write16(&es, r.di, v))
			return 0;
		if (r.eflags & F_D)
		{
			r.si -= sizeof(v);
			r.di -= sizeof(v);
		}
		else
		{
			r.si += sizeof(v);
			r.di += sizeof(v);
		}
	}
	return 1;
}

int movsd()
{
	unsigned int v;
	if (a32)
	{
		if (!read32(sel, r.esi, &v))
			return 0;
		if (!write32(&es, r.edi, v))
			return 0;
		if (r.eflags & F_D)
		{
			r.esi -= sizeof(v);
			r.edi -= sizeof(v);
		}
		else
		{
			r.esi += sizeof(v);
			r.edi += sizeof(v);
		}
	}
	else
	{
		if (!read32(sel, r.si, &v))
			return 0;
		if (!write32(&es, r.di, v))
			return 0;
		if (r.eflags & F_D)
		{
			r.si -= sizeof(v);
			r.di -= sizeof(v);
		}
		else
		{
			r.si += sizeof(v);
			r.di += sizeof(v);
		}
	}
	return 1;
}

int stosb()
{
	unsigned char v = r.al;
	if (a32)
	{
		if (!write8(&es, r.edi, v))
			return 0;
		if (r.eflags & F_D)
		{
			r.edi -= sizeof(v);
		}
		else
		{
			r.edi += sizeof(v);
		}
	}
	else
	{
		if (!write8(&es, r.di, v))
			return 0;
		if (r.eflags & F_D)
		{
			r.di -= sizeof(v);
		}
		else
		{
			r.di += sizeof(v);
		}
	}
	return 1;
}

int stosw()
{
	unsigned short v = r.ax;
	if (a32)
	{
		if (!write16(&es, r.edi, v))
			return 0;
		if (r.eflags & F_D)
		{
			r.edi -= sizeof(v);
		}
		else
		{
			r.edi += sizeof(v);
		}
	}
	else
	{
		writephys16(es.base + r.di, v);
		if (!write16(&es, r.di, v))
			return 0;
		if (r.eflags & F_D)
		{
			r.di -= sizeof(v);
		}
		else
		{
			r.di += sizeof(v);
		}
	}
	return 1;
}

int stosd()
{
	unsigned int v = r.eax;
	if (a32)
	{
		if (!write32(&es, r.edi, v))
			return 0;
		if (r.eflags & F_D)
		{
			r.edi -= sizeof(v);
		}
		else
		{
			r.edi += sizeof(v);
		}
	}
	else
	{
		if (!write32(&es, r.di, v))
			return 0;
		if (r.eflags & F_D)
		{
			r.di -= sizeof(v);
		}
		else
		{
			r.di += sizeof(v);
		}
	}
	return 1;
}

int cmpsb()
{
	unsigned char v, s;
	if (a32)
	{
		if (!read8(sel, r.esi, &v))
			return 0;
		if (!read8(&es, r.edi, &s))
			return 0;
		sub8(v, s, 0);
		if (r.eflags & F_D)
		{
			r.esi -= sizeof(v);
			r.edi -= sizeof(v);
		}
		else
		{
			r.esi += sizeof(v);
			r.edi += sizeof(v);
		}
	}
	else
	{
		if (!read8(sel, r.si, &v))
			return 0;
		if (!read8(&es, r.di, &s))
			return 0;
		sub8(v, s, 0);
		if (r.eflags & F_D)
		{
			r.si -= sizeof(v);
			r.di -= sizeof(v);
		}
		else
		{
			r.si += sizeof(v);
			r.di += sizeof(v);
		}
	}
	return 1;
}

int cmpsw()
{
	unsigned short v, s;
	if (a32)
	{
		if (!read16(sel, r.esi, &v))
			return 0;
		if (!read16(&es, r.edi, &s))
			return 0;
		sub16(v, s, 0);
		if (r.eflags & F_D)
		{
			r.esi -= sizeof(v);
			r.edi -= sizeof(v);
		}
		else
		{
			r.esi += sizeof(v);
			r.edi += sizeof(v);
		}
	}
	else
	{
		if (!read16(sel, r.si, &v))
			return 0;
		if (!read16(&es, r.di, &s))
			return 0;
		sub16(v, s, 0);
		if (r.eflags & F_D)
		{
			r.si -= sizeof(v);
			r.di -= sizeof(v);
		}
		else
		{
			r.si += sizeof(v);
			r.di += sizeof(v);
		}
	}
	return 1;
}

int cmpsd()
{
	unsigned int v, s;
	if (a32)
	{
		if (!read32(sel, r.esi, &v))
			return 0;
		if (!read32(&es, r.edi, &s))
			return 0;
		sub32(v, s, 0);
		if (r.eflags & F_D)
		{
			r.esi -= sizeof(v);
			r.edi -= sizeof(v);
		}
		else
		{
			r.esi += sizeof(v);
			r.edi += sizeof(v);
		}
	}
	else
	{
		if (!read32(sel, r.si, &v))
			return 0;
		if (!read32(&es, r.di, &s))
			return 0;
		sub32(v, s, 0);
		if (r.eflags & F_D)
		{
			r.si -= sizeof(v);
			r.di -= sizeof(v);
		}
		else
		{
			r.si += sizeof(v);
			r.di += sizeof(v);
		}
	}
	return 1;
}

int scasb()
{
	unsigned char v;
	if (a32)
	{
		if (!read8(&es, r.edi, &v))
			return 0;
		sub8(r.al, v, 0);
		if (r.eflags & F_D)
		{
			r.edi -= sizeof(v);
		}
		else
		{
			r.edi += sizeof(v);
		}
	}
	else
	{
		if (!read8(&es, r.di, &v))
			return 0;
		sub8(r.al, v, 0);
		if (r.eflags & F_D)
		{
			r.di -= sizeof(v);
		}
		else
		{
			r.di += sizeof(v);
		}
	}
	return 1;
}

int scasw()
{
	unsigned short v;
	if (a32)
	{
		if (!read16(&es, r.edi, &v))
			return 0;
		sub16(r.ax, v, 0);
		if (r.eflags & F_D)
		{
			r.edi -= sizeof(v);
		}
		else
		{
			r.edi += sizeof(v);
		}
	}
	else
	{
		if (!read16(&es, r.di, &v))
			return 0;
		sub16(r.ax, v, 0);
		if (r.eflags & F_D)
		{
			r.di -= sizeof(v);
		}
		else
		{
			r.di += sizeof(v);
		}
	}
	return 1;
}

int scasd()
{
	unsigned int v;
	if (a32)
	{
		if (!read32(&es, r.edi, &v))
			return 0;
		sub32(r.eax, v, 0);
		if (r.eflags & F_D)
		{
			r.edi -= sizeof(v);
		}
		else
		{
			r.edi += sizeof(v);
		}
	}
	else
	{
		if (!read32(&es, r.di, &v))
			return 0;
		sub32(r.eax, v, 0);
		if (r.eflags & F_D)
		{
			r.di -= sizeof(v);
		}
		else
		{
			r.di += sizeof(v);
		}
	}
	return 1;
}

int lodsb()
{
	unsigned char v;
	if (a32)
	{
		if (!read8(sel, r.esi, &v))
			return 0;
		r.al = v;
		if (r.eflags & F_D)
		{
			r.esi -= sizeof(v);
		}
		else
		{
			r.esi += sizeof(v);
		}
	}
	else
	{
		if (!read8(sel, r.si, &v))
			return 0;
		r.al = v;
		if (r.eflags & F_D)
		{
			r.si -= sizeof(v);
		}
		else
		{
			r.si += sizeof(v);
		}
	}
	return 1;
}

int lodsw()
{
	unsigned short v;
	if (a32)
	{
		if (!read16(sel, r.esi, &v))
			return 0;
		r.ax = v;
		if (r.eflags & F_D)
		{
			r.esi -= sizeof(v);
		}
		else
		{
			r.esi += sizeof(v);
		}
	}
	else
	{
		if (!read16(sel, r.si, &v))
			return 0;
		r.ax = v;
		if (r.eflags & F_D)
		{
			r.si -= sizeof(v);
		}
		else
		{
			r.si += sizeof(v);
		}
	}
	return 1;
}

int lodsd()
{
	unsigned int v;
	if (a32)
	{
		if (!read32(sel, r.esi, &v))
			return 0;
		r.eax = v;
		if (r.eflags & F_D)
		{
			r.esi -= sizeof(v);
		}
		else
		{
			r.esi += sizeof(v);
		}
	}
	else
	{
		if (!read32(sel, r.si, &v))
			return 0;
		r.eax = v;
		if (r.eflags & F_D)
		{
			r.si -= sizeof(v);
		}
		else
		{
			r.si += sizeof(v);
		}
	}
	return 1;
}

int outsb()
{
	unsigned char v;
	if (a32)
	{
		if (!read8(sel, r.esi, &v))
			return 0;
		portwrite8(r.dx, v);
		if (r.eflags & F_D)
		{
			r.esi -= sizeof(v);
		}
		else
		{
			r.esi += sizeof(v);
		}
	}
	else
	{
		if (!read8(sel, r.si, &v))
			return 0;
		portwrite8(r.dx, v);
		if (r.eflags & F_D)
		{
			r.si -= sizeof(v);
		}
		else
		{
			r.si += sizeof(v);
		}
	}
	return 1;
}

int outsw()
{
	unsigned short v;
	if (a32)
	{
		if (!read16(sel, r.esi, &v))
			return 0;
		portwrite16(r.dx, v);
		if (r.eflags & F_D)
		{
			r.esi -= sizeof(v);
		}
		else
		{
			r.esi += sizeof(v);
		}
	}
	else
	{
		if (!read16(sel, r.si, &v))
			return 0;
		portwrite16(r.dx, v);
		if (r.eflags & F_D)
		{
			r.si -= sizeof(v);
		}
		else
		{
			r.si += sizeof(v);
		}
	}
	return 1;
}

int outsd()
{
	unsigned int v;
	if (a32)
	{
		if (!read32(sel, r.esi, &v))
			return 0;
		portwrite32(r.dx, v);
		if (r.eflags & F_D)
		{
			r.esi -= sizeof(v);
		}
		else
		{
			r.esi += sizeof(v);
		}
	}
	else
	{
		if (!read32(sel, r.si, &v))
			return 0;
		portwrite32(r.dx, v);
		if (r.eflags & F_D)
		{
			r.si -= sizeof(v);
		}
		else
		{
			r.si += sizeof(v);
		}
	}
	return 1;
}

int insb()
{
	unsigned char v;
	if (a32)
	{
		v = portread8(r.dx);
		if (!write8(&es, r.edi, v))
			return 0;
		if (r.eflags & F_D)
		{
			r.edi -= sizeof(v);
		}
		else
		{
			r.edi += sizeof(v);
		}
	}
	else
	{
		v = portread8(r.dx);
		if (!write8(&es, r.di, v))
			return 0;
		if (r.eflags & F_D)
		{
			r.di -= sizeof(v);
		}
		else
		{
			r.di += sizeof(v);
		}
	}
	return 1;
}

int insw()
{
	unsigned short v;
	if (a32)
	{
		v = portread16(r.dx);
		if (!write16(&es, r.edi, v))
			return 0;
		if (r.eflags & F_D)
		{
			r.edi -= sizeof(v);
		}
		else
		{
			r.edi += sizeof(v);
		}
	}
	else
	{
		v = portread16(r.dx);
		if (!write16(&es, r.di, v))
			return 0;
		if (r.eflags & F_D)
		{
			r.di -= sizeof(v);
		}
		else
		{
			r.di += sizeof(v);
		}
	}
	return 1;
}

int insd()
{
	unsigned int v;
	if (a32)
	{
		v = portread32(r.dx);
		if (!write32(&es, r.edi, v))
			return 0;
		if (r.eflags & F_D)
		{
			r.edi -= sizeof(v);
		}
		else
		{
			r.edi += sizeof(v);
		}
	}
	else
	{
		v = portread32(r.dx);
		if (!write32(&es, r.di, v))
			return 0;
		if (r.eflags & F_D)
		{
			r.di -= sizeof(v);
		}
		else
		{
			r.di += sizeof(v);
		}
	}
	return 1;
}
