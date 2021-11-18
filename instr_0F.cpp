#include "stdafx.h"
#include "cpu.h"
#include "interrupts.h"
#include "memdescr.h"
#include "transfer.h"
#include "modrm.h"
#include "alu.h"
#include "stringops.h"
#include "ioports.h"
#include "x86.h"

unsigned char opcode_0F;

void undefined(int n)
{
	undefined_instr();
	D("\tcode: %.2X\n", n);
}

void f_00()
{
	unsigned int d;
	descr_t desc;
	if (!mod(0))
		return;
	switch ((modrm >> 3) & 7)
	{
		case 0:
			D("sldt ");
			disasm_mod();
			writemod(ldtr);
			return;
		case 1:
			D("str ");
			disasm_mod();
			writemod(tss);
			return;
		case 2:
			D("lldt ");
			disasm_mod();
			if (!readmod(&d))
				return;
			if (d != 0)
			{
				if (!get_descr(&desc, d))
					return;
				ldtr = d;
				ldt_limit = get_limit(&desc);
				ldt_base = get_base_phys(&desc);
			}
			return;
		case 3:
			D("ltr ");
			disasm_mod();
			if (!readmod(&d))
				return;
			if (!set_tss(d))
				return;
			return;
		case 4:
			D("verr");
			if (!readmod(&d))
				return;
			verr(d);
			return;
		case 5:
			D("verw");
			if (!readmod(&d))
				return;
			verw(d);
			return;
	}
	undefined(0x00);
}

void f_01()
{
	unsigned short w;
	unsigned int d;
	if (!mod(0))
		return;
	switch ((modrm >> 3) & 7)
	{
		case 0:
			D("sgdt ");
			disasm_mod();
			write16(sel, ofs, gdt_limit);
			write32(sel, ofs + 2, gdt_base);
			return;
		case 1:
			D("sidt ");
			disasm_mod();
			write16(sel, ofs, idt_limit);
			// For 286
			// write32(sel, ofs + 2, idt_base | 0xFF000000u);
			// For 386
			write32(sel, ofs + 2, idt_base | 0x00000000u);
			return;
		case 2:
			D("lgdt ");
			disasm_mod();
			read16(sel, ofs, &w);
			gdt_limit = w;
			read32(sel, ofs + 2, &gdt_base);
			gdt_base &= 0xFFFFFFu;
			return;
		case 3:
			D("lidt ");
			disasm_mod();
			read16(sel, ofs, &w);
			idt_limit = w;
			read32(sel, ofs + 2, &idt_base);
			idt_base &= 0xFFFFFFu;
			return;
		case 4:
			D("smsw ");
			disasm_mod();
#ifdef DETECT_FPU
			cr[0] |= CR0_MP | CR0_ET;
#else
			cr[0] &= ~(CR0_MP | CR0_ET);
#endif
			writemod((unsigned short)cr[0]);
			return;
		case 6:
			D("lmsw ");
			disasm_mod();
			readmod(&d);
			lmsw(d);
			return;
	}
	undefined(0x01);
}

void f_02()
{
	unsigned int d, a[2];
	descr_t desc;
	if (!mod(0))
		return;
	D("lar ");
	disasm_mod();
	if (!readmod(&d))
		return;

	writemodreg(lar(d));
	return;
}

void f_03()
{
	unsigned int d;
	descr_t desc;
	if (!mod(0))
		return;
	D("lsl ");
	disasm_mod();
	if (!readmod(&d))
		return;

	writemodreg(lsl(d));
	return;
}

void f_04()
{
	undefined(0x04);
}

void f_05()
{
	undefined(0x05);
}

void f_06()
{
	D("clts");
	cr[0] &= ~8;
}

void f_07()
{
	undefined(0x07);
}

void f_08()
{
	undefined(0x08);
}

void f_09()
{
	undefined(0x09);
}

void f_0A()
{
	undefined(0x0A);
}

void f_0B()
{
	undefined(0x0B);
}

void f_0C()
{
	undefined(0x0C);
}

void f_0D()
{
	undefined(0x0D);
}

