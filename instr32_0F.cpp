#include "stdafx.h"
#include "cpu.h"
#include "interrupts.h"
#include "memdescr.h"
#include "transfer.h"
#include "modrm.h"
#include "alu.h"
#include "x86.h"

extern unsigned char opcode_0F;

void undefined32(int n)
{
	undefined_instr();
	D("\tcode32: %.2X\n", n);
}

void f32_00()
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
			i32 = 0;
			writemod(ldtr);
			return;
		case 1:
			D("str ");
			disasm_mod();
			i32 = 0;
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
				ldt_base = get_base(&desc);
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
	undefined32(0x00 + ((modrm >> 3) & 7) * 256);
}

void f32_01()
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
			write32(sel, ofs + 2, idt_base);
			return;
		case 2:
			D("lgdt ");
			disasm_mod();
			if (!read16(sel, ofs, &w))
				return;
			if (!read32(sel, ofs + 2, &gdt_base))
				return;
			gdt_limit = w;
			return;
		case 3:
			D("lidt ");
			disasm_mod();
			if (!read16(sel, ofs, &w))
				return;
			if (!read32(sel, ofs + 2, &idt_base))
				return;
			idt_limit = w;
			return;
		case 4:
			D("smsw ");
			disasm_mod();
#if (FPU == 1)
			cr[0] |= CR0_MP | CR0_ET;
#else
			cr[0] &= ~(CR0_MP | CR0_ET);
#endif
			i32 = 0;
			writemod((unsigned short)cr[0] | 0x60000000);
			return;
		case 6:
			D("lmsw ");
			disasm_mod();
			readmod(&d);
			lmsw(d);
			return;
	}
	undefined32(0x01);
}

void f32_02()
{
	unsigned int u32;
	descr_t desc;
	unsigned int a[2];
	if (!mod(0))
		return;
	D("lar ");
	disasm_mod();
	if (!readmod(&u32))
		return;

	writemodreg(lar(u32));
	return;

	r.flags &= ~F_Z;
	if (u32 < 4)
		return;
	if (!get_descr(&desc, u32))
		return;
	if (desc.present)
	{
		memcpy(a, &desc, 8);
		writemodreg(a[1] & 0x00FFFF00u);
		r.flags |= F_Z;
	}
}

void f32_03()
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

	r.flags &= ~F_Z;
	if (d < 4)
		return;
	if (!get_descr(&desc, d))
		return;
	if (desc.present)
	{
		d = get_limit(&desc);
		writemodreg(d);
		r.flags |= F_Z;
	}
	else
	{
	}
}

void f32_04()
{
	undefined32(0x04);
}

void f32_05()
{
	undefined32(0x05);
}

void f32_06()
{
	D("clts");
	cr[0] &= ~CR0_TS;
}

void f32_07()
{
	undefined32(0x07);
}

void f32_08()
{
	undefined32(0x08);
}

void f32_09()
{
	undefined32(0x09);
}

void f32_0A()
{
	undefined32(0x0A);
}

void f32_0B()
{
	undefined32(0x0B);
}

void f32_0C()
{
	undefined32(0x0C);
}

void f32_0D()
{
	undefined32(0x0D);
}

void f32_0E()
{
	undefined32(0x0E);
}

void f32_0F()
{
	undefined32(0x0F);
}

void f32_10()
{
	undefined32(0x10);
}

void f32_11()
{
	undefined32(0x11);
}

void f32_12()
{
	undefined32(0x12);
}

void f32_13()
{
	undefined32(0x13);
}

void f32_14()
{
	undefined32(0x14);
}

void f32_15()
{
	undefined32(0x15);
}

void f32_16()
{
	undefined32(0x16);
}

void f32_17()
{
	undefined32(0x17);
}

void f32_18()
{
	undefined32(0x18);
}

void f32_19()
{
	undefined32(0x19);
}

void f32_1A()
{
	undefined32(0x1A);
}

