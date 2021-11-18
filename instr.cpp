#include "stdafx.h"
#include "cpu.h"
#include "interrupts.h"
#include "memdescr.h"
#include "transfer.h"
#include "modrm.h"
#include "alu.h"
#include "stringops.h"
#include "ioports.h"
#include "instr_0F.h"
#include "instr32_0F.h"
#include "disk.h"

extern void (*instrs[256])();
int repne = 0;
int repe = 0;

extern int cycle;

void i_00()
{
	unsigned int d, s;
	D("add ");
	if (!mod(1)) return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	if (!readmod(&d)) return;
	s = readmodreg();
	d = add8(d, s, 0);
	writemod(d);
	
}

void i_01()
{
	unsigned int d, s;
	D("add ");
	if (!mod(0)) return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	if (!readmod(&d)) return;
	s = readmodreg();
	if (i32)
		d = add32(d, s, 0);
	else
		d = add16(d, s, 0);
	writemod(d);
	
}

void i_02()
{
	unsigned int d, s;
	D("add ");
	if (!mod(1)) return;
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (!readmod(&s)) return;
	d = readmodreg();
	d = add8(d, s, 0);
	writemodreg(d);
	
}

void i_03()
{
	unsigned int d, s;
	D("add ");
	if (!mod(0)) return;
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (!readmod(&s)) return;
	d = readmodreg();
	if (i32)
		d = add32(d, s, 0);
	else
		d = add16(d, s, 0);
	writemodreg(d);
	
}

void i_04()
{
	unsigned char b;
	D("add al, ");
	if (!fetch8(&b))
		return;
	D("0x%x", b);
	r.al = add8(r.al, b, 0);
	
}

void i_05()
{
	unsigned short w;
	unsigned int d;
	if (i32)
	{
		D("add eax, ");
		if (!fetch32(&d))
			return;
		D("0x%x", d);
		r.eax = add32(r.eax, d, 0);
	}
	else
	{
		D("add ax, ");
		if (!fetch16(&w))
			return;
		D("0x%x", w);
		r.ax = add16(r.ax, w, 0);
	}
	
}

void i_06()
{
	D("push es");
	if (i32)
		push32(es.value);
	else
		push16(es.value);
}

void i_07()
{
	unsigned short w;
	unsigned int d;
	D("pop es");
	if (i32)
	{
		if (!pop32(&d))
			return;
		set_selector(&es, d, 1);
	}
	else
	{
		if (!pop16(&w))
			return;
		set_selector(&es, w, 1);
	}
}

void i_08()
{
	unsigned int d, s;
	D("or ");
	if (!mod(1)) return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	if (!readmod(&d)) return;
	s = readmodreg();
	d = or8(d, s);
	writemod(d);
	
}

void i_09()
{
	unsigned int d, s;
	D("or ");
	if (!mod(0)) return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	if (!readmod(&d)) return;
	s = readmodreg();
	if (i32)
		d = or32(d, s);
	else
		d = or16(d, s);
	writemod(d);
	
}

void i_0A()
{
	unsigned int d, s;
	D("or ");
	if (!mod(1)) return;
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (!readmod(&s)) return;
	d = readmodreg();
	d = or8(d, s);
	writemodreg(d);
	
}

void i_0B()
{
	unsigned int d, s;
	D("or ");
	if (!mod(0)) return;
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (!readmod(&s)) return;
	d = readmodreg();
	if (i32)
		d = or32(d, s);
	else
		d = or16(d, s);
	writemodreg(d);
	
}

void i_0C()
{
	unsigned char b;
	D("or al, ");
	if (!fetch8(&b))
		return;
	D("0x%x", b);
	r.al = or8(r.al, b);
	
}

void i_0D()
{
	unsigned short w;
	unsigned int d;
	if (i32)
	{
		D("or eax, ");
		if (!fetch32(&d))
			return;
		D("0x%x", d);
		r.eax = or32(r.eax, d);
	}
	else
	{
		D("or ax, ");
		if (!fetch16(&w))
			return;
		D("0x%x", w);
		r.ax = or16(r.ax, w);
	}
	
}

void i_0E()
{
	D("push cs");
	if (i32)
		push32(cs.value);
	else
		push16(cs.value);
}

extern int instr_count[512];

void i_0F()
{
	if (!fetch8(&opcode_0F))
		return;
	if (DEBUG)
		instr_count[opcode_0F + 0x100]++;
	if (i32)
		instrs32_0F[opcode_0F]();
	else
		instrs_0F[opcode_0F]();
}

void i_10()
{
	unsigned int d, s;
	D("adc ");
	if (!mod(1)) return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	if (!readmod(&d)) return;
	s = readmodreg();
	d = add8(d, s, r.eflags & F_C);
	writemod(d);
	
}

void i_11()
{
	unsigned int d, s;
	D("adc ");
	if (!mod(0)) return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	if (!readmod(&d)) return;
	s = readmodreg();
	if (i32)
		d = add32(d, s, r.eflags & F_C);
	else
		d = add16(d, s, r.eflags & F_C);
	writemod(d);
	
}

void i_12()
{
	unsigned int d, s;
	D("adc ");
	if (!mod(1)) return;
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (!readmod(&s)) return;
	d = readmodreg();
	d = add8(d, s, r.eflags & F_C);
	writemodreg(d);
	
}

void i_13()
{
	unsigned int d, s;
	D("adc ");
	if (!mod(0)) return;
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (!readmod(&s)) return;
	d = readmodreg();
	if (i32)
		d = add32(d, s, r.eflags & F_C);
	else
		d = add16(d, s, r.eflags & F_C);
	writemodreg(d);
	
}

void i_14()
{
	unsigned char b;
	D("adc al, ");
	if (!fetch8(&b))
		return;
	D("0x%x", b);
	r.al = add8(r.al, b, r.eflags & F_C);
	
}

void i_15()
{
	unsigned short w;
	unsigned int d;
	if (i32)
	{
		D("adc eax, ");
		if (!fetch32(&d))
			return;
		D("0x%x", d);
		r.eax = add32(r.eax, d, r.eflags & F_C);
	}
	else
	{
		D("adc ax, ");
		if (!fetch16(&w))
			return;
		D("0x%x", w);
		r.ax = add16(r.ax, w, r.eflags & F_C);
	}
	
}

void i_16()
{
	D("push ss");
	if (i32)
		push32(ss.value);
	else
		push16(ss.value);
}

void i_17()
{
	unsigned short w;
	unsigned int d;
	D("pop ss");
	if (i32)
	{
		if (!pop32(&d))
			return;
		set_selector(&ss, d, 1);
	}
	else
	{
		if (!pop16(&w))
			return;
		set_selector(&ss, w, 1);
	}
}

void i_18()
{
	unsigned int d, s;
	D("sbb ");
	if (!mod(1)) return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	if (!readmod(&d)) return;
	s = readmodreg();
	d = sub8(d, s, r.eflags & F_C);
	writemod(d);
	
}

void i_19()
{
	unsigned int d, s;
	D("sbb ");
	if (!mod(0)) return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	if (!readmod(&d)) return;
	s = readmodreg();
	if (i32)
		d = sub32(d, s, r.eflags & F_C);
	else
		d = sub16(d, s, r.eflags & F_C);
	writemod(d);
	
}

void i_1A()
{
	unsigned int d, s;
	D("sbb ");
	if (!mod(1)) return;
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (!readmod(&s)) return;
	d = readmodreg();
	d = sub8(d, s, r.eflags & F_C);
	writemodreg(d);
	
}

void i_1B()
{
	unsigned int d, s;
	D("sbb ");
	if (!mod(0)) return;
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (!readmod(&s)) return;
	d = readmodreg();
	if (i32)
		d = sub32(d, s, r.eflags & F_C);
	else
		d = sub16(d, s, r.eflags & F_C);
	writemodreg(d);
	
}

void i_1C()
{
	unsigned char b;
	D("sbb al, ");
	if (!fetch8(&b))
		return;
	D("0x%x", b);
	r.al = sub8(r.al, b, r.eflags & F_C);
	
}

void i_1D()
{
	unsigned short w;
	unsigned int d;
	if (i32)
	{
		D("sbb eax, ");
		if (!fetch32(&d))
			return;
		D("0x%x", d);
		r.eax = sub32(r.eax, d, r.eflags & F_C);
	}
	else
	{
		D("sbb ax, ");
		if (!fetch16(&w))
			return;
		D("0x%x", w);
		r.ax = sub16(r.ax, w, r.eflags & F_C);
	}
	
}

void i_1E()
{
	D("push ds");
	if (i32)
		push32(ds.value);
	else
		push16(ds.value);
}

void i_1F()
{
	unsigned short w;
	unsigned int d;
	D("pop ds");

	if (i32)
	{
		if (!pop32(&d))
			return;
		set_selector(&ds, d, 1);
	}
	else
	{
		if (!pop16(&w))
			return;
		set_selector(&ds, w, 1);
	}
}

void i_20()
{
	unsigned int d, s;
	D("and ");
	if (!mod(1)) return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	if (!readmod(&d)) return;
	s = readmodreg();
	d = and8(d, s);
	writemod(d);
	
}

void i_21()
{
	unsigned int d, s;
	D("and ");
	if (!mod(0)) return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	if (!readmod(&d)) return;
	s = readmodreg();
	if (i32)
		d = and32(d, s);
	else
		d = and16(d, s);
	writemod(d);
	
}

void i_22()
{
	unsigned int d, s;
	D("and ");
	if (!mod(1)) return;
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (!readmod(&s)) return;
	d = readmodreg();
	d = and8(d, s);
	writemodreg(d);
	
}

void i_23()
{
	unsigned int d, s;
	D("and ");
	if (!mod(0)) return;
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (!readmod(&s)) return;
	d = readmodreg();
	if (i32)
		d = and32(d, s);
	else
		d = and16(d, s);
	writemodreg(d);
	
}

void i_24()
{
	unsigned char b;
	D("and al, ");
	if (!fetch8(&b))
		return;
	D("0x%x", b);
	r.al = and8(r.al, b);
	
}

void i_25()
{
	unsigned short w;
	unsigned int d;
	if (i32)
	{
		D("and eax, ");
		if (!fetch32(&d))
			return;
		D("0x%x", d);
		r.eax = and32(r.eax, d);
	}
	else
	{
		D("and ax, ");
		if (!fetch16(&w))
			return;
		D("0x%x", w);
		r.ax = and16(r.ax, w);
	}
	
}

void i_26()
{
	sel = &es;
	ssel = &es;
	if (!fetch8(&opcode))
		return;
	instrs[opcode]();
}

void i_27()
{
	// daa - ok
	D("daa");
	if (((r.al & 0x0F) > 9) || (r.flags & F_A))
	{
		if ((r.al + 6u) > 255u)
			r.flags |= F_C;
		r.al += 6;
		r.flags |= F_A;
	}
	if ((r.al > 0x9F) || (r.flags & F_C))
	{
		r.al += 0x60;
		r.flags |= F_C;
	}
	setpsz8(r.al);
}

void i_28()
{
	unsigned int d, s;
	D("sub ");
	if (!mod(1)) return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	if (!readmod(&d)) return;
	s = readmodreg();
	d = sub8(d, s, 0);
	writemod(d);
	
}

void i_29()
{
	unsigned int d, s;
	D("sub ");
	if (!mod(0)) return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	if (!readmod(&d)) return;
	s = readmodreg();
	if (i32)
		d = sub32(d, s, 0);
	else
		d = sub16(d, s, 0);
	writemod(d);
	
}

void i_2A()
{
	unsigned int d, s;
	D("sub ");
	if (!mod(1)) return;
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (!readmod(&s)) return;
	d = readmodreg();
	d = sub8(d, s, 0);
	writemodreg(d);
	
}

void i_2B()
{
	unsigned int d, s;
	D("sub ");
	if (!mod(0)) return;
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (!readmod(&s)) return;
	d = readmodreg();
	if (i32)
		d = sub32(d, s, 0);
	else
		d = sub16(d, s, 0);
	writemodreg(d);
	
}

void i_2C()
{
	unsigned char b;
	D("sub al, ");
	if (!fetch8(&b))
		return;
	D("0x%x", b);
	r.al = sub8(r.al, b, 0);
	
}

void i_2D()
{
	unsigned short w;
	unsigned int d;
	if (i32)
	{
		D("sub eax, ");
		if (!fetch32(&d))
			return;
		D("0x%x", d);
		r.eax = sub32(r.eax, d, 0);
	}
	else
	{
		D("sub ax, ");
		if (!fetch16(&w))
			return;
		D("0x%x", w);
		r.ax = sub16(r.ax, w, 0);
	}
	
}

void i_2E()
{
	sel = &cs;
	ssel = &cs;
	if (!fetch8(&opcode))
		return;
	instrs[opcode]();
}

void i_2F()
{
	D("das");
	if (((r.al & 0x0F) > 9) || (r.flags & F_A))
	{
		if (r.al < 6)
			r.flags |= F_C;
		r.al -= 6;
		r.flags |= F_A;
	}
	if ((r.al > 0x9F) || (r.flags & F_C))
	{
		r.al -= 0x60;
		r.flags |= F_C;
	}
	setpsz8(r.al);
}