void f_0E()
{
	undefined(0x0E);
}

void f_0F()
{
	undefined(0x0F);
}

void f_10()
{
	undefined(0x10);
}

void f_11()
{
	undefined(0x11);
}

void f_12()
{
	undefined(0x12);
}

void f_13()
{
	undefined(0x13);
}

void f_14()
{
	undefined(0x14);
}

void f_15()
{
	undefined(0x15);
}

void f_16()
{
	undefined(0x16);
}

void f_17()
{
	undefined(0x17);
}

void f_18()
{
	undefined(0x18);
}

void f_19()
{
	undefined(0x19);
}

void f_1A()
{
	undefined(0x1A);
}

void f_1B()
{
	undefined(0x1B);
}

void f_1C()
{
	undefined(0x1C);
}

void f_1D()
{
	undefined(0x1D);
}

void f_1E()
{
	undefined(0x1E);
}

void f_1F()
{
	undefined(0x1F);
}

void f_20()
{
	if (!(mod(0)))
		return;
	D("mov ");
	disasm_mod();
	D(", cr%d", (modrm >> 3) & 7);
	i32 = 1;
#ifdef DETECT_FPU
	cr[0] |= CR0_MP | CR0_ET;
#else
	cr[0] &= ~(CR0_MP | CR0_ET);
#endif
	writemod(cr[(modrm >> 3) & 3]);
	i32 = 0;
}

void f_21()
{
	if (!(mod(0)))
		return;
	D("mov ");
	disasm_mod();
	D(", dr%d", (modrm >> 3) & 7);
}

void f_22()
{
	if (!(mod(0)))
		return;
	D("mov cr%d, ", (modrm >> 3) & 7);
	disasm_mod();
	i32 = 1;
	readmod(&cr[(modrm >> 3) & 3]);
#ifdef DETECT_FPU
	cr[0] |= CR0_MP | CR0_ET;
#else
	cr[0] &= ~(CR0_MP | CR0_ET);
#endif
	i32 = 0;
	pmode = (cr[0] & 1) != 0;
	paging = (cr[0] & 0x80000000u) != 0;
	dir = (unsigned int *)&ram[cr[3] & 0xFFFFF000u];
}

void f_23()
{
	if (!(mod(0)))
		return;
	D("mov dr%d, ", (modrm >> 3) & 7);
	disasm_mod();
}

void f_24()
{
	undefined(0x24);
}

void f_25()
{
	undefined(0x25);
}

void f_26()
{
	undefined(0x26);
}

void f_27()
{
	undefined(0x27);
}

void f_28()
{
	undefined(0x28);
}

void f_29()
{
	undefined(0x29);
}

void f_2A()
{
	undefined(0x2A);
}

void f_2B()
{
	undefined(0x2B);
}

void f_2C()
{
	undefined(0x2C);
}

void f_2D()
{
	undefined(0x2D);
}

void f_2E()
{
	undefined(0x2E);
}

void f_2F()
{
	undefined(0x2F);
}

void f_30()
{
	undefined(0x30);
}

void f_31()
{
	undefined(0x31);
}

void f_32()
{
	undefined(0x32);
}

void f_33()
{
	undefined(0x33);
}

void f_34()
{
	undefined(0x34);
}

void f_35()
{
	undefined(0x35);
}

void f_36()
{
	undefined(0x36);
}

void f_37()
{
	undefined(0x37);
}

void f_38()
{
	undefined(0x38);
}

void f_39()
{
	undefined(0x39);
}

void f_3A()
{
	undefined(0x3A);
}

void f_3B()
{
	undefined(0x3B);
}

void f_3C()
{
	undefined(0x3C);
}

void f_3D()
{
	undefined(0x3D);
}

void f_3E()
{
	undefined(0x3E);
}

void f_3F()
{
	undefined(0x3F);
}

void f_40()
{
	undefined(0x40);
}

void f_41()
{
	undefined(0x41);
}

void f_42()
{
	undefined(0x42);
}

void f_43()
{
	undefined(0x43);
}

void f_44()
{
	undefined(0x44);
}