void f32_1B()
{
	undefined32(0x1B);
}

void f32_1C()
{
	undefined32(0x1C);
}

void f32_1D()
{
	undefined32(0x1D);
}

void f32_1E()
{
	undefined32(0x1E);
}

void f32_1F()
{
	undefined32(0x1F);
}

void f32_20()
{
	if (!(mod(0)))
		return;
	D("mov ");
	disasm_mod();
	D(", cr%d", (modrm >> 3) & 7);
#if (FPU == 1)
	cr[0] |= CR0_MP | CR0_ET;
#else
	cr[0] &= ~(CR0_MP | CR0_ET);
#endif
	writemod(cr[(modrm >> 3) & 3]);
}

void f32_21()
{
	int n = (modrm >> 3) & 7;
	if (!(mod(0)))
		return;
	D("mov ");
	disasm_mod();
	D(", dr%d", (modrm >> 3) & 7);

	switch (n)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 6:
		case 7:
			writemod(dr[n]);
			break;
		case 4:
			writemod(dr[6]);
			break;
		case 5:
			writemod(dr[7]);
			break;
	}
}

void f32_22()
{
	if (!(mod(0)))
		return;
	D("mov cr%d, ", (modrm >> 3) & 7);
	disasm_mod();
	readmod(&cr[(modrm >> 3) & 3]);
#if (FPU == 1)
	cr[0] |= CR0_MP | CR0_ET;
#else
	cr[0] &= ~(CR0_MP | CR0_ET);
#endif
	pmode = (cr[0] & 1) != 0;
	paging = (cr[0] & 0x80000000u) != 0;
	dir = (unsigned int *)&ram[cr[3] & 0xFFFFF000u];
}

void f32_23()
{
	int n = (modrm >> 3) & 7;
	unsigned int v;
	if (!(mod(0)))
		return;
	
	D("mov dr%d, ", (modrm >> 3) & 7);
	disasm_mod();

	readmod(&v);

	switch (n)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			dr[n] = v;
			break;
		case 4:
		case 6:
			dr[6] = (v | 0xFFFF0FF0u) & 0xFFFFEFFFu;
			break;
		case 5:
		case 7:
			dr[7] = (v | 0x400) & 0xFFFF2FFFu;
			break;
	}
}

void f32_24()
{
	undefined32(0x24);
}

void f32_25()
{
	undefined32(0x25);
}

void f32_26()
{
	undefined32(0x26);
}

void f32_27()
{
	undefined32(0x27);
}

void f32_28()
{
	undefined32(0x28);
}

void f32_29()
{
	undefined32(0x29);
}

void f32_2A()
{
	undefined32(0x2A);
}

void f32_2B()
{
	undefined32(0x2B);
}

void f32_2C()
{
	undefined32(0x2C);
}

void f32_2D()
{
	undefined32(0x2D);
}

void f32_2E()
{
	undefined32(0x2E);
}

void f32_2F()
{
	undefined32(0x2F);
}

void f32_30()
{
	undefined32(0x30);
}

void f32_31()
{
	undefined32(0x31);
}

void f32_32()
{
	undefined32(0x32);
}

void f32_33()
{
	undefined32(0x33);
}

void f32_34()
{
	undefined32(0x34);
}

void f32_35()
{
	undefined32(0x35);
}

void f32_36()
{
	undefined32(0x36);
}

void f32_37()
{
	undefined32(0x37);
}

void f32_38()
{
	undefined32(0x38);
}

void f32_39()
{
	undefined32(0x39);
}

void f32_3A()
{
	undefined32(0x3A);
}

void f32_3B()
{
	undefined32(0x3B);
}

void f32_3C()
{
	undefined32(0x3C);
}

void f32_3D()
{
	undefined32(0x3D);
}

void f32_3E()
{
	undefined32(0x3E);
}

void f32_3F()
{
	undefined32(0x3F);
}