void i_30()
{
	unsigned int d, s;
	D("xor ");
	if (!mod(1)) return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	if (!readmod(&d)) return;
	s = readmodreg();
	d = xor8(d, s);
	writemod(d);
	
}

void i_31()
{
	unsigned int d, s;
	D("xor ");
	if (!mod(0)) return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	if (!readmod(&d)) return;
	s = readmodreg();
	if (i32)
		d = xor32(d, s);
	else
		d = xor16(d, s);
	writemod(d);
	
}

void i_32()
{
	unsigned int d, s;
	D("xor ");
	if (!mod(1)) return;
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (!readmod(&s)) return;
	d = readmodreg();
	d = xor8(d, s);
	writemodreg(d);
	
}

void i_33()
{
	unsigned int d, s;
	D("xor ");
	if (!mod(0)) return;
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (!readmod(&s)) return;
	d = readmodreg();
	if (i32)
		d = xor32(d, s);
	else
		d = xor16(d, s);
	writemodreg(d);
	
}

void i_34()
{
	unsigned char b;
	D("xor al, ");
	if (!fetch8(&b))
		return;
	D("0x%x", b);
	r.al = xor8(r.al, b);
	
}

void i_35()
{
	unsigned short w;
	unsigned int d;
	if (i32)
	{
		D("xor eax, ");
		if (!fetch32(&d))
			return;
		D("0x%x", d);
		r.eax = xor32(r.eax, d);
	}
	else
	{
		D("xor ax, ");
		if (!fetch16(&w))
			return;
		D("0x%x", w);
		r.ax = xor16(r.ax, w);
	}
	
}

void i_36()
{
	sel = &ss;
	ssel = &ss;
	if (!fetch8(&opcode))
		return;
	instrs[opcode]();
}

void i_37()
{
	D("aaa");
	if (((r.al & 0x0F) > 9) || (r.flags & F_A))
	{
		r.al += 6;
		r.ah++;
		r.flags |= F_A | F_C;
	} else
		r.flags &= ~(F_A | F_C);
	r.al &= 0x0F;
}

void i_38()
{
	unsigned int d, s;
	D("cmp ");
	if (!mod(1)) return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	if (!readmod(&d)) return;
	s = readmodreg();
	sub8(d, s, 0);
	
}

void i_39()
{
	unsigned int d, s;
	D("cmp ");
	if (!mod(0)) return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	if (!readmod(&d)) return;
	s = readmodreg();
	if (i32)
		sub32(d, s, 0);
	else
		sub16(d, s, 0);
}

void i_3A()
{
	unsigned int d, s;
	D("cmp ");
	if (!mod(1)) return;
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (!readmod(&s)) return;
	d = readmodreg();
	sub8(d, s, 0);
	
}

void i_3B()
{
	unsigned int d, s;
	D("cmp ");
	if (!mod(0)) return;
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (!readmod(&s)) return;
	d = readmodreg();
	if (i32)
		sub32(d, s, 0);
	else
		sub16(d, s, 0);
}

void i_3C()
{
	unsigned char b;
	D("cmp al, ");
	if (!fetch8(&b))
		return;
	D("0x%x", b);
	sub8(r.al, b, 0);
	
}

void i_3D()
{
	unsigned short w;
	unsigned int d;
	if (i32)
	{
		D("cmp eax, ");
		if (!fetch32(&d))
			return;
		D("0x%x", d);
		sub32(r.eax, d, 0);
	}
	else
	{
		D("cmp ax, ");
		if (!fetch16(&w))
			return;
		D("0x%x", w);
		sub16(r.ax, w, 0);
	}
	
}

void i_3E()
{
	sel = &ds;
	ssel = &ds;
	if (!fetch8(&opcode))
		return;
	instrs[opcode]();
}

void i_3F()
{
	D("aas");
	if (((r.al & 0x0F) > 9) || (r.flags & F_A))
	{
		r.al -= 6;
		r.ah--;
		r.flags |= F_A | F_C;
	} else
		r.flags &= ~(F_A | F_C);
	r.al &= 0x0F;
}

void i_40()
{
	if (i32)
	{
		D("inc eax");
		r.eax = inc32(r.eax);
	}
	else
	{
		D("inc ax");
		r.ax = inc16(r.ax);
	}
}

void i_41()
{
	if (i32)
	{
		D("inc ecx");
		r.ecx = inc32(r.ecx);
	}
	else
	{
		D("inc cx");
		r.cx = inc16(r.cx);
	}
}

void i_42()
{
	if (i32)
	{
		D("inc edx");
		r.edx = inc32(r.edx);
	}
	else
	{
		D("inc dx");
		r.dx = inc16(r.dx);
	}
}

void i_43()
{
	if (i32)
	{
		D("inc ebx");
		r.ebx = inc32(r.ebx);
	}
	else
	{
		D("inc bx");
		r.bx = inc16(r.bx);
	}
}

void i_44()
{
	if (i32)
	{
		D("inc esp");
		r.esp = inc32(r.esp);
	}
	else
	{
		D("inc sp");
		r.sp = inc16(r.sp);
	}
}

void i_45()
{
	if (i32)
	{
		D("inc ebp");
		r.ebp = inc32(r.ebp);
	}
	else
	{
		D("inc bp");
		r.bp = inc16(r.bp);
	}
}

void i_46()
{
	if (i32)
	{
		D("inc esi");
		r.esi = inc32(r.esi);
	}
	else
	{
		D("inc si");
		r.si = inc16(r.si);
	}
}

void i_47()
{
	if (i32)
	{
		D("inc edi");
		r.edi = inc32(r.edi);
	}
	else
	{
		D("inc di");
		r.di = inc16(r.di);
	}
}

void i_48()
{
	if (i32)
	{
		D("dec eax");
		r.eax = dec32(r.eax);
	}
	else
	{
		D("dec ax");
		r.ax = dec16(r.ax);
	}
}

void i_49()
{
	if (i32)
	{
		D("dec ecx");
		r.ecx = dec32(r.ecx);
	}
	else
	{
		D("dec cx");
		r.cx = dec16(r.cx);
	}
}

void i_4A()
{
	if (i32)
	{
		D("dec edx");
		r.edx = dec32(r.edx);
	}
	else
	{
		D("dec dx");
		r.dx = dec16(r.dx);
	}
}

void i_4B()
{
	if (i32)
	{
		D("dec ebx");
		r.ebx = dec32(r.ebx);
	}
	else
	{
		D("dec bx");
		r.bx = dec16(r.bx);
	}
}

void i_4C()
{
	if (i32)
	{
		D("dec esp");
		r.esp = dec32(r.esp);
	}
	else
	{
		D("dec sp");
		r.sp = dec16(r.sp);
	}
}

void i_4D()
{
	if (i32)
	{
		D("dec ebp");
		r.ebp = dec32(r.ebp);
	}
	else
	{
		D("dec bp");
		r.bp = dec16(r.bp);
	}
}

void i_4E()
{
	if (i32)
	{
		D("dec esi");
		r.esi = dec32(r.esi);
	}
	else
	{
		D("dec si");
		r.si = dec16(r.si);
	}
}

void i_4F()
{
	if (i32)
	{
		D("dec edi");
		r.edi = dec32(r.edi);
	}
	else
	{
		D("dec di");
		r.di = dec16(r.di);
	}
}

void i_50()
{
	if (i32)
	{
		D("push eax");
		push32(r.eax);
	}
	else
	{
		D("push ax");
		push16(r.ax);
	}
}

void i_51()
{
	if (i32)
	{
		D("push ecx");
		push32(r.ecx);
	}
	else
	{
		D("push cx");
		push16(r.cx);
	}
}

void i_52()
{
	if (i32)
	{
		D("push edx");
		push32(r.edx);
	}
	else
	{
		D("push dx");
		push16(r.dx);
	}
}

void i_53()
{
	if (i32)
	{
		D("push ebx");
		push32(r.ebx);
	}
	else
	{
		D("push bx");
		push16(r.bx);
	}
}

void i_54()
{
	if (i32)
	{
		D("push esp");
		push32(r.esp);
	}
	else
	{
		D("push sp");
#ifdef DETECT86
		push16(r.sp - 2);
#else
		push16(r.sp);
#endif
	}
}

void i_55()
{
	if (i32)
	{
		D("push ebp");
		push32(r.ebp);
	}
	else
	{
		D("push bp");
		push16(r.bp);
	}
}

void i_56()
{
	if (i32)
	{
		D("push esi");
		push32(r.esi);
	}
	else
	{
		D("push si");
		push16(r.si);
	}
}

void i_57()
{
	if (i32)
	{
		D("push edi");
		push32(r.edi);
	}
	else
	{
		D("push di");
		push16(r.di);
	}
}

void i_58()
{
	if (i32)
	{
		D("pop eax");
		pop32(&r.eax);
	}
	else
	{
		D("pop ax");
		pop16(&r.ax);
	}
}

void i_59()
{
	if (i32)
	{
		D("pop ecx");
		pop32(&r.ecx);
	}
	else
	{
		D("pop cx");
		pop16(&r.cx);
	}
}

void i_5A()
{
	if (i32)
	{
		D("pop edx");
		pop32(&r.edx);
	}
	else
	{
		D("pop dx");
		pop16(&r.dx);
	}
}

void i_5B()
{
	if (i32)
	{
		D("pop ebx");
		pop32(&r.ebx);
	}
	else
	{
		D("pop bx");
		pop16(&r.bx);
	}
}

void i_5C()
{
	unsigned short w;
	unsigned int d;
	if (i32)
	{
		D("pop esp");
		if (pop32(&d))
			r.esp = d;
	}
	else
	{
		D("pop sp");
		if (pop16(&w))
			r.sp = w;
	}
}

void i_5D()
{
	if (i32)
	{
		D("pop ebp");
		pop32(&r.ebp);
	}
	else
	{
		D("pop bp");
		pop16(&r.bp);
	}
}

void i_5E()
{
	if (i32)
	{
		D("pop esi");
		pop32(&r.esi);
	}
	else
	{
		D("pop si");
		pop16(&r.si);
	}
}

void i_5F()
{
	if (i32)
	{
		D("pop edi");
		pop32(&r.edi);
	}
	else
	{
		D("pop di");
		pop16(&r.di);
	}
}

void i_60()
{
	if (i32)
	{
		D("pushad");
		unsigned int old_esp;
		old_esp = r.esp;
		push32(r.eax);
		push32(r.ecx);
		push32(r.edx);
		push32(r.ebx);
		push32(old_esp);
		push32(r.ebp);
		push32(r.esi);
		push32(r.edi);
	}
	else
	{
		D("pusha");
		unsigned int old_sp;
		old_sp = r.sp;
		push16(r.ax);
		push16(r.cx);
		push16(r.dx);
		push16(r.bx);
		push16(old_sp);
		push16(r.bp);
		push16(r.si);
		push16(r.di);
	}
}

void i_61()
{
	if (i32)
	{
		D("popad");
		pop32(&r.edi);
		pop32(&r.esi);
		pop32(&r.ebp);
		r.esp += 4;
		pop32(&r.ebx);
		pop32(&r.edx);
		pop32(&r.ecx);
		pop32(&r.eax);
	}
	else
	{
		D("popa");
		pop16(&r.di);
		pop16(&r.si);
		pop16(&r.bp);
		r.esp += 2;
		pop16(&r.bx);
		pop16(&r.dx);
		pop16(&r.cx);
		pop16(&r.ax);
	}
}

void i_62()
{
	unsigned int min, max, value;
	int smin, smax, svalue;

	if (!mod(0))
		return;

	D("bound ");
	disasm_mod();
	D(", ");
	disasm_modreg();

	if (!readmod(&min))
		return;

	ofs += i32 ? 4 : 2;

	if (!readmod(&max))
		return;

	value = readmodreg();

	if (i32)
	{
		svalue = value;
		smin = min;
		smax = max;
	}
	else
	{
		svalue = (short)value;
		smin = (short)min;
		smax = (short)max;
	}

	if ((svalue < smin) || (svalue > smax))
		ex(EX_BOUND);
}

void i_63()
{
	unsigned int dest, src;

	if (!mod(0))
		return;

	D("arpl ");
	disasm_mod();
	D(", ");
	disasm_modreg();

	if ((modrm & 0xC0) != 0xC0)
	{
		ex(EX_OPCODE);
		return;
	}

	if (!readmod(&dest))
		return;

	src = readmodreg();

	dest &= 0xFFFFu;
	src &= 0xFFFFu;

	if ((dest & 3) < (src & 3))
	{
		dest = (dest & 0xFFFC) | (src & 3);
		writemod(dest);
		r.eflags |= F_Z;
	}
	else
	{
		r.eflags &= ~F_Z;
	}
}

void i_64()
{
	// D("fs: ");
	sel = &fs;
	ssel = &fs;
	if (!fetch8(&opcode))
		return;
	instrs[opcode]();
}

void i_65()
{
	// D("gs: ");
	sel = &gs;
	ssel = &gs;
	if (!fetch8(&opcode))
		return;
	instrs[opcode]();
}

void i_66()
{
	i32 = !cs.big;
	if (!fetch8(&opcode))
		return;
	instrs[opcode]();
}