void f_45()
{
	undefined(0x45);
}

void f_46()
{
	undefined(0x46);
}

void f_47()
{
	undefined(0x47);
}

void f_48()
{
	undefined(0x48);
}

void f_49()
{
	undefined(0x49);
}

void f_4A()
{
	undefined(0x4A);
}

void f_4B()
{
	undefined(0x4B);
}

void f_4C()
{
	undefined(0x4C);
}

void f_4D()
{
	undefined(0x4D);
}

void f_4E()
{
	undefined(0x4E);
}

void f_4F()
{
	undefined(0x4F);
}

void f_50()
{
	undefined(0x50);
}

void f_51()
{
	undefined(0x51);
}

void f_52()
{
	undefined(0x52);
}

void f_53()
{
	undefined(0x53);
}

void f_54()
{
	undefined(0x54);
}

void f_55()
{
	undefined(0x55);
}

void f_56()
{
	undefined(0x56);
}

void f_57()
{
	undefined(0x57);
}

void f_58()
{
	undefined(0x58);
}

void f_59()
{
	undefined(0x59);
}

void f_5A()
{
	undefined(0x5A);
}

void f_5B()
{
	undefined(0x5B);
}

void f_5C()
{
	undefined(0x5C);
}

void f_5D()
{
	undefined(0x5D);
}

void f_5E()
{
	undefined(0x5E);
}

void f_5F()
{
	undefined(0x5F);
}

void f_60()
{
	undefined(0x60);
}

void f_61()
{
	undefined(0x61);
}

void f_62()
{
	undefined(0x62);
}

void f_63()
{
	undefined(0x63);
}

void f_64()
{
	undefined(0x64);
}

void f_65()
{
	undefined(0x65);
}

void f_66()
{
	undefined(0x66);
}

void f_67()
{
	undefined(0x67);
}

void f_68()
{
	undefined(0x68);
}

void f_69()
{
	undefined(0x69);
}

void f_6A()
{
	undefined(0x6A);
}

void f_6B()
{
	undefined(0x6B);
}

void f_6C()
{
	undefined(0x6C);
}

void f_6D()
{
	undefined(0x6D);
}

void f_6E()
{
	undefined(0x6E);
}

void f_6F()
{
	undefined(0x6F);
}

void f_70()
{
	undefined(0x70);
}

void f_71()
{
	undefined(0x71);
}

void f_72()
{
	undefined(0x72);
}

void f_73()
{
	undefined(0x73);
}

void f_74()
{
	undefined(0x74);
}

void f_75()
{
	undefined(0x75);
}

void f_76()
{
	undefined(0x76);
}

void f_77()
{
	undefined(0x77);
}

void f_78()
{
	undefined(0x78);
}

void f_79()
{
	undefined(0x79);
}

void f_7A()
{
	undefined(0x7A);
}

void f_7B()
{
	undefined(0x7B);
}

void f_7C()
{
	undefined(0x7C);
}

void f_7D()
{
	undefined(0x7D);
}

void f_7E()
{
	undefined(0x7E);
}

void f_7F()
{
	undefined(0x7F);
}

void f_80()
{
	int d;
	if (!fetch16s(&d))
		return;
	D("jo %x", (r.ip + d) & 0xFFFFu);
	if (r.eflags & F_O)
		r.ip += d;
}

void f_81()
{
	int d;
	if (!fetch16s(&d))
		return;
	D("jno %x", (r.ip + d) & 0xFFFFu);
	if ((r.eflags & F_O) == 0)
		r.ip += d;
}

void f_82()
{
	int d;
	if (!fetch16s(&d))
		return;
	D("jc %x", (r.ip + d) & 0xFFFFu);
	if (r.eflags & F_C)
		r.ip += d;
}

void f_83()
{
	int d;
	if (!fetch16s(&d))
		return;
	D("jnc %x", (r.ip + d) & 0xFFFFu);
	if ((r.eflags & F_C) == 0)
		r.ip += d;
}

void f_84()
{
	int d;
	if (!fetch16s(&d))
		return;
	D("jz %x", (r.ip + d) & 0xFFFFu);
	if (r.eflags & F_Z)
		r.ip += d;
}