void f32_40()
{
	undefined32(0x40);
}

void f32_41()
{
	undefined32(0x41);
}

void f32_42()
{
	undefined32(0x42);
}

void f32_43()
{
	undefined32(0x43);
}

void f32_44()
{
	undefined32(0x44);
}

void f32_45()
{
	undefined32(0x45);
}

void f32_46()
{
	undefined32(0x46);
}

void f32_47()
{
	undefined32(0x47);
}

void f32_48()
{
	undefined32(0x48);
}

void f32_49()
{
	undefined32(0x49);
}

void f32_4A()
{
	undefined32(0x4A);
}

void f32_4B()
{
	undefined32(0x4B);
}

void f32_4C()
{
	undefined32(0x4C);
}

void f32_4D()
{
	undefined32(0x4D);
}

void f32_4E()
{
	undefined32(0x4E);
}

void f32_4F()
{
	undefined32(0x4F);
}

void f32_50()
{
	undefined32(0x50);
}

void f32_51()
{
	undefined32(0x51);
}

void f32_52()
{
	undefined32(0x52);
}

void f32_53()
{
	undefined32(0x53);
}

void f32_54()
{
	undefined32(0x54);
}

void f32_55()
{
	undefined32(0x55);
}

void f32_56()
{
	undefined32(0x56);
}

void f32_57()
{
	undefined32(0x57);
}

void f32_58()
{
	undefined32(0x58);
}

void f32_59()
{
	undefined32(0x59);
}

void f32_5A()
{
	undefined32(0x5A);
}

void f32_5B()
{
	undefined32(0x5B);
}

void f32_5C()
{
	undefined32(0x5C);
}

void f32_5D()
{
	undefined32(0x5D);
}

void f32_5E()
{
	undefined32(0x5E);
}

void f32_5F()
{
	undefined32(0x5F);
}

void f32_60()
{
	undefined32(0x60);
}

void f32_61()
{
	undefined32(0x61);
}

void f32_62()
{
	undefined32(0x62);
}

void f32_63()
{
	undefined32(0x63);
}

void f32_64()
{
	undefined32(0x64);
}

void f32_65()
{
	undefined32(0x65);
}

void f32_66()
{
	undefined32(0x66);
}

void f32_67()
{
	undefined32(0x67);
}

void f32_68()
{
	undefined32(0x68);
}

void f32_69()
{
	undefined32(0x69);
}

void f32_6A()
{
	undefined32(0x6A);
}

void f32_6B()
{
	undefined32(0x6B);
}

void f32_6C()
{
	undefined32(0x6C);
}

void f32_6D()
{
	undefined32(0x6D);
}

void f32_6E()
{
	undefined32(0x6E);
}

void f32_6F()
{
	undefined32(0x6F);
}

void f32_70()
{
	undefined32(0x70);
}

void f32_71()
{
	undefined32(0x71);
}

void f32_72()
{
	undefined32(0x72);
}

void f32_73()
{
	undefined32(0x73);
}

void f32_74()
{
	undefined32(0x74);
}

void f32_75()
{
	undefined32(0x75);
}

void f32_76()
{
	undefined32(0x76);
}

void f32_77()
{
	undefined32(0x77);
}

void f32_78()
{
	undefined32(0x78);
}

void f32_79()
{
	undefined32(0x79);
}

void f32_7A()
{
	undefined32(0x7A);
}

void f32_7B()
{
	undefined32(0x7B);
}

void f32_7C()
{
	undefined32(0x7C);
}

void f32_7D()
{
	undefined32(0x7D);
}

void f32_7E()
{
	undefined32(0x7E);
}

void f32_7F()
{
	undefined32(0x7F);
}

void f32_80()
{
	int d;
	if (!fetch32s(&d))
		return;
	D("jo %x", r.eip + d);
	if (r.eflags & F_O)
		r.eip += d;
}