void i_67()
{
	a32 = !cs.big;
	if (!fetch8(&opcode))
		return;
	instrs[opcode]();
}

void i_68()
{
	if (i32)
	{
		D("push ");
		unsigned int d;
		if (!fetch32(&d))
			return;
		D("0x%x", d);
		push32(d);
	}
	else
	{
		D("push ");
		unsigned short w;
		if (!fetch16(&w))
			return;
		D("0x%x", w);
		push16(w);
	}
}

void i_69()
{
	unsigned short a, b;
	unsigned int a32, b32;
	unsigned int res;
	if (!mod(0))
		return;
	D("imul ");
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (i32)
	{
		if (!readmod(&a32))
			return;
		if (!fetch32(&b32))
			return;
		D(", 0x%x", b32);
		res = (unsigned int)imul32(a32, b32);
		writemodreg(res);
	}
	else
	{
		if (!readmod(&a32))
			return;
		a = (unsigned short)a32;
		if (!fetch16(&b))
			return;
		D(", 0x%x", b);
		res = imul16(a, b);
		writemodreg(res);
	}
}

void i_6A()
{
	unsigned char b;
	unsigned int d;
	D("push ");
	if (!fetch8(&b))
		return;
	d = b;
	if (d & 0x80)
		d |= 0xFFFFFF00u;
	if (i32)
	{
		D("0x%x", d);
		push32(d);
	}
	else
	{
		d &= 0xFFFFu;
		D("0x%x", d);
		push16(d);
	}
}

void i_6B()
{
	unsigned char b8;
	unsigned short a, b;
	unsigned int a32, b32;
	unsigned int res;
	if (!mod(0))
		return;
	D("imul ");
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (!readmod(&a32))
		return;
	if (!fetch8(&b8))
		return;
	D(", %d", b8);
	if (i32)
	{
		b32 = b8;
		if (b32 & 0x80)
			b32 |= 0xFFFFFF00u;
		res = (unsigned int)imul32(a32, b32);
		writemodreg(res);
	}
	else
	{
		a = (unsigned short)a32;
		b = b8;
		if (b & 0x80)
			b |= 0xFF00u;
		res = imul16(a, b);
		writemodreg(res);
	}
}

void i_6C()
{
	D("insb");
	if (repe | repne)
	{
		if (a32)
		{
			while (r.ecx)
			{
				if (!insb())
					return;
				r.ecx--;
			}
		}
		else
		{
			while (r.cx)
			{
				if (!insb())
					return;
				r.cx--;
			}
		}
	}
	else
		insb();
}

void i_6D()
{
	if (i32)
	{
		D("insd");
		if (repe | repne)
		{
			if (a32)
			{
				while (r.ecx)
				{
					if (!insd())
						return;
					r.ecx--;
				}
			}
			else
			{
				while (r.cx)
				{
					if (!insd())
						return;
					r.cx--;
				}
			}
		}
		else
			insd();
	}
	else
	{
		D("insw");
		if (repe | repne)
		{
			if (a32)
			{
				while (r.ecx)
				{
					if (!insw())
						return;
					r.ecx--;
				}
			}
			else
			{
				while (r.cx)
				{
					if (!insw())
						return;
					r.cx--;
				}
			}
		}
		else
			insw();
	}
}

void i_6E()
{
	D("outsb");
	if (repe | repne)
	{
		if (a32)
		{
			while (r.ecx)
			{
				if (!outsb())
					return;
				r.ecx--;
			}
		}
		else
		{
			while (r.cx)
			{
				if (!outsb())
					return;
				r.cx--;
			}
		}
	}
	else
		outsb();
}

void i_6F()
{
	if (i32)
	{
		D("outsd");
		if (repe | repne)
		{
			if (a32)
			{
				while (r.ecx)
				{
					if (!outsd())
						return;
					r.ecx--;
				}
			}
			else
			{
				while (r.cx)
				{
					if (!outsd())
						return;
					r.cx--;
				}
			}
		}
		else
			outsd();
	}
	else
	{
		D("outsw");
		if (repe | repne)
		{
			if (a32)
			{
				while (r.ecx)
				{
					if (!outsw())
						return;
					r.ecx--;
				}
			}
			else
			{
				while (r.cx)
				{
					if (!outsw())
						return;
					r.cx--;
				}
			}
		}
		else
			outsw();
	}
}

void i_70()
{
	int cond = r.eflags & F_O;
	int d;
	if (!fetch8s(&d))
		return;
	if (i32)
	{
		D("jo %x", r.eip + d);
		if (cond)
			r.eip += d;
	}
	else
	{
		D("jo %x", (r.ip + d) & 0xFFFFu);
		if (cond)
		{
			r.ip += d;
			r.iph = 0;
		}
	}
}

void i_71()
{
	int cond = (r.eflags & F_O) == 0;
	int d;
	if (!fetch8s(&d))
		return;
	if (i32)
	{
		D("jno %x", r.eip + d);
		if (cond)
			r.eip += d;
	}
	else
	{
		D("jno %x", (r.ip + d) & 0xFFFFu);
		if (cond)
		{
			r.ip += d;
			r.iph = 0;
		}
	}
}

void i_72()
{
	int cond = r.eflags & F_C;
	int d;
	if (!fetch8s(&d))
		return;
	if (i32)
	{
		D("jb %x", r.eip + d);
		if (cond)
			r.eip += d;
	}
	else
	{
		D("jb %x", (r.ip + d) & 0xFFFFu);
		if (cond)
		{
			r.ip += d;
			r.iph = 0;
		}
	}
}

void i_73()
{
	int cond = (r.eflags & F_C) == 0;
	int d;
	if (!fetch8s(&d))
		return;
	if (i32)
	{
		D("jnb %x", r.eip + d);
		if (cond)
			r.eip += d;
	}
	else
	{
		D("jnb %x", (r.ip + d) & 0xFFFFu);
		if (cond)
		{
			r.ip += d;
			r.iph = 0;
		}
	}
}

void i_74()
{
	int cond = r.eflags & F_Z;
	int d;
	if (!fetch8s(&d))
		return;
	if (i32)
	{
		D("jz %x", r.eip + d);
		if (cond)
			r.eip += d;
	}
	else
	{
		D("jz %x", (r.ip + d) & 0xFFFFu);
		if (cond)
		{
			r.ip += d;
			r.iph = 0;
		}
	}
}

void i_75()
{
	int cond = (r.eflags & F_Z) == 0;
	int d;
	if (!fetch8s(&d))
		return;
	if (i32)
	{
		D("jnz %x", r.eip + d);
		if (cond)
			r.eip += d;
	}
	else
	{
		D("jnz %x", (r.ip + d) & 0xFFFFu);
		if (cond)
		{
			r.ip += d;
			r.iph = 0;
		}
	}
}

void i_76()
{
	int cond = r.eflags & (F_Z | F_C);
	int d;
	if (!fetch8s(&d))
		return;
	if (i32)
	{
		D("jbe %x", r.eip + d);
		if (cond)
			r.eip += d;
	}
	else
	{
		D("jbe %x", (r.ip + d) & 0xFFFFu);
		if (cond)
		{
			r.ip += d;
			r.iph = 0;
		}
	}
}

void i_77()
{
	int cond = (r.eflags & (F_Z | F_C)) == 0;
	int d;
	if (!fetch8s(&d))
		return;
	if (i32)
	{
		D("jnbe %x", r.eip + d);
		if (cond)
			r.eip += d;
	}
	else
	{
		D("jnbe %x", (r.ip + d) & 0xFFFFu);
		if (cond)
		{
			r.ip += d;
			r.iph = 0;
		}
	}
}

void i_78()
{
	int cond = r.eflags & F_S;
	int d;
	if (!fetch8s(&d))
		return;
	if (i32)
	{
		D("js %x", r.eip + d);
		if (cond)
			r.eip += d;
	}
	else
	{
		D("js %x", (r.ip + d) & 0xFFFFu);
		if (cond)
		{
			r.ip += d;
			r.iph = 0;
		}
	}
}

void i_79()
{
	int cond = (r.eflags & F_S) == 0;
	int d;
	if (!fetch8s(&d))
		return;
	if (i32)
	{
		D("jns %x", r.eip + d);
		if (cond)
			r.eip += d;
	}
	else
	{
		D("jns %x", (r.ip + d) & 0xFFFFu);
		if (cond)
		{
			r.ip += d;
			r.iph = 0;
		}
	}
}

void i_7A()
{
	int cond = r.eflags & F_P;
	int d;
	if (!fetch8s(&d))
		return;
	if (i32)
	{
		D("jp %x", r.eip + d);
		if (cond)
			r.eip += d;
	}
	else
	{
		D("jp %x", (r.ip + d) & 0xFFFFu);
		if (cond)
		{
			r.ip += d;
			r.iph = 0;
		}
	}
}

void i_7B()
{
	int cond = (r.eflags & F_P) == 0;
	int d;
	if (!fetch8s(&d))
		return;
	if (i32)
	{
		D("jnp %x", r.eip + d);
		if (cond)
			r.eip += d;
	}
	else
	{
		D("jnp %x", (r.ip + d) & 0xFFFFu);
		if (cond)
		{
			r.ip += d;
			r.iph = 0;
		}
	}
}

void i_7C()
{
	int cond = (!(r.flags & F_S)) != (!(r.flags & F_O));
	int d;
	if (!fetch8s(&d))
		return;
	if (i32)
	{
		D("jl %x", r.eip + d);
		if (cond)
			r.eip += d;
	}
	else
	{
		D("jl %x", (r.ip + d) & 0xFFFFu);
		if (cond)
		{
			r.ip += d;
			r.iph = 0;
		}
	}
}

void i_7D()
{
	int cond = (!(r.flags & F_S)) == (!(r.flags & F_O));
	int d;
	if (!fetch8s(&d))
		return;
	if (i32)
	{
		D("jnl %x", r.eip + d);
		if (cond)
			r.eip += d;
	}
	else
	{
		D("jnl %x", (r.ip + d) & 0xFFFFu);
		if (cond)
		{
			r.ip += d;
			r.iph = 0;
		}
	}
}

void i_7E()
{
	int cond = (r.flags & F_Z) || ((!(r.flags & F_S)) != (!(r.flags & F_O)));
	int d;
	if (!fetch8s(&d))
		return;
	if (i32)
	{
		D("jle %x", r.eip + d);
		if (cond)
			r.eip += d;
	}
	else
	{
		D("jle %x", (r.ip + d) & 0xFFFFu);
		if (cond)
		{
			r.ip += d;
			r.iph = 0;
		}
	}
}

void i_7F()
{
	int cond = ((r.flags & F_Z) == 0) && ((!(r.flags & F_S)) == (!(r.flags & F_O)));
	int d;
	if (!fetch8s(&d))
		return;
	if (i32)
	{
		D("jnle %x", r.eip + d);
		if (cond)
			r.eip += d;
	}
	else
	{
		D("jnle %x", (r.ip + d) & 0xFFFFu);
		if (cond)
		{
			r.ip += d;
			r.iph = 0;
		}
	}
}

void i_80()
{
	unsigned char b;
	unsigned int d;
	if (!mod(1))
		return;
	if (!fetch8(&b))
		return;
	if (DEBUG && dasm != NULL)
	{
		switch ((modrm >> 3) & 7)
		{
			case 0:
				D("add ");
				disasm_mod();
				D(", 0x%.2X", b);
				break;
			case 1:
				D("or ");
				disasm_mod();
				D(", 0x%.2X", b);
				break;
			case 2:
				D("adc ");
				disasm_mod();
				D(", 0x%.2X", b);
				break;
			case 3:
				D("sbb ");
				disasm_mod();
				D(", 0x%.2X", b);
				break;
			case 4:
				D("and ");
				disasm_mod();
				D(", 0x%.2X", b);
				break;
			case 5:
				D("sub ");
				disasm_mod();
				D(", 0x%.2X", b);
				break;
			case 6:
				D("xor ");
				disasm_mod();
				D(", 0x%.2X", b);
				break;
			case 7:
				D("cmp ");
				disasm_mod();
				D(", 0x%x", b);
				break;
		}
	}
	if (!readmod(&d))
		return;
	switch ((modrm >> 3) & 7)
	{
		case 0:
			writemod(add8(d, b, 0));
			break;
		case 1:
			writemod(or8(d, b));
			break;
		case 2:
			writemod(add8(d, b, r.eflags & F_C));
			break;
		case 3:
			writemod(sub8(d, b, r.eflags & F_C));
			break;
		case 4:
			writemod(and8(d, b));
			break;
		case 5:
			writemod(sub8(d, b, 0));
			break;
		case 6:
			writemod(xor8(d, b));
			break;
		case 7:
			sub8(d, b, 0);
			break;
	}
}

void i_81_0(unsigned int s, unsigned int d)
{
	if (i32) writemod(add32(d, s, 0)); else writemod(add16(d, s, 0));
}

void i_81_1(unsigned int s, unsigned int d)
{
	if (i32) writemod(or32(d, s)); else writemod(or16(d, s));
}

void i_81_2(unsigned int s, unsigned int d)
{
	if (i32) writemod(add32(d, s, r.eflags & F_C)); else writemod(add16(d, s, r.eflags & F_C));
}