void f_85()
{
	int d;
	if (!fetch16s(&d))
		return;
	D("jnz %x", (r.ip + d) & 0xFFFFu);
	if ((r.eflags & F_Z) == 0)
		r.ip += d;
}

void f_86()
{
	int d;
	if (!fetch16s(&d))
		return;
	D("jbe %x", (r.ip + d) & 0xFFFFu);
	if (r.eflags & (F_Z | F_C))
		r.ip += d;
}

void f_87()
{
	int d;
	if (!fetch16s(&d))
		return;
	D("ja %x", (r.ip + d) & 0xFFFFu);
	if ((r.eflags & (F_Z | F_C)) == 0)
		r.ip += d;
}

void f_88()
{
	int d;
	if (!fetch16s(&d))
		return;
	D("js %x", (r.ip + d) & 0xFFFFu);
	if (r.eflags & F_S)
		r.ip += d;
}

void f_89()
{
	int d;
	if (!fetch16s(&d))
		return;
	D("jns %x", (r.ip + d) & 0xFFFFu);
	if ((r.eflags & F_S) == 0)
		r.ip += d;
}

void f_8A()
{
	int d;
	if (!fetch16s(&d))
		return;
	D("jp %x", (r.ip + d) & 0xFFFFu);
	if (r.eflags & F_P)
		r.ip += d;
}

void f_8B()
{
	int d;
	if (!fetch16s(&d))
		return;
	D("jnp %x", (r.ip + d) & 0xFFFFu);
	if ((r.eflags & F_P) == 0)
		r.ip += d;
}

void f_8C()
{
	int d;
	if (!fetch16s(&d))
		return;
	D("jl %x", (r.ip + d) & 0xFFFFu);
	if ((!(r.eflags & F_S)) != (!(r.eflags & F_O)))
		r.ip += d;
}

void f_8D()
{
	int d;
	if (!fetch16s(&d))
		return;
	D("jge %x", (r.ip + d) & 0xFFFFu);
	if ((!(r.eflags & F_S)) == (!(r.eflags & F_O)))
		r.ip += d;
}

void f_8E()
{
	int d;
	if (!fetch16s(&d))
		return;
	D("jle %x", (r.ip + d) & 0xFFFFu);
	if ((r.eflags & F_Z) || ((!(r.eflags & F_S)) != (!(r.eflags & F_O))))
		r.ip += d;
}

void f_8F()
{
	int d;
	if (!fetch16s(&d))
		return;
	D("jg %x", (r.ip + d) & 0xFFFFu);
	if (((r.eflags & F_Z) == 0) && ((!(r.eflags & F_S)) == (!(r.eflags & F_O))))
		r.ip += d;
}

void f_90()
{
	D("seto");
	mod(1);
	writemod(r.eflags & F_O ? 1 : 0);
	
}

void f_91()
{
	D("setno");
	mod(1);
	writemod(r.eflags & F_O ? 0 : 1);
}

void f_92()
{
	D("setc");
	mod(1);
	writemod(r.eflags & F_C ? 1 : 0);
}

void f_93()
{
	D("setnc");
	mod(1);
	writemod(r.eflags & F_C ? 0 : 1);
}

void f_94()
{
	D("setz");
	mod(1);
	writemod(r.eflags & F_Z ? 1 : 0);
}

void f_95()
{
	D("setnz");
	mod(1);
	writemod(r.eflags & F_Z ? 0 : 1);
}

void f_96()
{
	D("setbe");
	mod(1);
	writemod(r.eflags & (F_Z | F_C) ? 1 : 0);
}

void f_97()
{
	D("seta");
	mod(1);
	writemod(r.eflags & (F_Z | F_C) ? 0 : 1);
}

void f_98()
{
	D("sets");
	mod(1);
	writemod(r.eflags & F_S ? 1 : 0);
}

void f_99()
{
	D("setns");
	mod(1);
	writemod(r.eflags & F_S ? 0 : 1);
}