void f32_81()
{
	int d;
	if (!fetch32s(&d))
		return;
	D("jno %x", r.eip + d);
	if ((r.eflags & F_O) == 0)
		r.eip += d;
}

void f32_82()
{
	int d;
	if (!fetch32s(&d))
		return;
	D("jb %x", r.eip + d);
	if (r.eflags & F_C)
		r.eip += d;
}

void f32_83()
{
	int d;
	if (!fetch32s(&d))
		return;
	D("jnb %x", r.eip + d);
	if ((r.eflags & F_C) == 0)
		r.eip += d;
}

void f32_84()
{
	int d;
	if (!fetch32s(&d))
		return;
	D("jz %x", r.eip + d);
	if (r.eflags & F_Z)
		r.eip += d;
}

void f32_85()
{
	int d;
	if (!fetch32s(&d))
		return;
	D("jnz %x", r.eip + d);
	if ((r.eflags & F_Z) == 0)
		r.eip += d;
}

void f32_86()
{
	int d;
	if (!fetch32s(&d))
		return;
	D("jbe %x", r.eip + d);
	if (r.eflags & (F_Z | F_C))
		r.eip += d;
}

void f32_87()
{
	int d;
	if (!fetch32s(&d))
		return;
	D("jnbe %x", r.eip + d);
	if ((r.eflags & (F_Z | F_C)) == 0)
		r.eip += d;
}

void f32_88()
{
	int d;
	if (!fetch32s(&d))
		return;
	D("js %x", r.eip + d);
	if (r.eflags & F_S)
		r.eip += d;
}

void f32_89()
{
	int d;
	if (!fetch32s(&d))
		return;
	D("jns %x", r.eip + d);
	if ((r.eflags & F_S) == 0)
		r.eip += d;
}

void f32_8A()
{
	int d;
	if (!fetch32s(&d))
		return;
	D("jp %x", r.eip + d);
	if (r.eflags & F_P)
		r.eip += d;
}

void f32_8B()
{
	int d;
	if (!fetch32s(&d))
		return;
	D("jnp %x", r.eip + d);
	if ((r.eflags & F_P) == 0)
		r.eip += d;
}

void f32_8C()
{
	int d;
	if (!fetch32s(&d))
		return;
	D("jl %x", r.eip + d);
	if ((!(r.eflags & F_S)) != (!(r.eflags & F_O)))
		r.eip += d;
}

void f32_8D()
{
	int d;
	if (!fetch32s(&d))
		return;
	D("jnl %x", r.eip + d);
	if ((!(r.eflags & F_S)) == (!(r.eflags & F_O)))
		r.eip += d;
}

void f32_8E()
{
	int d;
	if (!fetch32s(&d))
		return;
	D("jle %x", r.eip + d);
	if ((r.eflags & F_Z) || ((!(r.eflags & F_S)) != (!(r.eflags & F_O))))
		r.eip += d;
}

void f32_8F()
{
	int d;
	if (!fetch32s(&d))
		return;
	D("jnle %x", r.eip + d);
	if (((r.eflags & F_Z) == 0) && ((!(r.eflags & F_S)) == (!(r.eflags & F_O))))
		r.eip += d;
}

void f32_90()
{
	D("seto");
	mod(1);
	writemod(r.eflags & F_O ? 1 : 0);
	
}

void f32_91()
{
	D("setno");
	mod(1);
	writemod(r.eflags & F_O ? 0 : 1);
}

void f32_92()
{
	D("setb ");
	mod(1);
	disasm_mod();
	writemod(r.eflags & F_C ? 1 : 0);
}

void f32_93()
{
	D("setnb ");
	mod(1);
	disasm_mod();
	writemod(r.eflags & F_C ? 0 : 1);
}

void f32_94()
{
	D("setz");
	mod(1);
	writemod(r.eflags & F_Z ? 1 : 0);
}