void i_81_3(unsigned int s, unsigned int d)
{
	if (i32) writemod(sub32(d, s, r.eflags & F_C)); else writemod(sub16(d, s, r.eflags & F_C));
}

void i_81_4(unsigned int s, unsigned int d)
{
	if (i32) writemod(and32(d, s)); else writemod(and16(d, s));
}

void i_81_5(unsigned int s, unsigned int d)
{
	if (i32) writemod(sub32(d, s, 0)); else writemod(sub16(d, s, 0));
}

void i_81_6(unsigned int s, unsigned int d)
{
	if (i32) writemod(xor32(d, s)); else writemod(xor16(d, s));
}

void i_81_7(unsigned int s, unsigned int d)
{
	if (i32) sub32(d, s, 0); else sub16(d, s, 0);
}

void (*i81table[8])(unsigned int, unsigned int) = {
	&i_81_0, &i_81_1, &i_81_2, &i_81_3, &i_81_4, &i_81_5, &i_81_6, &i_81_7
};

void i_81()
{
	unsigned short w;
	unsigned int s;
	unsigned int d;
	if (!mod(0))
		return;
	if (i32)
	{
		if (!fetch32(&s))
			return;
	}
	else
	{
		if (!fetch16(&w))
			return;
		s = w;
	}
	if (DEBUG && dasm != NULL)
	{
		switch ((modrm >> 3) & 7)
		{
			case 0:
				D("add ");
				disasm_mod();
				D(", 0x%x", s);
				break;
			case 1:
				D("or ");
				disasm_mod();
				D(", 0x%x", s);
				break;
			case 2:
				D("adc ");
				disasm_mod();
				D(", 0x%x", s);
				break;
			case 3:
				D("sbb ");
				disasm_mod();
				D(", 0x%x", s);
				break;
			case 4:
				D("and ");
				disasm_mod();
				D(", 0x%x", s);
				break;
			case 5:
				D("sub ");
				disasm_mod();
				D(", 0x%x", s);
				break;
			case 6:
				D("xor ");
				disasm_mod();
				D(", 0x%x", s);
				break;
			case 7:
				D("cmp ");
				disasm_mod();
				D(", 0x%x", s);
				break;
		}
	}
	if (!readmod(&d))
		return;
	i81table[(modrm >> 3) & 7](s, d);
	return;
	if (i32)
	{
		switch ((modrm >> 3) & 7)
		{
			case 0:
				writemod(add32(d, s, 0));
				break;
			case 1:
				writemod(or32(d, s));
				break;
			case 2:
				writemod(add32(d, s, r.eflags & F_C));
				break;
			case 3:
				writemod(sub32(d, s, r.eflags & F_C));
				break;
			case 4:
				writemod(and32(d, s));
				break;
			case 5:
				writemod(sub32(d, s, 0));
				break;
			case 6:
				writemod(xor32(d, s));
				break;
			case 7:
				sub32(d, s, 0);
				break;
		}
	}
	else
	{
		switch ((modrm >> 3) & 7)
		{
			case 0:
				writemod(add16(d, s, 0));
				break;
			case 1:
				writemod(or16(d, s));
				break;
			case 2:
				writemod(add16(d, s, r.eflags & F_C));
				break;
			case 3:
				writemod(sub16(d, s, r.eflags & F_C));
				break;
			case 4:
				writemod(and16(d, s));
				break;
			case 5:
				writemod(sub16(d, s, 0));
				break;
			case 6:
				writemod(xor16(d, s));
				break;
			case 7:
				sub16(d, s, 0);
				break;
		}
	}
}

void i_82()
{
	i_80();
}

void i_83()
{
	unsigned char b;
	unsigned int s;
	unsigned int d;
	if (!mod(0))
		return;
	if (!fetch8(&b))
		return;
	s = b;
	if (s & 0x80)
		s |= 0xFFFFFF00u;
	if (DEBUG && dasm != NULL)
	{
		switch ((modrm >> 3) & 7)
		{
			case 0:
				D("add ");
				disasm_mod();
				D(", 0x%x", s);
				break;
			case 1:
				D("or ");
				disasm_mod();
				D(", 0x%x", s);
				break;
			case 2:
				D("adc ");
				disasm_mod();
				D(", 0x%x", s);
				break;
			case 3:
				D("sbb ");
				disasm_mod();
				D(", 0x%x", s);
				break;
			case 4:
				D("and ");
				disasm_mod();
				D(", 0x%x", s);
				break;
			case 5:
				D("sub ");
				disasm_mod();
				D(", 0x%x", s);
				break;
			case 6:
				D("xor ");
				disasm_mod();
				D(", 0x%x", s);
				break;
			case 7:
				D("cmp ");
				disasm_mod();
				D(", 0x%x", s);
				break;
		}
	}
	if (!readmod(&d))
		return;
	i81table[(modrm >> 3) & 7](s, d);
	return;
	if (i32)
	{
		switch ((modrm >> 3) & 7)
		{
			case 0:
				writemod(add32(d, s, 0));
				break;
			case 1:
				writemod(or32(d, s));
				break;
			case 2:
				writemod(add32(d, s, r.eflags & F_C));
				break;
			case 3:
				writemod(sub32(d, s, r.eflags & F_C));
				break;
			case 4:
				writemod(and32(d, s));
				break;
			case 5:
				writemod(sub32(d, s, 0));
				break;
			case 6:
				writemod(xor32(d, s));
				break;
			case 7:
				sub32(d, s, 0);
				break;
		}
	}
	else
	{
		switch ((modrm >> 3) & 7)
		{
			case 0:
				writemod(add16(d, s, 0));
				break;
			case 1:
				writemod(or16(d, s));
				break;
			case 2:
				writemod(add16(d, s, r.eflags & F_C));
				break;
			case 3:
				writemod(sub16(d, s, r.eflags & F_C));
				break;
			case 4:
				writemod(and16(d, s));
				break;
			case 5:
				writemod(sub16(d, s, 0));
				break;
			case 6:
				writemod(xor16(d, s));
				break;
			case 7:
				sub16(d, s, 0);
				break;
		}
	}
}

void i_84()
{
	unsigned int d, s;
	D("test ");
	if (!mod(1)) return;
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (!readmod(&s)) return;
	d = readmodreg();
	test8(d, s);
	
}

void i_85()
{
	unsigned int d, s;
	D("test ");
	if (!mod(0)) return;
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (!readmod(&s)) return;
	d = readmodreg();
	if (i32)
		test32(d, s);
	else
		test16(d, s);
	
}

void i_86()
{
	unsigned int d, s;
	D("xchg ");
	if (!mod(1)) return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	if (!readmod(&s)) return;
	d = readmodreg();
	if (!writemod(d)) return;
	writemodreg(s);
}

void i_87()
{
	unsigned int d, s;
	D("xchg ");
	if (!mod(0)) return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	if (!readmod(&s)) return;
	d = readmodreg();
	if (!writemod(d)) return;
	writemodreg(s);
}

void i_88()
{
	D("mov ");
	if (!mod(1)) return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	writemod(readmodreg());
	
}

void i_89()
{
	D("mov ");
	if (!mod(0)) return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	writemod(readmodreg());
	
}

void i_8A()
{
	unsigned int s;
	D("mov ");
	if (!mod(1)) return;
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (!readmod(&s))
		return;
	writemodreg(s);
	
}

void i_8B()
{
	unsigned int s;
	D("mov ");
	if (!mod(0)) return;
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (!readmod(&s))
		return;
	writemodreg(s);
	
}

void i_8C()
{
	D("mov ");
	if (!mod(0)) return;
	disasm_mod();
	D(", ");
	disasm_modsreg();
	i32 = 0;
	writemod(readmodsreg());
}

void i_8D()
{
	D("lea ");
	if (!mod(0)) return;
	disasm_modreg();
	D(", ");
	disasm_mod();
	writemodreg(ofs);
}

void i_8E()
{
	unsigned int s;
	D("mov ");
	if (!mod(0)) return;
	disasm_modsreg();
	D(", ");
	disasm_mod();
	if (!readmod(&s))
		return;
	writemodsreg(s);
	
}

void i_8F()
{
	unsigned short w;
	unsigned int s;
	D("pop ");
	unsigned int esp = r.esp;

	// !!!
	if (i32)
	{
		if (!pop32(&s))
		{
			r.esp = esp;
			return;
		}
	}
	else
	{
		if (!pop16(&w))
		{
			r.esp = esp;
			return;
		}
		s = w;
	}

	if (!mod(0))
	{
		r.esp = esp;
		return;
	}
	disasm_mod();
	if (!writemod(s))
	{
		r.esp = esp;
		return;
	}
}

void i_90()
{
	D("nop");
}

void i_91()
{
	unsigned int t = r.eax;
	if (i32)
	{
		D("xchg eax, ecx");
		r.eax = r.ecx;
		r.ecx = t;
	}
	else
	{
		D("xchg ax, cx");
		r.ax = r.cx;
		r.cx = (unsigned short)t;
	}
}

void i_92()
{
	unsigned int t = r.eax;
	if (i32)
	{
		D("xchg eax, edx");
		r.eax = r.edx;
		r.edx = t;
	}
	else
	{
		D("xchg ax, dx");
		r.ax = r.dx;
		r.dx = (unsigned short)t;
	}
}

void i_93()
{
	unsigned int t = r.eax;
	if (i32)
	{
		D("xchg eax, ebx");
		r.eax = r.ebx;
		r.ebx = t;
	}
	else
	{
		D("xchg ax, bx");
		r.ax = r.bx;
		r.bx = (unsigned short)t;
	}
}

void i_94()
{
	unsigned int t = r.eax;
	if (i32)
	{
		D("xchg eax, esp");
		r.eax = r.esp;
		r.esp = t;
	}
	else
	{
		D("xchg ax, sp");
		r.ax = r.sp;
		r.sp = (unsigned short)t;
	}
}

void i_95()
{
	unsigned int t = r.eax;
	if (i32)
	{
		D("xchg eax, ebp");
		r.eax = r.ebp;
		r.ebp = t;
	}
	else
	{
		D("xchg ax, bp");
		r.ax = r.bp;
		r.bp = (unsigned short)t;
	}
}

void i_96()
{
	unsigned int t = r.eax;
	if (i32)
	{
		D("xchg eax, esi");
		r.eax = r.esi;
		r.esi = t;
	}
	else
	{
		D("xchg ax, si");
		r.ax = r.si;
		r.si = (unsigned short)t;
	}
}

void i_97()
{
	unsigned int t = r.eax;
	if (i32)
	{
		D("xchg eax, edi");
		r.eax = r.edi;
		r.edi = t;
	}
	else
	{
		D("xchg ax, di");
		r.ax = r.di;
		r.di = (unsigned short)t;
	}
}

void i_98()
{
	if (i32)
	{
		D("cwde");
		if (r.ax & 0x8000)
			r.axh = 0xFFFFu;
		else
			r.axh = 0;
	}
	else
	{
		D("cbw");
		if (r.al & 0x80)
			r.ah = 0xFF;
		else
			r.ah = 0;
	}
}

void i_99()
{
	if (i32)
	{
		D("cdq");
		if (r.eax & 0x80000000u)
			r.edx = 0xFFFFFFFFu;
		else
			r.edx = 0;
	}
	else
	{
		D("cwd");
		if (r.ax & 0x8000)
			r.dx = 0xFFFFu;
		else
			r.dx = 0;
	}
}

void i_9A()
{
	unsigned int o32;
	unsigned short o16, s;
	D("call far ");
	if (i32)
	{
		if (!fetch32(&o32))
			return;
	}
	else
	{
		if (!fetch16(&o16))
			return;
		o32 = o16;
	}
	if (!fetch16(&s))
		return;
	D("%x:%x", s, o32);
	far_call(s, o32);
}

void i_9B()
{
	D("wait");
	if (((cr[0] & (CR0_EM | CR0_TS)) == (CR0_EM | CR0_TS)) && (idt_limit > 0))
	{
		ex(EX_COPROCESSOR_NA);
		return;
	}
}

void i_9C()
{
	if (i32)
	{
		D("pushf");
		push32(r.eflags | 0x0002);
	}
	else
	{
		D("pushf");
#ifdef DETECT86
		push16(r.flags | 0xF002);
#else
		push16(r.flags | 0x0002);
#endif
	}
}

void i_9D()
{
	unsigned int d;
	unsigned short w;
	unsigned int mask;
	
	mask = F_VM | F_RF;

	/*
	if (pmode)
	{
		if (r.eflags & F_VM)
		{
			// #GP(0) fault if IOPL is less than 3, to permit emulation
			if (IOPL < 3)
			{
				ex(EX_GP, 0);
				return;
			}
		}
		else
		{
			if (IOPL < cpl)
				mask |= F_I;
		}

		if (cpl != 0)
			mask |= F_IOPL;
	}
	*/

	if (i32)
	{
		D("popf");
		if (!pop32(&d))
			return;
		d |= 0x0002;
#ifndef DETECT486
		d &= ~F_ID;
#endif
		set_flags(d, ~mask);
	}
	else
	{
		D("popf");
		if (!pop16(&w))
			return;
		w |= 0x0002;
		set_flags16(w, ~mask);
	}
}