void f_9A()
{
	D("setp");
	mod(1);
	writemod(r.eflags & F_P ? 1 : 0);
}

void f_9B()
{
	D("setnp");
	mod(1);
	writemod(r.eflags & F_P ? 0 : 1);
}

void f_9C()
{
	D("setl");
	mod(1);
	writemod((!(r.eflags & F_S)) != (!(r.eflags & F_O)) ? 1 : 0);
}

void f_9D()
{
	D("setge");
	mod(1);
	writemod((!(r.eflags & F_S)) != (!(r.eflags & F_O)) ? 0 : 1);
}

void f_9E()
{
	D("setle");
	mod(1);
	writemod((r.eflags & F_Z) || ((!(r.eflags & F_S)) != (!(r.eflags & F_O))) ? 1 : 0);
}

void f_9F()
{
	D("setge");
	mod(1);
	writemod(((r.eflags & F_Z) == 0) && ((!(r.eflags & F_S)) == (!(r.eflags & F_O))) ? 1 : 0);
}

void f_A0()
{
	D("push fs");
	push16(fs.value);
}

void f_A1()
{
	unsigned short v;
	D("pop fs");
	if (!pop16(&v))
		return;
	set_selector(&fs, v, 1);
}

void f_A2()
{
	// CPUID
	D("cpuid");
	switch (r.eax)
	{
		case 0:
			r.eax = 1;
			r.ebx = 'G' | ('e' << 8u) | ('n' << 16u) | ('u' << 24u); 
			r.edx = 'i' | ('n' << 8u) | ('e' << 16u) | ('I' << 24u); 
			r.ecx = 'n' | ('t' << 8u) | ('e' << 16u) | ('l' << 24u); 

			r.ebx = 'A' | ('u' << 8u) | ('t' << 16u) | ('h' << 24u); 
			r.edx = 'e' | ('n' << 8u) | ('t' << 16u) | ('i' << 24u); 
			r.ecx = 'c' | ('A' << 8u) | ('M' << 16u) | ('D' << 24u); 

			break;
		case 1:
			r.eax = 0x402;
			r.ebx = 0;
			r.ecx = 0;
			r.edx = 0x00000000;
			break;
		default:
			r.eax = r.ebx = r.ecx = r.edx  =0;
			break;
	}
}

void f_A3()
{
	unsigned int v32, u32;
	if (!mod(0))
		return;
	D("bt ");
	disasm_mod();
	D(", ");
	disasm_modreg();
	v32 = readmodreg();
	
	bt16((int)v32);
}

void f_A4()
{
	unsigned int a, b;
	unsigned char n;
	D("shld ");
	if (!mod(0))
		return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	D(", ");
	if (!fetch8(&n))
		return;
	n &= 0x0F;
	if (!readmod(&a))
		return;
	b = readmodreg();
	writemod(dshl16(a, b, n));
}

void f_A5()
{
	unsigned int a, b;
	unsigned char n;
	D("shld ");
	if (!mod(0))
		return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	D(", cl");
	n = r.cl;
	n &= 0x0F;
	if (!readmod(&a))
		return;
	b = readmodreg();
	writemod(dshl16(a, b, n));
}

void f_A6()
{
	// ibts
	D("ibts");
	// mod(0);
	ex(EX_OPCODE);
	// undefined(0xA6);
}

void f_A7()
{
	// xbts
	D("xbts");
	// mod(0);
	ex(EX_OPCODE);
	// undefined(0xA6);
}

void f_A8()
{
	D("push gs");
	push16(gs.value);
}

void f_A9()
{
	unsigned short v;
	D("pop gs");
	if (!pop16(&v))
		return;
	set_selector(&gs, v, 1);
}

void f_AA()
{
	undefined(0xAA);
}

void f_AB()
{
	unsigned int v32, u32;
	unsigned int mask;
	int d;
	D("bts ");
	if (!mod(0))
		return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	v32 = readmodreg();
	
	bts16((int)v32);
}