void f32_95()
{
	D("setnz");
	mod(1);
	writemod(r.eflags & F_Z ? 0 : 1);
}

void f32_96()
{
	D("setbe");
	mod(1);
	writemod(r.eflags & (F_Z | F_C) ? 1 : 0);
}

void f32_97()
{
	D("seta");
	mod(1);
	writemod(r.eflags & (F_Z | F_C) ? 0 : 1);
}

void f32_98()
{
	D("sets");
	mod(1);
	writemod(r.eflags & F_S ? 1 : 0);
}

void f32_99()
{
	D("setns");
	mod(1);
	writemod(r.eflags & F_S ? 0 : 1);
}

void f32_9A()
{
	D("setp");
	mod(1);
	writemod(r.eflags & F_P ? 1 : 0);
}

void f32_9B()
{
	D("setnp");
	mod(1);
	writemod(r.eflags & F_P ? 0 : 1);
}

void f32_9C()
{
	D("setl");
	mod(1);
	writemod((!(r.eflags & F_S)) != (!(r.eflags & F_O)) ? 1 : 0);
}

void f32_9D()
{
	D("setge");
	mod(1);
	writemod((!(r.eflags & F_S)) == (!(r.eflags & F_O)) ? 1 : 0);
}

void f32_9E()
{
	D("setle");
	mod(1);
	writemod((r.eflags & F_Z) || ((!(r.eflags & F_S)) != (!(r.eflags & F_O))) ? 1 : 0);
}

void f32_9F()
{
	D("setge");
	mod(1);
	writemod(((r.eflags & F_Z) == 0) && ((!(r.eflags & F_S)) == (!(r.eflags & F_O))) ? 1 : 0);
}

void f32_A0()
{
	D("push fs");
	push32(fs.value);
}

void f32_A1()
{
	unsigned int d;
	D("pop fs");
	if (!pop32(&d))
		return;
	set_selector(&fs, d, 1);
}

void f32_A2()
{
	// CPUID
	undefined32(0xA2);
}

void f32_A3()
{
	unsigned int v32;
	if (!mod(0))
		return;

	D("bt ");
	disasm_mod();
	D(", ");
	disasm_modreg();

	v32 = readmodreg();

	bt32((int)v32);
}

void f32_A4()
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
	n &= 0x1F;
	if (!readmod(&a))
		return;
	b = readmodreg();
	writemod(dshl32(a, b, n));
}

void f32_A5()
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
	n &= 0x1F;
	if (!readmod(&a))
		return;
	b = readmodreg();
	writemod(dshl32(a, b, n));
}

void f32_A6()
{
	// ibts
	// ex(EX_OPCODE);
	undefined32(0xA6);
}

void f32_A7()
{
	// xbts
	// ex(EX_OPCODE);
	undefined32(0xA7);
}

void f32_A8()
{
	D("push gs");
	push32(gs.value);
}

void f32_A9()
{
	unsigned int d;
	D("pop gs");
	if (!pop32(&d))
		return;
	set_selector(&gs, d, 1);
}

void f32_AA()
{
	undefined32(0xAA);
}

void f32_AB()
{
	unsigned int v32;
	D("bts ");
	if (!mod(0))
		return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	v32 = readmodreg();

	bts32((int)v32);
}

void f32_AC()
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
	n &= 0x1F;
	if (!readmod(&a))
		return;
	b = readmodreg();
	writemod(dshr32(a, b, n));
}

void f32_AD()
{
	unsigned int a, b;
	unsigned char n;
	D("shrd ");
	if (!mod(0))
		return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	D(", CL");
	n = r.ecx;
	n &= 0x1F;
	if (!readmod(&a))
		return;
	b = readmodreg();
	writemod(dshr32(a, b, n));
}

void f32_AE()
{
	undefined32(0xAE);
}

void f32_AF()
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
	writemodreg((unsigned int)imul32(readmodreg(), d));
}

void f32_B0()
{
	undefined32(0xB0);
}