void i_9E()
{
	D("sahf");
	set_flags16(r.ah, F_S | F_Z | F_A | F_P | F_C);
}

void i_9F()
{
	D("lahf");
	r.ah = (unsigned char)r.flags;
}

void i_A0()
{
	unsigned short w;
	if (a32)
	{
		if (!fetch32(&ofs))
			return;
	}
	else
	{
		if (!fetch16(&w))
			return;
		ofs = w;
	}
	D("mov al, %s:[0x%x]", sel->name, ofs);
	read8(sel, ofs, &r.al);
}

void i_A1()
{
	unsigned short w;
	if (a32)
	{
		if (!fetch32(&ofs))
			return;
	}
	else
	{
		if (!fetch16(&w))
			return;
		ofs = w;
	}
	if (i32)
	{
		D("mov eax, %s:[0x%.8x]", sel->name, ofs);
		read32(sel, ofs, &r.eax);
	}
	else
	{
		D("mov ax, %s:[0x%x]", sel->name, ofs);
		read16(sel, ofs, &r.ax);
	}
}

void i_A2()
{
	unsigned short w;
	if (a32)
	{
		if (!fetch32(&ofs))
			return;
	}
	else
	{
		if (!fetch16(&w))
			return;
		ofs = w;
	}
	D("mov %s:[0x%.8x], al", sel->name, ofs);
	write8(sel, ofs, r.al);
}

void i_A3()
{
	unsigned short w;
	if (a32)
	{
		if (!fetch32(&ofs))
			return;
	}
	else
	{
		if (!fetch16(&w))
			return;
		ofs = w;
	}
	if (i32)
	{
		D("mov %s:[0x%.8x], eax", sel->name, ofs);
		write32(sel, ofs, r.eax);
	}
	else
	{
		D("mov %s:[0x%x], ax", sel->name, ofs);
		write16(sel, ofs, r.ax);
	}
}

void i_A4()
{
	D("movsb");
	if (repe | repne)
	{
		if (a32)
		{
			while (r.ecx)
			{
				if (!movsb())
					return;
				r.ecx--;
			}
		}
		else
		{
			while (r.cx)
			{
				if (!movsb())
					return;
				r.cx--;
			}
		}
	}
	else
		movsb();
}

void i_A5()
{
	unsigned int ip = r.eip;
	if (i32)
	{
		D("movsd");
		if (repe | repne)
		{
			if (a32)
			{
				while (r.ecx)
				{
					if (!movsd())
						return;
					r.ecx--;
				}
			}
			else
			{
				while (r.cx)
				{
					if (!movsd())
						return;
					r.cx--;
				}
			}
		}
		else
			movsd();
	}
	else
	{
		D("movsw");
		if (repe | repne)
		{
			if (a32)
			{
				while (r.ecx)
				{
					if (!movsw())
						return;
					r.ecx--;
				}
			}
			else
			{
				while (r.cx)
				{
					if (!movsw())
						return;
					r.cx--;
				}
			}
		}
		else
			movsw();
	}
}

void i_A6()
{
	D("cmpsb");
	if (repe | repne)
	{
		if (a32)
		{
			while (r.ecx)
			{
				if (!cmpsb())
					return;
				r.ecx--;
				if ((!(r.eflags & F_Z)) == (!repne))
					break;
			}
		}
		else
		{
			while (r.cx)
			{
				if (!cmpsb())
					return;
				r.cx--;
				if ((!(r.eflags & F_Z)) == (!repne))
					break;
			}
		}
	}
	else
		cmpsb();
}

void i_A7()
{
	if (i32)
	{
		D("cmpsd");
		if (repe | repne)
		{
			if (a32)
			{
				while (r.ecx)
				{
					if (!cmpsd())
						return;
					r.ecx--;
					if ((!(r.eflags & F_Z)) == (!repne))
						break;
				}
			}
			else
			{
				while (r.cx)
				{
					if (!cmpsd())
						return;
					r.cx--;
					if ((!(r.eflags & F_Z)) == (!repne))
						break;
				}
			}
		}
		else
			cmpsd();
	}
	else
	{
		D("cmpsw");
		if (repe | repne)
		{
			if (a32)
			{
				while (r.ecx)
				{
					if (!cmpsw())
						return;
					r.ecx--;
					if ((!(r.eflags & F_Z)) == (!repne))
						break;
				}
			}
			else
			{
				while (r.cx)
				{
					if (!cmpsw())
						return;
					r.cx--;
					if ((!(r.eflags & F_Z)) == (!repne))
						break;
				}
			}
		}
		else
			cmpsw();
	}
}

void i_A8()
{
	unsigned char v;
	if (!fetch8(&v))
		return;
	D("test al, 0x%x", v);
	test8(r.al, v);
}

void i_A9()
{
	unsigned short w;
	unsigned int d;
	if (i32)
	{
		if (!fetch32(&d))
			return;
		D("test eax, 0x%x", d);
		test32(r.eax, d);
	}
	else
	{
		if (!fetch16(&w))
			return;
		D("test ax, 0x%x", w);
		test16(r.ax, w);
	}
}

void i_AA()
{
	D("stosb");
	if (repe | repne)
	{
		if (a32)
		{
			while (r.ecx)
			{
				if (!stosb())
					return;
				r.ecx--;
			}
		}
		else
		{
			while (r.cx)
			{
				if (!stosb())
					return;
				r.cx--;
			}
		}
	}
	else
		stosb();
}

void i_AB()
{
	if (i32)
	{
		D("stosd");
		if (repe | repne)
		{
			if (a32)
			{
				while (r.ecx)
				{
					if (!stosd())
						return;
					r.ecx--;
				}
			}
			else
			{
				while (r.cx)
				{
					if (!stosd())
						return;
					r.cx--;
				}
			}
		}
		else
			stosd();
	}
	else
	{
		D("stosw");
		if (repe | repne)
		{
			if (a32)
			{
				while (r.ecx)
				{
					if (!stosw())
						return;
					r.ecx--;
				}
			}
			else
			{
				while (r.cx)
				{
					if (!stosw())
						return;
					r.cx--;
				}
			}
		}
		else
			stosw();
	}
}

void i_AC()
{
	D("lodsb");
	lodsb();
}

void i_AD()
{
	if (i32)
	{
		D("lodsd");
		lodsd();
	}
	else
	{
		D("lodsw");
		lodsw();
	}
}

void i_AE()
{
	D("scasb");
	if (repe | repne)
	{
		if (a32)
		{
			while (r.ecx)
			{
				if (!scasb())
					return;
				r.ecx--;
				if ((!(r.eflags & F_Z)) == (!repne))
					break;
			}
		}
		else
		{
			while (r.cx)
			{
				if (!scasb())
					return;
				r.cx--;
				if ((!(r.eflags & F_Z)) == (!repne))
					break;
			}
		}
	}
	else
		scasb();
}

void i_AF()
{
	if (i32)
	{
		D("scasd");
		if (repe | repne)
		{
			if (a32)
			{
				while (r.ecx)
				{
					if (!scasd())
						return;
					r.ecx--;
					if ((!(r.eflags & F_Z)) == (!repne))
						break;
				}
			}
			else
			{
				while (r.cx)
				{
					if (!scasd())
						return;
					r.cx--;
					if ((!(r.eflags & F_Z)) == (!repne))
						break;
				}
			}
		}
		else
			scasd();
	}
	else
	{
		D("scasw");
		if (repe | repne)
		{
			if (a32)
			{
				while (r.ecx)
				{
					if (!scasw())
						return;
					r.ecx--;
					if ((!(r.eflags & F_Z)) == (!repne))
						break;
				}
			}
			else
			{
				while (r.cx)
				{
					if (!scasw())
						return;
					r.cx--;
					if ((!(r.eflags & F_Z)) == (!repne))
						break;
				}
			}
		}
		else
			scasw();
	}
}

void i_B0()
{
	unsigned char b;
	if (!fetch8(&b))
		return;
	D("mov al, 0x%x", b);
	r.al = b;
}

void i_B1()
{
	unsigned char b;
	if (!fetch8(&b))
		return;
	D("mov cl, 0x%x", b);
	r.cl = b;
}

void i_B2()
{
	unsigned char b;
	if (!fetch8(&b))
		return;
	D("mov dl, 0x%x", b);
	r.dl = b;
}

void i_B3()
{
	unsigned char b;
	if (!fetch8(&b))
		return;
	D("mov bl, 0x%x", b);
	r.bl = b;
}

void i_B4()
{
	unsigned char b;
	if (!fetch8(&b))
		return;
	D("mov ah, 0x%x", b);
	r.ah = b;
}

void i_B5()
{
	unsigned char b;
	if (!fetch8(&b))
		return;
	D("mov ch, 0x%x", b);
	r.ch = b;
}

void i_B6()
{
	unsigned char b;
	if (!fetch8(&b))
		return;
	D("mov dh, 0x%x", b);
	r.dh = b;
}

void i_B7()
{
	unsigned char b;
	if (!fetch8(&b))
		return;
	D("mov bh, 0x%x", b);
	r.bh = b;
}

void i_B8()
{
	unsigned short w;
	unsigned int d;
	if (i32)
	{
		if (!fetch32(&d))
			return;
		D("mov eax, 0x%x", d);
		r.eax = d;
	}
	else
	{
		if (!fetch16(&w))
			return;
		D("mov ax, 0x%x", w);
		r.ax = w;
	}
}

void i_B9()
{
	unsigned short w;
	unsigned int d;
	if (i32)
	{
		if (!fetch32(&d))
			return;
		D("mov ecx, 0x%x", d);
		r.ecx = d;
	}
	else
	{
		if (!fetch16(&w))
			return;
		D("mov cx, 0x%x", w);
		r.cx = w;
	}
}

void i_BA()
{
	unsigned short w;
	unsigned int d;
	if (i32)
	{
		if (!fetch32(&d))
			return;
		D("mov edx, 0x%x", d);
		r.edx = d;
	}
	else
	{
		if (!fetch16(&w))
			return;
		D("mov dx, 0x%x", w);
		r.dx = w;
	}
}

void i_BB()
{
	unsigned short w;
	unsigned int d;
	if (i32)
	{
		if (!fetch32(&d))
			return;
		D("mov ebx, 0x%x", d);
		r.ebx = d;
	}
	else
	{
		if (!fetch16(&w))
			return;
		D("mov bx, 0x%x", w);
		r.bx = w;
	}
}

void i_BC()
{
	unsigned short w;
	unsigned int d;
	if (i32)
	{
		if (!fetch32(&d))
			return;
		D("mov esp, 0x%x", d);
		r.esp = d;
	}
	else
	{
		if (!fetch16(&w))
			return;
		D("mov sp, 0x%x", w);
		r.sp = w;
	}
}

void i_BD()
{
	unsigned short w;
	unsigned int d;
	if (i32)
	{
		if (!fetch32(&d))
			return;
		D("mov ebp, 0x%x", d);
		r.ebp = d;
	}
	else
	{
		if (!fetch16(&w))
			return;
		D("mov bp, 0x%x", w);
		r.bp = w;
	}
}

void i_BE()
{
	unsigned short w;
	unsigned int d;
	if (i32)
	{
		if (!fetch32(&d))
			return;
		D("mov esi, 0x%x", d);
		r.esi = d;
	}
	else
	{
		if (!fetch16(&w))
			return;
		D("mov si, 0x%x", w);
		r.si = w;
	}
}

void i_BF()
{
	unsigned short w;
	unsigned int d;
	if (i32)
	{
		if (!fetch32(&d))
			return;
		D("mov edi, 0x%x", d);
		r.edi = d;
	}
	else
	{
		if (!fetch16(&w))
			return;
		D("mov di, 0x%x", w);
		r.di = w;
	}
}

void i_C0()
{
	int count;
	unsigned char v;
	unsigned int d;
	if (!mod(1))
		return;
	if (!fetch8(&v))
		return;
	count = v & 0x1F;
	if (!readmod(&d))
		return;
	v = (unsigned char)d;
	switch ((modrm >> 3) & 0x07)
	{
		case 0:
			// rol
			D("rol ");
			disasm_mod();
			D(", 0x%x", count);
			v = rol8(v, count);
			break;
		case 1:
			// ror
			D("ror ");
			disasm_mod();
			D(", 0x%x", count);
			v = ror8(v, count);
			break;
		case 2:
			// rcl
			D("rcl ");
			disasm_mod();
			D(", 0x%x", count);
			v = rcl8(v, count);
			break;
		case 3:
			// rcr
			D("rcr ");
			disasm_mod();
			D(", 0x%x", count);
			v = rcr8(v, count);
			break;
		case 4:
			// shl
			D("shl ");
			disasm_mod();
			D(", 0x%x", count);
			v = shl8(v, count);
			break;
		case 5:
			// shr
			D("shr ");
			disasm_mod();
			D(", 0x%x", count);
			v = shr8(v, count);
			break;
		case 6:
			// sal
			D("sal ");
			disasm_mod();
			D(", 0x%x", count);
			v = shl8(v, count);
			break;
		case 7:
			// sar
			D("sar ");
			disasm_mod();
			D(", 0x%x", count);
			v = sar8(v, count);
			break;
	}
	writemod(v);
}