void f_AC()
{
	unsigned int a, b;
	unsigned char n;
	D("shrd ");
	if (!mod(0))
		return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	D(", ");
	if (!fetch8(&n))
		return;
	n &= 0x0F;
	if (!readmod(&a))
		return;
	b = readmodreg();
	writemod(dshr16(a, b, n));
}

void f_AD()
{
	unsigned int a, b;
	unsigned char n;
	D("shrd ");
	if (!mod(0))
		return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	D(", cl");
	n = r.cl;
	n &= 0x0F;
	if (!readmod(&a))
		return;
	b = readmodreg();
	writemod(dshr16(a, b, n));
}

void f_AE()
{
	undefined(0xAE);
}

void f_AF()
{
	unsigned int d;
	// imul gd, ed
	if (!mod(0))
		return;
	D("imul ");
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (!readmod(&d))
		return;
	writemodreg((unsigned short)imul16(readmodreg(), d));
}

void f_B0()
{
	undefined(0xB0);
}

void f_B1()
{
	undefined(0xB1);
}

void f_B2()
{
	unsigned short s, o16;
	if (!mod(0))
		return;
	D("lds ");
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (!read16(sel, ofs, &o16))
		return;
	ofs += 2;
	if (!read16(sel, ofs, &s))
		return;
	if (!set_selector(&ds, s, 1))
		return;
	writemodreg(o16);
}

void f_B3()
{
	unsigned int v32, u32;
	unsigned int mask;
	int d;
	D("btr ");
	if (!mod(0))
		return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	v32 = readmodreg();
	
	btr16((int)v32);
}

void f_B4()
{
	unsigned short s, o16;
	unsigned int o32;
	if (!mod(0))
		return;
	D("lfs ");
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
		if (!set_selector(&fs, s, 1))
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
		if (!set_selector(&fs, s, 1))
			return;
		writemodreg(o16);
	}
}

void f_B5()
{
	unsigned short s, o16;
	unsigned int o32;
	if (!mod(0))
		return;
	D("lgs ");
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
		if (!set_selector(&gs, s, 1))
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
		if (!set_selector(&gs, s, 1))
			return;
		writemodreg(o16);
	}
}

void f_B6()
{
	unsigned int d;
	D("movzx ");
	if (!mod(1))
		return;
	if (!readmod(&d))
		return;
	modrm_byte = 0;
	disasm_modreg();
	modrm_byte = 1;
	D(", ");
	disasm_mod();
	modrm_byte = 0;
	writemodreg(d & 0xFF);
}

void f_B7()
{
	unsigned int d;
	D("movzx ");
	if (!mod(0))
		return;
	if (!readmod(&d))
		return;
	disasm_modreg();
	D(", ");
	disasm_mod();
	writemodreg(d & 0xFFFFu);
}

void f_B8()
{
	undefined(0xB8);
}

void f_B9()
{
	undefined(0xB9);
}

void f_BA()
{
	unsigned int d, v;
	unsigned char b;
	unsigned char op;
	if (!mod(0))
		return;
	if (!readmod(&d))
		return;
	if (!fetch8(&b))
		return;
	b &= 0x0F;
	v = 1u << b;
	if (d & v)
		r.flags |= F_C;
	else
		r.flags &= ~F_C;
	op = (modrm >> 3) & 7;
	switch (op)
	{
		case 4:
			D("bt ");
			disasm_mod();
			D(", %d", b);
			return;
		case 5:
			D("bts ");
			disasm_mod();
			D(", %d", b);
			writemod(d | v);
			return;
		case 6:
			D("btr ");
			disasm_mod();
			D(", %d", b);
			writemod(d & (~v));
			return;
		case 7:
			D("btc ");
			disasm_mod();
			D(", %d", b);
			writemod(d ^ v);
			return;
	}
}

void f_BB()
{
	undefined(0xBB);
}

void f_BC()
{
	unsigned int u32;
	unsigned int u;
	D("bsf ");
	if (!mod(0))
		return;
	disasm_mod();
	if (!readmod(&u32))
		return;

	writemodreg(bsf16(readmodreg(), u32));
}