void f32_B1()
{
	undefined32(0xB1);
}

void f32_B2()
{
	unsigned short s;
	unsigned int o32;
	if (!mod(0))
		return;
	D("lss ");
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (dasm != NULL)
		fflush(dasm);
	if (!read32(sel, ofs, &o32))
		return;
	ofs += 4;
	if (!read16(sel, ofs, &s))
		return;
	if (!set_selector(&ss, s, 1))
		return;
	writemodreg(o32);
}

void f32_B3()
{
	unsigned int v32;
	D("btr ");
	if (!mod(0))
		return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	v32 = readmodreg();

	btr32((int)v32);
}

void f32_B4()
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

void f32_B5()
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

void f32_B6()
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

void f32_B7()
{
	unsigned int d;
	D("movzx ");
	if (!mod(0))
		return;
	i32 = 0;
	if (!readmod(&d))
	{
		i32 = 1;
		return;
	}
	i32 = 1;
	disasm_modreg();
	D(", ");
	disasm_mod();
	writemodreg(d & 0xFFFFu);
}

void f32_B8()
{
	undefined32(0xB8);
}

void f32_B9()
{
	undefined32(0xB9);
}

void f32_BA()
{
	unsigned char b;
	unsigned char op;
	if (!mod(0))
		return;
	if (!fetch8(&b))
		return;
	b &= 31;
	op = (modrm >> 3) & 7;
	switch (op)
	{
		case 4:
			D("bt ");
			disasm_mod();
			D(", %d", b);
			bt32(b);
			return;
		case 5:
			D("bts ");
			disasm_mod();
			D(", %d", b);
			bts32(b);
			return;
		case 6:
			D("btr ");
			disasm_mod();
			D(", %d", b);
			btr32(b);
			return;
		case 7:
			D("btc ");
			disasm_mod();
			D(", %d", b);
			btc32(b);
			return;
	}
}

void f32_BB()
{
	unsigned int v32;
	D("btc ");
	if (!mod(0))
		return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	v32 = readmodreg();

	btc32((int)v32);
}

void f32_BC()
{
	unsigned int u32;
	D("bsf ");
	if (!mod(0))
		return;
	disasm_mod();
	if (!readmod(&u32))
		return;

	writemodreg(bsf32(readmodreg(), u32));
}

void f32_BD()
{
	unsigned int u32;
	D("bsr ");
	if (!mod(0))
		return;
	disasm_mod();
	if (!readmod(&u32))
		return;

	writemodreg(bsr32(readmodreg(), u32));
}

void f32_BE()
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

void f32_BF()
{
	unsigned int d;
	D("movsx ");
	if (!mod(0))
		return;
	i32 = 0;
	if (!readmod(&d))
	{
		i32 = 1;
		return;
	}
	i32 = 1;
	disasm_modreg();
	D(", ");
	disasm_mod();
	if (d & 0x8000)
		d |= 0xFFFF0000u;
	writemodreg(d);
}

void f32_C0()
{
	undefined32(0xC0);
}

void f32_C1()
{
	// xadd
	undefined32(0xC0);
	return;
	unsigned int d, s;
	D("xadd ");
	if (!mod(1)) return;
	disasm_mod();
	D(", ");
	disasm_modreg();
	if (!readmod(&s)) return;
	d = readmodreg();
	if (!writemod(d+s)) return;
	writemodreg(s);
}

void f32_C2()
{
	undefined32(0xC2);
}

void f32_C3()
{
	undefined32(0xC3);
}

void f32_C4()
{
	undefined32(0xC4);
}

void f32_C5()
{
	undefined32(0xC5);
}

void f32_C6()
{
	undefined32(0xC6);
}

void f32_C7()
{
	undefined32(0xC7);
}

void f32_C8()
{
	r.eax = bswap32(r.eax);
}

void f32_C9()
{
	r.ecx = bswap32(r.ecx);
}