void i_C1()
{
	int count;
	unsigned char b;
	unsigned int v;
	if (!mod(0))
		return;
	if (!fetch8(&b))
		return;
	count = b & 0x1F;
	if (!readmod(&v))
		return;

	if (i32)
	{
		switch ((modrm >> 3) & 0x07)
		{
			case 0:
				// rol
				D("rol ");
				disasm_mod();
				D(", %d", count);
				v = rol32(v, count);
				break;
			case 1:
				// ror
				D("ror ");
				disasm_mod();
				D(", 0x%x", count);
				v = ror32(v, count);
				break;
			case 2:
				// rcl
				D("rcl ");
				disasm_mod();
				D(", 0x%x", count);
				v = rcl32(v, count);
				break;
			case 3:
				// rcr
				D("rcr ");
				disasm_mod();
				D(", 0x%x", count);
				v = rcr32(v, count);
				break;
			case 4:
				// shl
				D("shl ");
				disasm_mod();
				D(", 0x%x", count);
				v = shl32(v, count);
				break;
			case 5:
				// shr
				D("shr ");
				disasm_mod();
				D(", 0x%x", count);
				v = shr32(v, count);
				break;
			case 6:
				// sal
				D("sal ");
				disasm_mod();
				D(", 0x%x", count);
				v = shl32(v, count);
				break;
			case 7:
				// sar
				D("sar ");
				disasm_mod();
				D(", 0x%x", count);
				v = sar32(v, count);
				break;
		}
	}
	else
	{
		switch ((modrm >> 3) & 0x07)
		{
			case 0:
				// rol
				D("rol ");
				disasm_mod();
				D(", 0x%x", count);
				v = rol16(v, count);
				break;
			case 1:
				// ror
				D("ror ");
				disasm_mod();
				D(", 0x%x", count);
				v = ror16(v, count);
				break;
			case 2:
				// rcl
				D("rcl ");
				disasm_mod();
				D(", 0x%x", count);
				v = rcl16(v, count);
				break;
			case 3:
				// rcr
				D("rcr ");
				disasm_mod();
				D(", 0x%x", count);
				v = rcr16(v, count);
				break;
			case 4:
				// shl
				D("shl ");
				disasm_mod();
				D(", 0x%x", count);
				v = shl16(v, count);
				break;
			case 5:
				// shr
				D("shr ");
				disasm_mod();
				D(", 0x%x", count);
				v = shr16(v, count);
				break;
			case 6:
				// sal
				D("sal ");
				disasm_mod();
				D(", 0x%x", count);
				v = shl16(v, count);
				break;
			case 7:
				// sar
				D("sar ");
				disasm_mod();
				D(", 0x%x", count);
				v = sar16(v, count);
				break;
		}
	}
	writemod(v);
}

void i_C2()
{
	unsigned short w;
	unsigned int d;

	if (!fetch16(&w))
		return;
	d = w;

	D("retn %d", d);
	if (i32)
	{
		pop32(&r.eip);
		r.esp += d;
	}
	else
	{
		pop16(&r.ip);
		r.iph = 0;
		r.sp += d;
		r.sph = 0;
	}
}

void i_C3()
{
	D("ret");
	if (i32)
	{
		pop32(&r.eip);
	}
	else
	{
		pop16(&r.ip);
		r.iph = 0;
	}
}

void i_C4()
{
	unsigned short s, o16;
	unsigned int o32;
	if (!mod(0))
		return;
	D("les ");
	disasm_modreg();
	D(", ");
	disasm_mod();
	
	if (i32)
	{
		if (!read32(sel, ofs, &o32))
			return;
		ofs += 4;
		if (!read16(sel, ofs, &s))
			return;
		if (!set_selector(&es, s, 1))
			return;
		writemodreg(o32);
	}
	else
	{
		if (!read16(sel, ofs, &o16))
			return;
		ofs += 2;
		if (!read16(sel, ofs, &s))
			return;
		if (!set_selector(&es, s, 1))
			return;
		writemodreg(o16);
	}
}

void i_C5()
{
	unsigned short s, o16;
	unsigned int o32;
	if (!mod(0))
		return;
	D("lds ");
	disasm_modreg();
	D(", ");
	disasm_mod();
	
	if (i32)
	{
		if (!read32(sel, ofs, &o32))
			return;
		ofs += 4;
		if (!read16(sel, ofs, &s))
			return;
		if (!set_selector(&ds, s, 1))
			return;
		writemodreg(o32);
	}
	else
	{
		if (!read16(sel, ofs, &o16))
			return;
		ofs += 2;
		if (!read16(sel, ofs, &s))
			return;
		if (!set_selector(&ds, s, 1))
			return;
		writemodreg(o16);
	}
}

void i_C6()
{
	unsigned char b;
	D("mov ");
	if (!mod(1))
		return;
	disasm_mod();
	if (!fetch8(&b))
		return;
	D(", 0x%x", b);
	writemod(b);
}

void i_C7()
{
	unsigned short w;
	unsigned int d;
	D("mov ");
	if (!mod(0))
		return;
	disasm_mod();
	if (i32)
	{
		if (!fetch32(&d))
			return;
	}
	else
	{
		if (!fetch16(&w))
			return;
		d = w;
	}
	D(", 0x%x", d);
	writemod(d);
}

void i_C8()
{
	unsigned short stack;
	unsigned char nest;
	unsigned int frame;
	unsigned int v;
	unsigned short w;
	int i;
	if (!fetch16(&stack))
		return;
	if (!fetch8(&nest))
		return;
	nest &= 0x1F;
	D("enter %d, %d", stack, nest);


	unsigned int sp = r.esp & ss_mask;
	unsigned int bp = r.ebp & ss_mask;

	if (i32)
	{
		sp -= 4;
		if (!write32(&ss, sp, r.ebp))
			return;
		r.ebp = r.esp - 4;
		if (nest)
		{
			for (i = 1; i < nest; i++)
			{
				sp -= 4;
				bp -= 4;
				if (!read32(&ss, bp, &v))
					return;
				if (!write32(&ss, sp, v))
					return;
			}
			sp -= 4;
			if (!write32(&ss, sp, r.ebp))
				return;
		}
	}
	else
	{
		sp -= 2;
		if (!write16(&ss, sp, r.bp))
			return;
		r.bp = r.sp - 2;
		if (nest)
		{
			for (i = 1; i < nest; i++)
			{
				sp -= 2;
				bp -= 2;
				if (!read16(&ss, bp, &w))
					return;
				if (!write16(&ss, sp, w))
					return;
			}
			sp -= 2;
			if (!write16(&ss, sp, r.bp))
				return;
		}
	}

	sp -= stack;
	r.esp = (r.esp & ss_inv_mask) | (sp & ss_mask);

	return;

	if (i32)
	{
		push32(r.ebp);
		frame = r.esp;
		if (nest)
		{
			for (i = 1; i < nest; i++)
			{
				r.ebp -= 4;
				push32(r.ebp);
			}
			push32(frame);
		}
		r.ebp = frame;
		r.esp = r.esp - stack;
	}
	else
	{
		push16(r.bp);
		frame = r.sp;
		if (nest)
		{
			for (i = 1; i < nest; i++)
			{
				r.bp -= 2;
				push16(r.bp);
			}
			push16(frame);
		}
		r.bp = frame;
		r.sp = r.sp - stack;
	}
}

void i_C9()
{
	D("leave");

	r.esp &= ~ss_mask;
	r.esp |= r.ebp & ss_mask;
	if (i32)
		pop32(&r.ebp);
	else
		pop16(&r.bp);
	return;

	if (i32)
	{
		r.esp = r.ebp;
		pop32(&r.ebp);
	}
	else
	{
		r.esp = r.bp;
		pop16(&r.bp);
	}
}

void i_CA()
{
	unsigned short w;
	if (!fetch16(&w))
		return;
	D("retf %d", w);
	far_ret(w);
}

void i_CB()
{
	D("retf");
	far_ret(0);
}

void i_CC()
{
	int i;

	if (idt_limit == 0)
	{
		// Restart 286
		for (i = 0; i < 8; i++)
			cr[i] = 0;

		gdt_base = 0;
		gdt_limit = 0xFFFFu;
		ldt_base = 0;
		ldt_limit = 0xFFFFu;
		idt_base = 0;
		idt_limit = 0xFFFFu;

		pmode = 0;
		paging = 0;

		set_selector(&es, 0xFFFFu, 1);
		set_selector(&cs, 0xFFFFu, 1);
		set_selector(&ss, 0xFFFFu, 1);
		set_selector(&ds, 0xFFFFu, 1);
		set_selector(&fs, 0xFFFFu, 1);
		set_selector(&gs, 0xFFFFu, 1);

		set_flags(0x0002, 0xFFFFFFFFu);

		r.eip = *(unsigned short *)&ram[0x467];
		set_selector(&cs, *(unsigned short *)&ram[0x469], 1);

		return;
	}
	D("int 3");
	interrupt(3, -1, INT_FLAGS_SOFT);
}

void i_CD()
{
	unsigned char n;
	if (!fetch8(&n))
		return;
	D("int 0x%.2X", n);
	interrupt(n, -1, INT_FLAGS_SOFT);
}

void i_CE()
{
	D("into");
	if (r.eflags & F_O)
		interrupt(4, -1, INT_FLAGS_SOFT);
}

void i_CF()
{
	D("iret");
	iret();
}

void i_D0()
{
	unsigned char v;
	unsigned int d;
	if (!mod(1))
		return;
	if (!readmod(&d))
		return;
	v = (unsigned char)d;
	switch ((modrm >> 3) & 0x07)
	{
		case 0:
			// rol
			D("rol ");
			disasm_mod();
			D(", 0x1");
			v = rol8(v, 1);
			break;
		case 1:
			// ror
			D("ror ");
			disasm_mod();
			D(", 0x1");
			v = ror8(v, 1);
			break;
		case 2:
			// rcl
			D("rcl ");
			disasm_mod();
			D(", 0x1");
			v = rcl8(v, 1);
			break;
		case 3:
			// rcr
			D("rcr ");
			disasm_mod();
			D(", 0x1");
			v = rcr8(v, 1);
			break;
		case 4:
			// shl
			D("shl ");
			disasm_mod();
			D(", 0x1");
			v = shl8(v, 1);
			break;
		case 5:
			// shr
			D("shr ");
			disasm_mod();
			D(", 0x1");
			v = shr8(v, 1);
			break;
		case 6:
			// sal
			D("sal ");
			disasm_mod();
			D(", 0x1");
			v = shl8(v, 1);
			break;
		case 7:
			// sar
			D("sar ");
			disasm_mod();
			D(", 0x1");
			v = sar8(v, 1);
			break;
	}
	writemod(v);
}

void i_D1()
{
	unsigned int v;
	if (!mod(0))
		return;
	if (!readmod(&v))
		return;
	if (i32)
	{
		switch ((modrm >> 3) & 0x07)
		{
			case 0:
				// rol
				D("rol ");
				disasm_mod();
				D(", 0x1");
				v = rol32(v, 1);
				break;
			case 1:
				// ror
				D("ror ");
				disasm_mod();
				D(", 0x1");
				v = ror32(v, 1);
				break;
			case 2:
				// rcl
				D("rcl ");
				disasm_mod();
				D(", 0x1");
				v = rcl32(v, 1);
				break;
			case 3:
				// rcr
				D("rcr ");
				disasm_mod();
				D(", 0x1");
				v = rcr32(v, 1);
				break;
			case 4:
				// shl
				D("shl ");
				disasm_mod();
				D(", 0x1");
				v = shl32(v, 1);
				break;
			case 5:
				// shr
				D("shr ");
				disasm_mod();
				D(", 0x1");
				
				v = shr32(v, 1);
				break;
			case 6:
				// sal
				D("sal ");
				disasm_mod();
				D(", 0x1");
				v = shl32(v, 1);
				break;
			case 7:
				// sar
				D("sar ");
				disasm_mod();
				D(", 0x1");
				v = sar32(v, 1);
				break;
		}
	}
	else
	{
		switch ((modrm >> 3) & 0x07)
		{
			case 0:
				// rol
				D("rol ");
				disasm_mod();
				D(", 0x1");
				v = rol16(v, 1);
				break;
			case 1:
				// ror
				D("ror ");
				disasm_mod();
				D(", 0x1");
				v = ror16(v, 1);
				break;
			case 2:
				// rcl
				D("rcl ");
				disasm_mod();
				D(", 0x1");
				v = rcl16(v, 1);
				break;
			case 3:
				// rcr
				D("rcr ");
				disasm_mod();
				D(", 0x1");
				v = rcr16(v, 1);
				break;
			case 4:
				// shl
				D("shl ");
				disasm_mod();
				D(", 0x1");
				v = shl16(v, 1);
				break;
			case 5:
				// shr
				D("shr ");
				disasm_mod();
				D(", 0x1");
				v = shr16(v, 1);
				break;
			case 6:
				// sal
				D("sal ");
				disasm_mod();
				D(", 0x1");
				v = shl16(v, 1);
				break;
			case 7:
				// sar
				D("sar ");
				disasm_mod();
				D(", 0x1");
				v = sar16(v, 1);
				break;
		}
	}
	writemod(v);
}