void f_BD()
{
	unsigned int u32;
	unsigned int u;
	D("bsr ");
	if (!mod(0))
		return;
	disasm_mod();
	if (!readmod(&u32))
		return;

	writemodreg(bsr16(readmodreg(), u32));
}

void f_BE()
{
	unsigned int d;
	D("movsx ");
	if (!mod(1))
		return;
	if (!readmod(&d))
		return;
	modrm_byte = 0;
	disasm_modreg();
	modrm_byte = 1;
	D(", ");
	disasm_mod();
	modrm_byte = 0;
	if (d & 0x80)
		d |= 0xFFFFFF00u;
	writemodreg(d);
}

void f_BF()
{
	unsigned int d;
	D("movsx ");
	if (!mod(0))
		return;
	if (!readmod(&d))
		return;
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (d & 0x8000)
		d |= 0xFFFF0000u;
	writemodreg(d);
}

void f_C0()
{
	undefined(0xC0);
}

void f_C1()
{
	undefined(0xC1);
}

void f_C2()
{
	undefined(0xC2);
}

void f_C3()
{
	undefined(0xC3);
}

void f_C4()
{
	undefined(0xC4);
}

void f_C5()
{
	undefined(0xC5);
}

void f_C6()
{
	undefined(0xC6);
}

void f_C7()
{
	undefined(0xC7);
}

void f_C8()
{
	undefined(0xC8);
}

void f_C9()
{
	undefined(0xC9);
}

void f_CA()
{
	undefined(0xCA);
}

void f_CB()
{
	undefined(0xCB);
}

void f_CC()
{
	undefined(0xCC);
}

void f_CD()
{
	undefined(0xCD);
}

void f_CE()
{
	undefined(0xCE);
}

void f_CF()
{
	undefined(0xCF);
}

void f_D0()
{
	undefined(0xD0);
}

void f_D1()
{
	undefined(0xD1);
}

void f_D2()
{
	undefined(0xD2);
}

void f_D3()
{
	undefined(0xD3);
}

void f_D4()
{
	undefined(0xD4);
}

void f_D5()
{
	undefined(0xD5);
}

void f_D6()
{
	undefined(0xD6);
}

void f_D7()
{
	undefined(0xD7);
}

void f_D8()
{
	undefined(0xD8);
}

void f_D9()
{
	undefined(0xD9);
}

void f_DA()
{
	undefined(0xDA);
}

void f_DB()
{
	undefined(0xDB);
}

void f_DC()
{
	undefined(0xDC);
}

void f_DD()
{
	undefined(0xDD);
}

void f_DE()
{
	undefined(0xDE);
}

void f_DF()
{
	undefined(0xDF);
}

void f_E0()
{
	undefined(0xE0);
}

void f_E1()
{
	undefined(0xE1);
}

void f_E2()
{
	undefined(0xE2);
}

void f_E3()
{
	undefined(0xE3);
}

void f_E4()
{
	undefined(0xE4);
}

void f_E5()
{
	undefined(0xE5);
}

void f_E6()
{
	undefined(0xE6);
}

void f_E7()
{
	undefined(0xE7);
}

void f_E8()
{
	undefined(0xE8);
}

void f_E9()
{
	undefined(0xE9);
}

void f_EA()
{
	undefined(0xEA);
}

void f_EB()
{
	undefined(0xEB);
}

void f_EC()
{
	undefined(0xEC);
}

void f_ED()
{
	undefined(0xED);
}

void f_EE()
{
	undefined(0xEE);
}

void f_EF()
{
	undefined(0xEF);
}

void f_F0()
{
	undefined(0xF0);
}

void f_F1()
{
	undefined(0xF1);
}

void f_F2()
{
	undefined(0xF2);
}

void f_F3()
{
	undefined(0xF3);
}

void f_F4()
{
	undefined(0xF4);
}

void f_F5()
{
	undefined(0xF5);
}

void f_F6()
{
	undefined(0xF6);
}

void f_F7()
{
	undefined(0xF7);
}

void f_F8()
{
	undefined(0xF8);
}

void f_F9()
{
	undefined(0xF9);
}

void f_FA()
{
	undefined(0xFA);
}