void f32_CA()
{
	r.edx = bswap32(r.edx);
}

void f32_CB()
{
	r.ebx = bswap32(r.ebx);
}

void f32_CC()
{
	r.esp = bswap32(r.esp);
}

void f32_CD()
{
	r.ebp = bswap32(r.ebp);
}

void f32_CE()
{
	r.esi = bswap32(r.esi);
}

void f32_CF()
{
	r.edi = bswap32(r.edi);
}

void f32_D0()
{
	undefined32(0xD0);
}

void f32_D1()
{
	undefined32(0xD1);
}

void f32_D2()
{
	undefined32(0xD2);
}

void f32_D3()
{
	undefined32(0xD3);
}

void f32_D4()
{
	undefined32(0xD4);
}

void f32_D5()
{
	undefined32(0xD5);
}

void f32_D6()
{
	undefined32(0xD6);
}

void f32_D7()
{
	undefined32(0xD7);
}

void f32_D8()
{
	undefined32(0xD8);
}

void f32_D9()
{
	undefined32(0xD9);
}

void f32_DA()
{
	undefined32(0xDA);
}

void f32_DB()
{
	undefined32(0xDB);
}

void f32_DC()
{
	undefined32(0xDC);
}

void f32_DD()
{
	undefined32(0xDD);
}

void f32_DE()
{
	undefined32(0xDE);
}

void f32_DF()
{
	undefined32(0xDF);
}

void f32_E0()
{
	undefined32(0xE0);
}

void f32_E1()
{
	undefined32(0xE1);
}

void f32_E2()
{
	undefined32(0xE2);
}

void f32_E3()
{
	undefined32(0xE3);
}

void f32_E4()
{
	undefined32(0xE4);
}

void f32_E5()
{
	undefined32(0xE5);
}

void f32_E6()
{
	undefined32(0xE6);
}

void f32_E7()
{
	undefined32(0xE7);
}

void f32_E8()
{
	undefined32(0xE8);
}

void f32_E9()
{
	undefined32(0xE9);
}

void f32_EA()
{
	undefined32(0xEA);
}

void f32_EB()
{
	undefined32(0xEB);
}

void f32_EC()
{
	undefined32(0xEC);
}

void f32_ED()
{
	undefined32(0xED);
}

void f32_EE()
{
	undefined32(0xEE);
}

void f32_EF()
{
	undefined32(0xEF);
}

void f32_F0()
{
	undefined32(0xF0);
}

void f32_F1()
{
	undefined32(0xF1);
}

void f32_F2()
{
	undefined32(0xF2);
}

void f32_F3()
{
	undefined32(0xF3);
}

void f32_F4()
{
	undefined32(0xF4);
}

void f32_F5()
{
	undefined32(0xF5);
}

void f32_F6()
{
	undefined32(0xF6);
}

void f32_F7()
{
	undefined32(0xF7);
}

void f32_F8()
{
	undefined32(0xF8);
}

void f32_F9()
{
	undefined32(0xF9);
}

void f32_FA()
{
	undefined32(0xFA);
}

void f32_FB()
{
	undefined32(0xFB);
}

void f32_FC()
{
	undefined32(0xFC);
}

void f32_FD()
{
	undefined32(0xFD);
}

void f32_FE()
{
	undefined32(0xFE);
}

void f32_FF()
{
	undefined32(0xFF);
}