void i_D2()
{
	int count;
	unsigned char v;
	unsigned int d;
	if (!mod(1))
		return;
	count = r.cl & 0x1F;
	if (!readmod(&d))
		return;
	v = (unsigned char)d;
	switch ((modrm >> 3) & 0x07)
	{
		case 0:
			// rol
			D("rol ");
			disasm_mod();
			D(", cl");
			v = rol8(v, count);
			break;
		case 1:
			// ror
			D("ror ");
			disasm_mod();
			D(", cl");
			v = ror8(v, count);
			break;
		case 2:
			// rcl
			D("rcl ");
			disasm_mod();
			D(", cl");
			v = rcl8(v, count);
			break;
		case 3:
			// rcr
			D("rcr ");
			disasm_mod();
			D(", cl");
			v = rcr8(v, count);
			break;
		case 4:
			// shl
			D("shl ");
			disasm_mod();
			D(", cl");
			v = shl8(v, count);
			break;
		case 5:
			// shr
			D("shr ");
			disasm_mod();
			D(", cl");
			v = shr8(v, count);
			break;
		case 6:
			// sal
			D("sal ");
			disasm_mod();
			D(", cl");
			v = shl8(v, count);
			break;
		case 7:
			// sar
			D("sar ");
			disasm_mod();
			D(", cl");
			v = sar8(v, count);
			break;
	}
	writemod(v);
}

void i_D3()
{
	int count;
	unsigned int v;
	if (!mod(0))
		return;
	count = r.cl & 0x1F;
	if (!readmod(&v))
		return;
	if (i32)
	{
		switch ((modrm >> 3) & 0x07)
		{
			case 0:
				// rol
				D("rol ");
				disasm_mod();
				D(", cl");
				v = rol32(v, count);
				break;
			case 1:
				// ror
				D("ror ");
				disasm_mod();
				D(", cl");
				v = ror32(v, count);
				break;
			case 2:
				// rcl
				D("rcl ");
				disasm_mod();
				D(", cl");
				v = rcl32(v, count);
				break;
			case 3:
				// rcr
				D("rcr ");
				disasm_mod();
				D(", cl");
				v = rcr32(v, count);
				break;
			case 4:
				// shl
				D("shl ");
				disasm_mod();
				D(", cl");
				v = shl32(v, count);
				break;
			case 5:
				// shr
				D("shr ");
				disasm_mod();
				D(", cl");
				v = shr32(v, count);
				break;
			case 6:
				// sal
				D("sal ");
				disasm_mod();
				D(", cl");
				v = shl32(v, count);
				break;
			case 7:
				// sar
				D("sar ");
				disasm_mod();
				D(", cl");
				v = sar32(v, count);
				break;
		}
	}
	else
	{
		switch ((modrm >> 3) & 0x07)
		{
			case 0:
				// rol
				D("rol ");
				disasm_mod();
				D(", cl");
				v = rol16(v, count);
				break;
			case 1:
				// ror
				D("ror ");
				disasm_mod();
				D(", cl");
				v = ror16(v, count);
				break;
			case 2:
				// rcl
				D("rcl ");
				disasm_mod();
				D(", cl");
				v = rcl16(v, count);
				break;
			case 3:
				// rcr
				D("rcr ");
				disasm_mod();
				D(", cl");
				v = rcr16(v, count);
				break;
			case 4:
				// shl
				D("shl ");
				disasm_mod();
				D(", cl");
				v = shl16(v, count);
				break;
			case 5:
				// shr
				D("shr ");
				disasm_mod();
				D(", cl");
				v = shr16(v, count);
				break;
			case 6:
				// sal
				D("sal ");
				disasm_mod();
				D(", cl");
				v = shl16(v, count);
				break;
			case 7:
				// sar
				D("sar ");
				disasm_mod();
				D(", cl");
				v = sar16(v, count);
				break;
		}
	}
	writemod(v);
}

void i_D4()
{
	// aam
	unsigned char v;
	if (!fetch8(&v))
		return;
	D("aam %d", v);
	if (v == 0)
		v = 10;
	r.ah = r.al / v;
	r.al = r.al % v;
	setpsz8(r.al);
}

void i_D5()
{
	unsigned char v;
	if (!fetch8(&v))
		return;
	D("aad %d", v);
	r.al = r.ah * v + r.al;
	r.ah = 0;
	setpsz8(r.al);
}

void i_D6()
{
	D("salc");
	r.al = r.flags & F_C ? 0xFF : 0;
}

void i_D7()
{
	unsigned char b;
	D("xlat");
	if (i32)
	{
		if (!read8(sel, r.ebx + r.al, &b))
			return;
	}
	else
	{
		if (!read8(sel, (r.bx + r.al) & 0xFFFFu, &b))
			return;
	}
	r.al = b;
}

void i_D8()
{
	D("math");
	num_math++;
	if ((cr[0] & CR0_EM) && (idt_limit > 0))// && (cr[0] & CR0_PE) && 1)
	{
		ex(EX_COPROCESSOR_NA);
		return;
	}
	mod(1);
}

void i_D9()
{
	D("math");
	num_math++;
	if ((cr[0] & CR0_EM) && (idt_limit > 0))// && (cr[0] & CR0_PE) && 1)
	{
		ex(EX_COPROCESSOR_NA);
		return;
	}
	mod(1);
}

void i_DA()
{
	D("math");
	num_math++;
	if ((cr[0] & CR0_EM) && (idt_limit > 0))// && (cr[0] & CR0_PE) && 1)
	{
		ex(EX_COPROCESSOR_NA);
		return;
	}
	mod(1);
}

void i_DB()
{
	D("math");
	num_math++;
	if ((cr[0] & CR0_EM) && (idt_limit > 0))// && (cr[0] & CR0_PE) && 1)
	{
		ex(EX_COPROCESSOR_NA);
		return;
	}
#ifdef DETECT_FPU
	r.al = 0;
#endif
	mod(1);
}

void i_DC()
{
	D("math");
	num_math++;
	if ((cr[0] & CR0_EM) && (idt_limit > 0))// && (cr[0] & CR0_PE) && 1)
	{
		ex(EX_COPROCESSOR_NA);
		return;
	}
	mod(1);
}

void i_DD()
{
	D("math");
	num_math++;
	if ((cr[0] & CR0_EM) && (idt_limit > 0))// && (cr[0] & CR0_PE) && 1)
	{
		ex(EX_COPROCESSOR_NA);
		return;
	}
	mod(1);
}

void i_DE()
{
	D("math");
	num_math++;
	if ((cr[0] & CR0_EM) && (idt_limit > 0))// && (cr[0] & CR0_PE) && 1)
	{
		ex(EX_COPROCESSOR_NA);
		return;
	}
	mod(1);
}

void i_DF()
{
	D("math");
	num_math++;
	if ((cr[0] & CR0_EM) && (idt_limit > 0))// && (cr[0] & CR0_PE) && 1)
	{
		ex(EX_COPROCESSOR_NA);
		return;
	}
	mod(1);
}

void i_E0()
{
	int d;
	if (!fetch8s(&d))
		return;
	if (i32)
	{
		D("loopnz %x", r.eip + d);
		if (a32)
		{
			r.ecx--;
			if (r.ecx && ((r.eflags & F_Z) == 0))
				r.eip += d;
		}
		else
		{
			r.cx--;
			if (r.cx && ((r.eflags & F_Z) == 0))
				r.eip += d;
		}
	}
	else
	{
		D("loopnz %x", (r.ip + d) & 0xFFFFu);
		if (a32)
		{
			r.ecx--;
			if (r.ecx && ((r.eflags & F_Z) == 0))
				r.ip += d;
		}
		else
		{
			r.cx--;
			if (r.cx && ((r.eflags & F_Z) == 0))
				r.ip += d;
		}
	}
}

void i_E1()
{
	int d;
	if (!fetch8s(&d))
		return;
	if (i32)
	{
		D("loopz %x", r.eip + d);
		if (a32)
		{
			r.ecx--;
			if (r.ecx && (r.eflags & F_Z))
				r.eip += d;
		}
		else
		{
			r.cx--;
			if (r.cx && (r.eflags & F_Z))
				r.eip += d;
		}
	}
	else
	{
		D("loopz %x", (r.ip + d) & 0xFFFFu);
		if (a32)
		{
			r.ecx--;
			if (r.ecx && (r.eflags & F_Z))
				r.ip += d;
		}
		else
		{
			r.cx--;
			if (r.cx && (r.eflags & F_Z))
				r.ip += d;
		}
	}
}

void i_E2()
{
	int d;
	if (!fetch8s(&d))
		return;
	if (i32)
	{
		D("loop %x", r.eip + d);
		if (a32)
		{
			r.ecx--;
			if (r.ecx)
				r.eip += d;
		}
		else
		{
			r.cx--;
			if (r.cx)
				r.eip += d;
		}
	}
	else
	{
		D("loop %x", (r.ip + d) & 0xFFFFu);
		if (a32)
		{
			r.ecx--;
			if (r.ecx)
				r.ip += d;
		}
		else
		{
			r.cx--;
			if (r.cx)
				r.ip += d;
		}
	}
}

void i_E3()
{
	int d;
	if (!fetch8s(&d))
		return;
	if (a32)
	{
		D("jecxz %x", r.eip + d);
		if (r.ecx == 0)
			r.eip += d;
	}
	else
	{
		D("jcxz %x", r.ip + d);
		if (r.cx == 0)
			r.ip += d;
	}
}

void i_E4()
{
	unsigned char v;
	if (!fetch8(&v))
		return;
	D("in al, 0x%x", v);
	r.al = portread8(v);
}

void i_E5()
{
	unsigned char v;
	if (!fetch8(&v))
		return;
	if (i32)
	{
		D("in eax, 0x%x", v);
		r.eax = portread32(v);
	}
	else
	{
		D("in ax, 0x%x", v);
		r.ax = portread16(v);
	}
}

void i_E6()
{
	unsigned char v;
	if (!fetch8(&v))
		return;
	D("out 0x%x, al", v);
	portwrite8(v, r.al);
}

void i_E7()
{
	unsigned char v;
	if (!fetch8(&v))
		return;
	if (i32)
	{
		D("out 0x%x, eax", v);
		portwrite32(v, r.eax);
	}
	else
	{
		D("out 0x%x, ax", v);
		portwrite16(v, r.ax);
	}
}

void i_E8()
{
	int d;
	if (i32)
	{
		if (!fetch32s(&d))
			return;
		D("call %x", r.eip + d);
		if (push32(r.eip))
			r.eip += d;
	}
	else
	{
		if (!fetch16s(&d))
			return;
		D("call %x", r.ip + d);
		if (push16(r.ip))
		{
			r.ip += d;
			r.iph = 0;
		}
	}
}

void i_E9()
{
	int d;
	if (i32)
	{
		if (!fetch32s(&d))
			return;
		D("jmp %x", r.eip + d);
		r.eip += d;
	}
	else
	{
		if (!fetch16s(&d))
			return;
		D("jmp %x", (r.ip + d) & 0xFFFFu);
		r.ip += d;
		r.iph = 0;
	}
}

void i_EA()
{
	unsigned short ncs;
	unsigned short nip;
	unsigned int neip;

	if (i32)
	{
		if (!fetch32(&neip))
			return;
	}
	else
	{
		if (!fetch16(&nip))
			return;
		neip = nip;
	}
	if (!fetch16(&ncs))
		return;

	D("jmp far %.4X:%.4X", ncs, neip);

	far_jmp(ncs, neip);
}

void i_EB()
{
	int d;
	if (!fetch8s(&d))
		return;
	if (i32)
	{
		D("jmp %x", r.eip + d);
		r.eip += d;
	}
	else
	{
		D("jmp %x", (r.ip + d) & 0xFFFFu);
		r.ip += d;
		r.iph = 0;
	}
}

void i_EC()
{
	D("in al, dx");
	r.al = portread8(r.dx);
}

void i_ED()
{
	if (i32)
	{
		D("in eax, dx");
		r.eax = portread32(r.dx);
	}
	else
	{
		D("in ax, dx");
		r.ax = portread16(r.dx);
	}
}

void i_EE()
{
	D("out dx, al");
	portwrite8(r.dx, r.al);
}

void i_EF()
{
	if (i32)
	{
		D("out dx, eax");
		portwrite32(r.dx, r.eax);
	}
	else
	{
		D("out dx, ax");
		portwrite16(r.dx, r.ax);
	}
}

void i_F0()
{
	D("lock ");
	if (!fetch8(&opcode))
		return;
	instrs[opcode]();
}

void i_F1()
{
	D("int 1");
	interrupt(1, -1, INT_FLAGS_SOFT);
}

void i_F2()
{
	D("repne ");
	repne = 1;
	if (!fetch8(&opcode))
		return;
	instr_count[opcode]++;
	instrs[opcode]();
}

void i_F3()
{
	D("rep ");
	repe = 1;
	if (!fetch8(&opcode))
		return;
	instr_count[opcode]++;
	instrs[opcode]();
}

void i_F4()
{
	D("hlt");
	hlt = 1;
}