void f_FB()
{
	undefined(0xFB);
}

void f_FC()
{
	undefined(0xFC);
}

void f_FD()
{
	undefined(0xFD);
}

void f_FE()
{
	undefined(0xFE);
}

void f_FF()
{
	undefined(0xFF);
}

void (*instrs_0F[256])() = {
	&f_00, &f_01, &f_02, &f_03, &f_04, &f_05, &f_06, &f_07, &f_08, &f_09, &f_0A, &f_0B, &f_0C, &f_0D, &f_0E, &f_0F, 
	&f_10, &f_11, &f_12, &f_13, &f_14, &f_15, &f_16, &f_17, &f_18, &f_19, &f_1A, &f_1B, &f_1C, &f_1D, &f_1E, &f_1F, 
	&f_20, &f_21, &f_22, &f_23, &f_24, &f_25, &f_26, &f_27, &f_28, &f_29, &f_2A, &f_2B, &f_2C, &f_2D, &f_2E, &f_2F, 
	&f_30, &f_31, &f_32, &f_33, &f_34, &f_35, &f_36, &f_37, &f_38, &f_39, &f_3A, &f_3B, &f_3C, &f_3D, &f_3E, &f_3F, 
	&f_40, &f_41, &f_42, &f_43, &f_44, &f_45, &f_46, &f_47, &f_48, &f_49, &f_4A, &f_4B, &f_4C, &f_4D, &f_4E, &f_4F, 
	&f_50, &f_51, &f_52, &f_53, &f_54, &f_55, &f_56, &f_57, &f_58, &f_59, &f_5A, &f_5B, &f_5C, &f_5D, &f_5E, &f_5F, 
	&f_60, &f_61, &f_62, &f_63, &f_64, &f_65, &f_66, &f_67, &f_68, &f_69, &f_6A, &f_6B, &f_6C, &f_6D, &f_6E, &f_6F, 
	&f_70, &f_71, &f_72, &f_73, &f_74, &f_75, &f_76, &f_77, &f_78, &f_79, &f_7A, &f_7B, &f_7C, &f_7D, &f_7E, &f_7F, 
	&f_80, &f_81, &f_82, &f_83, &f_84, &f_85, &f_86, &f_87, &f_88, &f_89, &f_8A, &f_8B, &f_8C, &f_8D, &f_8E, &f_8F, 
	&f_90, &f_91, &f_92, &f_93, &f_94, &f_95, &f_96, &f_97, &f_98, &f_99, &f_9A, &f_9B, &f_9C, &f_9D, &f_9E, &f_9F, 
	&f_A0, &f_A1, &f_A2, &f_A3, &f_A4, &f_A5, &f_A6, &f_A7, &f_A8, &f_A9, &f_AA, &f_AB, &f_AC, &f_AD, &f_AE, &f_AF, 
	&f_B0, &f_B1, &f_B2, &f_B3, &f_B4, &f_B5, &f_B6, &f_B7, &f_B8, &f_B9, &f_BA, &f_BB, &f_BC, &f_BD, &f_BE, &f_BF, 
	&f_C0, &f_C1, &f_C2, &f_C3, &f_C4, &f_C5, &f_C6, &f_C7, &f_C8, &f_C9, &f_CA, &f_CB, &f_CC, &f_CD, &f_CE, &f_CF, 
	&f_D0, &f_D1, &f_D2, &f_D3, &f_D4, &f_D5, &f_D6, &f_D7, &f_D8, &f_D9, &f_DA, &f_DB, &f_DC, &f_DD, &f_DE, &f_DF, 
	&f_E0, &f_E1, &f_E2, &f_E3, &f_E4, &f_E5, &f_E6, &f_E7, &f_E8, &f_E9, &f_EA, &f_EB, &f_EC, &f_ED, &f_EE, &f_EF, 
	&f_F0, &f_F1, &f_F2, &f_F3, &f_F4, &f_F5, &f_F6, &f_F7, &f_F8, &f_F9, &f_FA, &f_FB, &f_FC, &f_FD, &f_FE, &f_FF
};
