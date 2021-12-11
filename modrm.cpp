#include "stdafx.h"
#include "cpu.h"
#include "memdescr.h"
#include "interrupts.h"

unsigned char modrm = 0;
unsigned char sib = 0;
unsigned int ofs = 0;
int modrm_isreg = 0;
int modrm_byte = 0;
int modd = 0;
int sib_modd = 0;

const char *regnames8[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
const char *regnames16[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};

extern int (*modofs16[256])();

int fetchmodrm()
{
	unsigned short ofs16;
	modd = 0;
	if (!fetch8(&modrm))
		return 0;
	modrm_isreg = 0;
	ofs16 = 0;
	if ((modrm & 0xC0) == 0xC0)
	{
		modrm_isreg = 1;
		ofs = 0;
		return 1;
	}
	return modofs16[modrm]();
}

unsigned int zero32 = 0;
unsigned int *sib_r1[8] =
{
	&r.eax, &r.ecx, &r.edx, &r.ebx, &zero32, &r.ebp, &r.esi, &r.edi
};

unsigned int sib_ea()
{
	unsigned int res = 0;
	sib_modd = 0;
	fetch8(&sib);
	res = *sib_r1[(sib >> 3) & 0x07];
	res <<= (sib >> 6);
	switch (sib & 0x07)
	{
		case 0: res += r.eax; break;
		case 1: res += r.ecx; break;
		case 2: res += r.edx; break;
		case 3: res += r.ebx; break;
		case 4: res += r.esp; sel = ssel; break;
		case 5: if ((modrm & 0xC0) == 0) { fetch32s(&sib_modd); res += sib_modd; } else { res += r.ebp; sel = ssel; } break;
		case 6: res += r.esi; break;
		case 7: res += r.edi; break;
	}
	return res;
}

extern int (*modofs32[256])();

int fetchmodrm32()
{
	modd = 0;
	if (!fetch8(&modrm))
		return 0;
	modrm_isreg = 0;
	ofs = 0;
	if ((modrm & 0xC0) == 0xC0)
	{
		modrm_isreg = 1;
		return 1;
	}
	return modofs32[modrm]();
}

int mod(int byte)
{
	modrm_byte = byte;
	if (a32)
		return fetchmodrm32();
	return fetchmodrm();
}

#if (PC)
int disasm_modrm()
{
	switch (modrm & 0xC0)
	{
		case 0:
			if (sel != &ds)
				D("%s:", sel->name);
			switch (modrm & 0x07)
			{
				case 0: D("[bx+si]"); break;
				case 1: D("[bx+di]"); break;
				case 2: D("[bp+si]"); break;
				case 3: D("[bp+di]"); break;
				case 4: D("[si]"); break;
				case 5: D("[di]"); break;
				case 6: D("[0x%.8X]", modd); break;
				case 7: D("[bx]"); break;
			}
			break;
		case 0x40:
			if (sel != &ds)
				D("%s:", sel->name);
			switch (modrm & 0x07)
			{
				case 0: D("[bx+si"); break;
				case 1: D("[bx+di"); break;
				case 2: D("[bp+si"); break;
				case 3: D("[bp+di"); break;
				case 4: D("[si"); break;
				case 5: D("[di"); break;
				case 6: D("[bp"); break;
				case 7: D("[bx"); break;
			}
			D(modd >= 0 ? "+%d]" : "-%d]", modd >= 0 ? modd : -modd);
			break;
		case 0x80:
			if (sel != &ds)
				D("%s:", sel->name);
			switch (modrm & 0x07)
			{
				case 0: D("[bx+si"); break;
				case 1: D("[bx+di"); break;
				case 2: D("[bp+si"); break;
				case 3: D("[bp+di"); break;
				case 4: D("[si"); break;
				case 5: D("[di"); break;
				case 6: D("[bp"); break;
				case 7: D("[bx"); break;
			}
			D(modd >= 0 ? "+%d]" : "-%d]", modd >= 0 ? modd : -modd);
			break;
		case 0xC0:
			if (modrm_byte)
			{
				D(regnames8[modrm & 0x07]);
			}
			else
			{
				D(i32 ? "e%s" : "%s", regnames16[modrm & 0x07]);
			}
			break;
	}
	return 1;
}

void disasm_sib_ea()
{
	if (DEBUG && dasm != NULL)
	{
		D("%s:[", sel->name);
		if ((sib & 0x07) == 0x05)
		{
			switch ((sib >> 3) & 0x07)
			{
				case 0: D("eax"); break;
				case 1: D("ecx"); break;
				case 2: D("edx"); break;
				case 3: D("ebx"); break;
				case 5: D("ebp"); break;
				case 6: D("esi"); break;
				case 7: D("edi"); break;
			}
			switch (sib >> 6)
			{
				case 1: D("*2"); break;
				case 2: D("*4"); break;
				case 3: D("*8"); break;
			}
			if ((modrm & 0xC0) == 0)
			{
				if (sib_modd != 0)
					D(sib_modd >= 0 ? "+%d" : "-%d", sib_modd >= 0 ? sib_modd : -sib_modd);
			}
			else
			{
				D("+ebp");
			}
		}
		else
		{
			switch (sib & 0x07)
			{
				case 0: D("eax"); break;
				case 1: D("ecx"); break;
				case 2: D("edx"); break;
				case 3: D("ebx"); break;
				case 4: D("esp"); break;
				case 5: if ((modrm & 0xC0) == 0) { D(sib_modd >= 0 ? "%d" : "-%d", sib_modd >= 0 ? sib_modd : -sib_modd); } else { D("+ebp"); } break;
				case 6: D("esi"); break;
				case 7: D("edi"); break;
			}
			switch ((sib >> 3) & 0x07)
			{
				case 0: D("+eax"); break;
				case 1: D("+ecx"); break;
				case 2: D("+edx"); break;
				case 3: D("+ebx"); break;
				case 5: D("+ebp"); break;
				case 6: D("+esi"); break;
				case 7: D("+edi"); break;
			}
			switch (sib >> 6)
			{
				case 1: D("*2"); break;
				case 2: D("*4"); break;
				case 3: D("*8"); break;
			}
		}
	}
}

int disasm_modrm32()
{
	switch (modrm & 0xC0)
	{
		case 0:
			switch (modrm & 0x07)
			{
				case 0: D("%s:[eax", sel->name); break;
				case 1: D("%s:[ecx", sel->name); break;
				case 2: D("%s:[edx", sel->name); break;
				case 3: D("%s:[ebx", sel->name); break;
				case 4: disasm_sib_ea(); break;
				case 5: D("%s:[0x%.8x", sel->name, modd); break;
				case 6: D("%s:[esi", sel->name); break;
				case 7: D("%s:[edi", sel->name); break;
			}
			D("]");
			break;
		case 0x40:
			switch (modrm & 0x07)
			{
				case 0: D("%s:[eax", sel->name); break;
				case 1: D("%s:[ecx", sel->name); break;
				case 2: D("%s:[edx", sel->name); break;
				case 3: D("%s:[ebx", sel->name); break;
				case 4: disasm_sib_ea(); break;
				case 5: D("%s:[ebp", sel->name); break;
				case 6: D("%s:[esi", sel->name); break;
				case 7: D("%s:[edi", sel->name); break;
			}
			if (modd != 0)
			{
				D(modd >= 0 ? "+%d]" : "-%d]", modd >= 0 ? modd : -modd);
			}
			else
			{
				D("]");
			}
			break;
		case 0x80:
			switch (modrm & 0x07)
			{
				case 0: D("%s:[eax", sel->name); break;
				case 1: D("%s:[ecx", sel->name); break;
				case 2: D("%s:[edx", sel->name); break;
				case 3: D("%s:[ebx", sel->name); break;
				case 4: disasm_sib_ea(); break;
				case 5: D("%s:[ebp", sel->name); break;
				case 6: D("%s:[esi", sel->name); break;
				case 7: D("%s:[edi", sel->name); break;
			}
			if (modd != 0)
			{
				D(modd >= 0 ? "+%d]" : "-%d]", modd >= 0 ? modd : -modd);
			}
			else
			{
				D("]");
			}
			break;
		case 0xC0:
			if (modrm_byte)
			{
				D(regnames8[modrm & 0x07]);
			}
			else
			{
				D(i32 ? "e%s" : "%s", regnames16[modrm & 0x07]);
			}
			break;
	}

	return 1;
}

void disasm_mod()
{
	if (a32)
		disasm_modrm32();
	else
		disasm_modrm();
}

void disasm_modreg()
{
	if (modrm_byte)
	{
		switch (modrm & 0x38)
		{
			case 0x00: D("al"); break;
			case 0x08: D("cl"); break;
			case 0x10: D("dl"); break;
			case 0x18: D("bl"); break;
			case 0x20: D("ah"); break;
			case 0x28: D("ch"); break;
			case 0x30: D("dh"); break;
			case 0x38: D("bh"); break;
		}
	}
	else
	{
		if (i32)
		{
			switch (modrm & 0x38)
			{
				case 0x00: D("eax"); break;
				case 0x08: D("ecx"); break;
				case 0x10: D("edx"); break;
				case 0x18: D("ebx"); break;
				case 0x20: D("esp"); break;
				case 0x28: D("ebp"); break;
				case 0x30: D("esi"); break;
				case 0x38: D("edi"); break;
			}
		}
		else
		{
			switch (modrm & 0x38)
			{
				case 0x00: D("ax"); break;
				case 0x08: D("cx"); break;
				case 0x10: D("dx"); break;
				case 0x18: D("bx"); break;
				case 0x20: D("sp"); break;
				case 0x28: D("bp"); break;
				case 0x30: D("si"); break;
				case 0x38: D("di"); break;
			}
		}
	}
}

void disasm_modsreg()
{
	switch (modrm & 0x38)
	{
		case 0x00: D("es"); break;
		case 0x08: D("cs"); break;
		case 0x10: D("ss"); break;
		case 0x18: D("ds"); break;
		case 0x20: D("fs"); break;
		case 0x28: D("gs"); break;
		case 0x30: D("?"); break;
		case 0x38: D("?"); break;
	}
}
#endif

unsigned int readmodreg()
{
	if (modrm_byte)
	{
		return r.r8[((modrm >> 3) & 3) * 4 + ((modrm >> 5) & 1)];
	}
	else
	{
		if (i32)
		{
			return r.r32[(modrm >> 3) & 7];
		}
		else
		{
			return r.r16[((modrm >> 3) & 7) * 2];
		}
	}
	return 0;
}

void writemodreg(unsigned int value)
{
	if (modrm_byte)
	{
		r.r8[((modrm >> 3) & 3) * 4 + ((modrm >> 5) & 1)] = value;
		return;
	}
	else
	{
		if (i32)
		{
			r.r32[(modrm >> 3) & 7] = value;
			return;
		}
		else
		{
			r.r16[((modrm >> 3) & 7) * 2] = value;
			return;
		}
	}
}

unsigned short readmodsreg()
{
	switch (modrm & 0x38)
	{
		case 0x00: return es.value;
		case 0x08: return cs.value;
		case 0x10: return ss.value;
		case 0x18: return ds.value;
		case 0x20: return fs.value;
		case 0x28: return gs.value;
	}
	return 0;
}

int writemodsreg(unsigned short value)
{
	switch (modrm & 0x38)
	{
		case 0x00: return set_selector(&es, value, 1);
		case 0x08: return set_selector(&cs, value, 1);
		case 0x10: return set_selector(&ss, value, 1);
		case 0x18: return set_selector(&ds, value, 1);
		case 0x20: return set_selector(&fs, value, 1);
		case 0x28: return set_selector(&gs, value, 1);
	}
	ex(EX_OPCODE);
	return 0;
}

int readmod(unsigned int *v)
{
	unsigned char b;
	unsigned short w;
	if (modrm_byte)
	{
		if (modrm_isreg)
		{
			*v = r.r8[(modrm & 3) * 4 + ((modrm >> 2) & 1)];
			return 1;
		}
		if (!read8(sel, ofs, &b))
			return 0;
		*v = b;
	}
	else
	{
		if (i32)
		{
			if (modrm_isreg)
			{
				*v = r.r32[modrm & 7];
				return 1;
			}
			if (!read32(sel, ofs, v))
				return 0;
		}
		else
		{
			if (modrm_isreg)
			{
				*v = r.r16[(modrm & 7) * 2];
				return 1;
			}
			if (!read16(sel, ofs, &w))
				return 0;
			*v = w;
		}
	}
	return 1;
}

int writemod(unsigned int value)
{
	if (modrm_byte)
	{
		if (modrm_isreg)
		{
			r.r8[(modrm & 3) * 4 + ((modrm >> 2) & 1)] = value;
			return 1;
		}
		return write8(sel, ofs, (unsigned char)value);
	}
	else
	{
		if (i32)
		{
			if (modrm_isreg)
			{
				r.r32[modrm & 7] = value;
				return 1;
			}
			return write32(sel, ofs, value);
		}
		else
		{
			if (modrm_isreg)
			{
				r.r16[(modrm & 7) * 2] = value;
				return 1;
			}
			return write16(sel, ofs, (unsigned short)value);
		}
	}
}