void i_F5()
{
	D("cmc");
	r.eflags ^= F_C;
}

void i_F6()
{
	unsigned char v, q, rm;
	unsigned int d;
	if (!mod(1))
		return;
	if (!readmod(&d))
		return;
	v = (unsigned char)d;
	switch ((modrm >> 3) & 7)
	{
		case 0:
			// test eb, i8
			if (!fetch8(&q))
				return;
			D("test ");
			disasm_mod();
			D(", 0x%x", q);
			test8(v, q);
			break;
		case 2:
			// not
			D("not ");
			disasm_mod();
			
			writemod(~v);
			break;
		case 3:
			// neg
			D("neg ");
			disasm_mod();
			
			q = sub8(0, v, 0);
			writemod(q);
			if (q == 0)
				r.flags &= ~F_C;
			else
				r.flags |= F_C;
			if (q != 0x80)
				r.flags &= ~F_O;
			else
				r.flags |= F_O;
			break;
		case 4:
			// mul8
			D("mul ");
			disasm_mod();
			
			r.ax = mul8(v, r.al);
			break;
		case 5:
			// imul8
			D("imul ");
			disasm_mod();
			
			r.ax = imul8(v, r.al);
			break;
		case 6:
			// div8
			D("div ");
			disasm_mod();
			
			div8(r.ax, v, &q, &rm);
			r.al = q;
			r.ah = rm;
			break;
		case 7:
			// idiv8
			D("idiv ");
			disasm_mod();
			
			idiv8(r.ax, v, (char *)&q, (char *)&rm);
			r.al = q;
			r.ah = rm;
			break;
	}
}

void i_F7()
{
	unsigned short v, q, rm;
	unsigned int v32, q32, rm32;
	unsigned int d;
	long long d64;
	unsigned char *pd64 = (unsigned char *)&d64;
	unsigned long long res64;
	unsigned char *pres64 = (unsigned char *)&res64;
	if (!mod(0))
		return;
	if (!readmod(&d))
		return;
	if (i32)
	{
		v32 = d;
		switch ((modrm >> 3) & 7)
		{
			case 0:
				// test ed, i32
				if (!fetch32(&d))
					return;
				D("test ");
				disasm_mod();
				D(", 0x%x", d);
				test32(v32, d);
				break;
			case 2:
				// not
				D("not ");
				disasm_mod();
				
				writemod(~v32);
				break;
			case 3:
				// neg
				D("neg ");
				disasm_mod();
				
				q32 = sub32(0, v32, 0);
				writemod(q32);
				if (q32 == 0)
					r.flags &= ~F_C;
				else
					r.flags |= F_C;
				if (q32 != 0x80000000u)
					r.flags &= ~F_O;
				else
					r.flags |= F_O;
				break;
			case 4:
				// mul8
				D("mul eax, ");
				disasm_mod();
				
				res64 = mul32(v32, r.eax);
				r.eax = (unsigned int)res64;
				r.edx = res64 >> 32ull;
				break;
			case 5:
				// imul8
				D("imul eax, ");
				disasm_mod();
				
				res64 = imul32(v32, r.eax);
				r.eax = (unsigned int)res64;
				r.edx = res64 >> 32ull;
				break;
			case 6:
				// div8
				D("div eax, ");
				disasm_mod();
				
				memcpy(pres64, &r.eax, 4);
				memcpy(&pres64[4], &r.edx, 4);
				div32(res64, v32, &q32, &rm32);
				r.eax = q32;
				r.edx = rm32;
				break;
			case 7:
				// idiv8
				D("idiv eax, ");
				disasm_mod();
				
				memcpy(pd64, &r.eax, 4);
				memcpy(&pd64[4], &r.edx, 4);
				idiv32((long long)(d64), (int)v32, (int *)&q32, (int *)&rm32);
				r.eax = q32;
				r.edx = rm32;
				break;
		}
	}
	else
	{
		v = (unsigned short)d;
		switch ((modrm >> 3) & 7)
		{
			case 0:
				// test ew, i16
				if (!fetch16(&q))
					return;
				D("test ");
				disasm_mod();
				D(", 0x%x", q);
				test16(v, q);
				break;
			case 2:
				// not
				D("not ");
				disasm_mod();
				
				writemod(~v);
				break;
			case 3:
				// neg
				D("neg ");
				disasm_mod();
				
				q = sub16(0, v, 0);
				writemod(q);
				if (q == 0)
					r.flags &= ~F_C;
				else
					r.flags |= F_C;
				if (q != 0x8000u)
					r.flags &= ~F_O;
				else
					r.flags |= F_O;
				break;
			case 4:
				// mul16
				D("mul ");
				disasm_mod();
				
				d = mul16(v, r.ax);
				r.ax = (unsigned short)d;
				r.dx = d >> 16u;
				break;
			case 5:
				// imul16
				D("imul ");
				disasm_mod();
				
				d = imul16(v, r.ax);
				r.ax = (unsigned short)d;
				r.dx = d >> 16u;
				break;
			case 6:
				// div16
				D("div ");
				disasm_mod();
				
				div16(r.ax | (r.dx << 16u), v, &q, &rm);
				r.ax = q;
				r.dx = rm;
				break;
			case 7:
				// idiv16
				D("idiv ");
				disasm_mod();
				
				idiv16(r.ax | (r.dx << 16u), v, (short *)&q, (short *)&rm);
				r.ax = q;
				r.dx = rm;
				break;
		}
	}
}

void i_F8()
{
	D("clc");
	r.eflags &= ~F_C;
}

void i_F9()
{
	D("stc");
	r.eflags |= F_C;
}

void i_FA()
{
	D("cli");
	cli();
}

void i_FB()
{
	D("sti");
	sti();
}

void i_FC()
{
	D("cld");
	set_flags(r.eflags & ~F_D, F_D);
}

void i_FD()
{
	D("std");
	set_flags(r.eflags | F_D, F_D);
}

void i_FE()
{
	unsigned int d;
	if (!mod(1))
		return;
	if (!readmod(&d))
		return;
	switch ((modrm >> 3) & 7)
	{
		case 0:
			// inc eb
			D("inc ");
			disasm_mod();
			
			writemod(inc8(d));
			break;
		case 1:
			// dec eb
			D("dec ");
			disasm_mod();
			
			writemod(dec8(d));
			break;
		default:
			undefined_instr();
			break;
	}
}

void i_FF()
{
	unsigned short a, b;
	unsigned int d, a32, b32;
	unsigned char hyper;
	
	if (!mod(0))
		return;

	if (i32)
	{
		switch ((modrm >> 3) & 7)
		{
			case 0:
				// inc ed
				D("inc ");
				disasm_mod();
				
				if (!readmod(&d))
					return;
				writemod(inc32(d));
				break;
			case 1:
				// dec ed
				D("dec ");
				disasm_mod();
				
				if (!readmod(&d))
					return;
				writemod(dec32(d));
				break;
			case 2:
				// call ea
				D("call ");
				disasm_mod();

				if (!readmod(&d))
					return;
				push32(r.eip);
				r.eip = d;
				break;
			case 3:
				// call far [ea]
				D("call far ");
				disasm_mod();
				
				if (!read32(sel, ofs, &a32))
					return;
				if (!read32(sel, ofs + 4, &b32))
					return;

				b32 &= 0xFFFFu;

				far_call(b32, a32);
				break;
			case 4:
				// jmp ea
				D("jmp ");
				disasm_mod();
				
				if (!readmod(&d))
					return;
				r.eip = d;
				break;
			case 5:
				// jmp far [ea]
				D("jmp far ");
				disasm_mod();
				
				if (!read32(sel, ofs, &a32))
					return;
				if (!read32(sel, ofs + 4, &b32))
					return;

				b32 &= 0xFFFFu;

				far_jmp(b32, a32);
				break;
			case 6:
				D("push ");
				disasm_mod();
				
				if (!readmod(&d))
					return;
				push32(d);
				break;
			default:
				undefined_instr();
				break;
		}
	}
	else
	{
		switch ((modrm >> 3) & 7)
		{
			case 0:
				// inc ew
				D("inc ");
				disasm_mod();
				
				if (!readmod(&d))
					return;
				writemod(inc16(d));
				break;
			case 1:
				// dec ew
				D("dec ");
				disasm_mod();
				
				if (!readmod(&d))
					return;
				writemod(dec16(d));
				break;
			case 2:
				// call ea
				D("call ");
				disasm_mod();
				if (!readmod(&d))
					return;
				push16(r.ip);
				r.eip = d;
				break;
			case 3:
				// call far [ea]
				D("call far ");
				disasm_mod();
				if (!read16(sel, ofs, &a))
					return;
				if (!read16(sel, ofs + 2, &b))
					return;

				far_call(b, a);
				break;
			case 4:
				// jmp ea
				D("jmp ");
				disasm_mod();
				
				if (!readmod(&d))
					return;
				r.eip = d & 0xFFFFu;
				break;
			case 5:
				// jmp far [ea]
				D("jmp far ");
				disasm_mod();
				
				if (!read16(sel, ofs, &a))
					return;
				if (!read16(sel, ofs + 2, &b))
					return;
				far_jmp(b, a);
				break;
			case 6:
				D("push ");
				disasm_mod();
				
				if (!readmod(&d))
					return;
				push16(d);
				break;
			default:
				D("hyper");
				if (!fetch8(&hyper))
					return;
				switch (hyper)
				{
					case 0x13:
						bios_disk();
						break;
					default:
						undefined_instr();
						break;
				}
				break;
		}
	}
}

void (*instrs[256])() = {
	&i_00, &i_01, &i_02, &i_03, &i_04, &i_05, &i_06, &i_07, &i_08, &i_09, &i_0A, &i_0B, &i_0C, &i_0D, &i_0E, &i_0F, 
	&i_10, &i_11, &i_12, &i_13, &i_14, &i_15, &i_16, &i_17, &i_18, &i_19, &i_1A, &i_1B, &i_1C, &i_1D, &i_1E, &i_1F, 
	&i_20, &i_21, &i_22, &i_23, &i_24, &i_25, &i_26, &i_27, &i_28, &i_29, &i_2A, &i_2B, &i_2C, &i_2D, &i_2E, &i_2F, 
	&i_30, &i_31, &i_32, &i_33, &i_34, &i_35, &i_36, &i_37, &i_38, &i_39, &i_3A, &i_3B, &i_3C, &i_3D, &i_3E, &i_3F, 
	&i_40, &i_41, &i_42, &i_43, &i_44, &i_45, &i_46, &i_47, &i_48, &i_49, &i_4A, &i_4B, &i_4C, &i_4D, &i_4E, &i_4F, 
	&i_50, &i_51, &i_52, &i_53, &i_54, &i_55, &i_56, &i_57, &i_58, &i_59, &i_5A, &i_5B, &i_5C, &i_5D, &i_5E, &i_5F, 
	&i_60, &i_61, &i_62, &i_63, &i_64, &i_65, &i_66, &i_67, &i_68, &i_69, &i_6A, &i_6B, &i_6C, &i_6D, &i_6E, &i_6F, 
	&i_70, &i_71, &i_72, &i_73, &i_74, &i_75, &i_76, &i_77, &i_78, &i_79, &i_7A, &i_7B, &i_7C, &i_7D, &i_7E, &i_7F, 
	&i_80, &i_81, &i_82, &i_83, &i_84, &i_85, &i_86, &i_87, &i_88, &i_89, &i_8A, &i_8B, &i_8C, &i_8D, &i_8E, &i_8F, 
	&i_90, &i_91, &i_92, &i_93, &i_94, &i_95, &i_96, &i_97, &i_98, &i_99, &i_9A, &i_9B, &i_9C, &i_9D, &i_9E, &i_9F, 
	&i_A0, &i_A1, &i_A2, &i_A3, &i_A4, &i_A5, &i_A6, &i_A7, &i_A8, &i_A9, &i_AA, &i_AB, &i_AC, &i_AD, &i_AE, &i_AF, 
	&i_B0, &i_B1, &i_B2, &i_B3, &i_B4, &i_B5, &i_B6, &i_B7, &i_B8, &i_B9, &i_BA, &i_BB, &i_BC, &i_BD, &i_BE, &i_BF, 
	&i_C0, &i_C1, &i_C2, &i_C3, &i_C4, &i_C5, &i_C6, &i_C7, &i_C8, &i_C9, &i_CA, &i_CB, &i_CC, &i_CD, &i_CE, &i_CF, 
	&i_D0, &i_D1, &i_D2, &i_D3, &i_D4, &i_D5, &i_D6, &i_D7, &i_D8, &i_D9, &i_DA, &i_DB, &i_DC, &i_DD, &i_DE, &i_DF, 
	&i_E0, &i_E1, &i_E2, &i_E3, &i_E4, &i_E5, &i_E6, &i_E7, &i_E8, &i_E9, &i_EA, &i_EB, &i_EC, &i_ED, &i_EE, &i_EF, 
	&i_F0, &i_F1, &i_F2, &i_F3, &i_F4, &i_F5, &i_F6, &i_F7, &i_F8, &i_F9, &i_FA, &i_FB, &i_FC, &i_FD, &i_FE, &i_FF
};