void (*instrs32_0F[256])() = {
	&f32_00, &f32_01, &f32_02, &f32_03, &f32_04, &f32_05, &f32_06, &f32_07, &f32_08, &f32_09, &f32_0A, &f32_0B, &f32_0C, &f32_0D, &f32_0E, &f32_0F, 
	&f32_10, &f32_11, &f32_12, &f32_13, &f32_14, &f32_15, &f32_16, &f32_17, &f32_18, &f32_19, &f32_1A, &f32_1B, &f32_1C, &f32_1D, &f32_1E, &f32_1F, 
	&f32_20, &f32_21, &f32_22, &f32_23, &f32_24, &f32_25, &f32_26, &f32_27, &f32_28, &f32_29, &f32_2A, &f32_2B, &f32_2C, &f32_2D, &f32_2E, &f32_2F, 
	&f32_30, &f32_31, &f32_32, &f32_33, &f32_34, &f32_35, &f32_36, &f32_37, &f32_38, &f32_39, &f32_3A, &f32_3B, &f32_3C, &f32_3D, &f32_3E, &f32_3F, 
	&f32_40, &f32_41, &f32_42, &f32_43, &f32_44, &f32_45, &f32_46, &f32_47, &f32_48, &f32_49, &f32_4A, &f32_4B, &f32_4C, &f32_4D, &f32_4E, &f32_4F, 
	&f32_50, &f32_51, &f32_52, &f32_53, &f32_54, &f32_55, &f32_56, &f32_57, &f32_58, &f32_59, &f32_5A, &f32_5B, &f32_5C, &f32_5D, &f32_5E, &f32_5F, 
	&f32_60, &f32_61, &f32_62, &f32_63, &f32_64, &f32_65, &f32_66, &f32_67, &f32_68, &f32_69, &f32_6A, &f32_6B, &f32_6C, &f32_6D, &f32_6E, &f32_6F, 
	&f32_70, &f32_71, &f32_72, &f32_73, &f32_74, &f32_75, &f32_76, &f32_77, &f32_78, &f32_79, &f32_7A, &f32_7B, &f32_7C, &f32_7D, &f32_7E, &f32_7F, 
	&f32_80, &f32_81, &f32_82, &f32_83, &f32_84, &f32_85, &f32_86, &f32_87, &f32_88, &f32_89, &f32_8A, &f32_8B, &f32_8C, &f32_8D, &f32_8E, &f32_8F, 
	&f32_90, &f32_91, &f32_92, &f32_93, &f32_94, &f32_95, &f32_96, &f32_97, &f32_98, &f32_99, &f32_9A, &f32_9B, &f32_9C, &f32_9D, &f32_9E, &f32_9F, 
	&f32_A0, &f32_A1, &f32_A2, &f32_A3, &f32_A4, &f32_A5, &f32_A6, &f32_A7, &f32_A8, &f32_A9, &f32_AA, &f32_AB, &f32_AC, &f32_AD, &f32_AE, &f32_AF, 
	&f32_B0, &f32_B1, &f32_B2, &f32_B3, &f32_B4, &f32_B5, &f32_B6, &f32_B7, &f32_B8, &f32_B9, &f32_BA, &f32_BB, &f32_BC, &f32_BD, &f32_BE, &f32_BF, 
	&f32_C0, &f32_C1, &f32_C2, &f32_C3, &f32_C4, &f32_C5, &f32_C6, &f32_C7, &f32_C8, &f32_C9, &f32_CA, &f32_CB, &f32_CC, &f32_CD, &f32_CE, &f32_CF, 
	&f32_D0, &f32_D1, &f32_D2, &f32_D3, &f32_D4, &f32_D5, &f32_D6, &f32_D7, &f32_D8, &f32_D9, &f32_DA, &f32_DB, &f32_DC, &f32_DD, &f32_DE, &f32_DF, 
	&f32_E0, &f32_E1, &f32_E2, &f32_E3, &f32_E4, &f32_E5, &f32_E6, &f32_E7, &f32_E8, &f32_E9, &f32_EA, &f32_EB, &f32_EC, &f32_ED, &f32_EE, &f32_EF, 
	&f32_F0, &f32_F1, &f32_F2, &f32_F3, &f32_F4, &f32_F5, &f32_F6, &f32_F7, &f32_F8, &f32_F9, &f32_FA, &f32_FB, &f32_FC, &f32_FD, &f32_FE, &f32_FF
};
