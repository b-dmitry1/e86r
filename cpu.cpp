#include "stdafx.h"
#include "cpu.h"
#include "memdescr.h"
#include "interrupts.h"
#include "ioports.h"
#include "disk.h"
#include "pic_pit.h"
#include "config.h"

unsigned char ram[RAM_SIZE];

extern unsigned char ports[65536];

regs_t r;
selector_t *sel;
selector_t *ssel;
selector_t es, cs, ss, ds, fs, gs;

unsigned int cr[8];
unsigned int dr[8];
unsigned int tr[8];

unsigned short ldtr = 0;

unsigned int gdt_base = 0;
unsigned int gdt_limit = 0xFFFF;
unsigned int ldt_base = 0;
unsigned int ldt_limit = 0xFFFF;
unsigned int idt_base = 0;
unsigned int idt_limit = 0x3FF;

int pmode = 0;
int paging = 0;
unsigned int cpl = 0;

unsigned int stack_mask = 0xFFFFu;
unsigned int stack_not_mask = 0xFFFF0000u;

unsigned short tss = 0;
int tss286 = 0;
unsigned int tssbase = 0;
unsigned int tsslimit = 0;

int i32 = 0;
int a32 = 0;
unsigned int a32mask = 0xFFFFFFFFu;

int dir1 = 1;
int dir2 = 2;
int dir4 = 4;

int irqs = 0;

int hlt = 0;

int a20 = 1;
unsigned int a20mask = 0xFFFFFFFFu;

int num_pf = 0;
int num_gp = 0;
int num_ex = 0;
int num_math = 0;

selector_t instr_cs, instr_ss;
unsigned int instr_eip;
unsigned int instr_esp;
unsigned int instr_fl;

unsigned char opcode;

extern void (*instrs[256])();

FILE *dasm = NULL;
FILE *c0 = NULL;

void set_flags(unsigned int value, unsigned int mask)
{
	// value &= F_ALL|F_IOPL;

	value &= ~(F_AC | F_ID);

#if (CPU == 286)
	if (!(cr[0] & 1))
		mask &= ~(F_NT | F_IOPL);
#endif

	// value &= 0x7FFF;

	value = (value & mask) | (r.eflags & (~mask));

	r.eflags = value;
	r.eflags |= 2;
	if (r.eflags & F_D)
	{
		dir1 = -1;
		dir2 = -2;
		dir4 = -4;
	}
	else
	{
		dir1 = 1;
		dir2 = 2;
		dir4 = 4;
	}

#if (CPU < 286)
	r.eflags &= 0xFFFF;
	r.eflags |= 0xF002;
#endif
}

void set_flags16(unsigned short value, unsigned int mask)
{
	set_flags((r.eflags & 0xFFFF0000u) | value, mask);
}

void dump(unsigned int addr)
{
	int i, j;
	unsigned int *p;
	for (i = 0; i < 128; i += 16)
	{
		p = (unsigned int *)&ram[addr + i];
		fprintf(c0, "\n%x  ", addr + i);
		for (j = 0; j < 4; j++)
			fprintf(c0, "%.8X ", p[j]);
	}
	fprintf(c0, "\n");
}

void dump_descr(unsigned short sel)
{
	descr_t d;
	gate_t g;

	if (sel & 0x04)
		memcpy(&d, &ram[ldtbase() + (sel & 0xFFF8u)], sizeof(d));
	else
		memcpy(&d, &ram[gdtbase() + (sel & 0xFFF8u)], sizeof(d));

	memcpy(&g, &d, sizeof(g));

	fprintf(c0, "\ndescr 0x%.4X: ", sel);
	if (d.present)
		fprintf(c0, "p, ");
	fprintf(c0, "ring: %d, 0x%x, lim: %d\n", d.dpl, get_base(&d), get_limit(&d));
	if (d.type < 16)
	{
		switch (d.type)
		{
			case 0: case 8: fprintf(c0, "---"); break;
			case 1: fprintf(c0, "Free 286 TSS"); break;
			case 2: fprintf(c0, "LDT"); break;
			case 3: fprintf(c0, "286 TSS"); break;
			case 4: fprintf(c0, "286 call gate, %.4X:%.8X", g.selector, g.offset0_15 + g.offset_16_31 * 65536u); break;
			case 5: fprintf(c0, "task gate, %.4X:%.8X", g.selector, g.offset0_15 + g.offset_16_31 * 65536u); break;
			case 6: fprintf(c0, "286 int gate, %.4X:%.8X", g.selector, g.offset0_15 + g.offset_16_31 * 65536u); break;
			case 7: fprintf(c0, "286 trap gate, %.4X:%.8X", g.selector, g.offset0_15 + g.offset_16_31 * 65536u); break;
			case 9: fprintf(c0, "Free 386 TSS"); break;
			case 10: fprintf(c0, "reserved"); break;
			case 11: fprintf(c0, "386 TSS"); break;
			case 12: fprintf(c0, "386 call gate, %.4X:%.8X", g.selector, g.offset0_15 + g.offset_16_31 * 65536u); break;
			case 13: fprintf(c0, "reserved"); break;
			case 14: fprintf(c0, "386 int gate, %.4X:%.8X", g.selector, g.offset0_15 + g.offset_16_31 * 65536u); break;
			case 15: fprintf(c0, "386 trap gate, %.4X:%.8X", g.selector, g.offset0_15 + g.offset_16_31 * 65536u); break;
		}
	}
	else
	{
		fprintf(c0, "dt[%.4X]: base %.8X, limit %.8X, gran = %s, %s, ring = %d, type = ", sel,
			d.base0_15 + d.base16_23 * 65536u + d.base24_31 * 16777216u,
			d.gran4k ? (d.limit0_15 + d.limit16_19 * 65536u) * 4096 + 4095 : d.limit0_15 + d.limit16_19 * 65536u,
			d.gran4k ? "  4K" : "byte",
			d.big ? "32bit" : "16bit",
			d.dpl);
		if (d.type & 0x08)
		{
			fprintf(c0, "code");
			if (d.type & 0x04)
				fprintf(c0, ", conforming");
			if (d.type & 0x02)
				fprintf(c0, ", read");
			if (d.type & 0x01)
				fprintf(c0, ", accessed");
		}
		else
		{
			fprintf(c0, "data");
			if (d.type & 0x04)
				fprintf(c0, ", expand down");
			else
				fprintf(c0, ", expand up");
			if (d.type & 0x02)
				fprintf(c0, ", write");
			if (d.type & 0x01)
				fprintf(c0, ", accessed");
		}
	}
	fprintf(c0, "\n");
}

void reset()
{
	int i;
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

	tss = 0;
	tss286 = 0;
	tssbase = 0;
	tsslimit = 0;

	r.eax = r.ecx = r.edx = r.ebx = r.esp = r.ebp = r.esi = r.edi = 0xCCCC;
	
	set_selector(&es, 0xFFFFu, 1);
	set_selector(&cs, 0xFFFFu, 1);
	set_selector(&ss, 0xFFFFu, 1);
	set_selector(&ds, 0xFFFFu, 1);
	set_selector(&fs, 0xFFFFu, 1);
	set_selector(&gs, 0xFFFFu, 1);

	es.name = "es";
	cs.name = "cs";
	ss.name = "ss";
	ds.name = "ds";
	fs.name = "fs";
	gs.name = "gs";
	
	// cr[0] |= CR0_EM;

	r.eip = 0;
#if (CPU < 286)
	set_flags(0xF002, 0xFFFFFFFFu);
#else
	set_flags(0x0002, 0xFFFFFFFFu);
#endif

	memset(ports, 0xff, sizeof(ports));

	/*
	ram[0xF1E6E] = CYLS & 0xFF;
	ram[0xF1E6F] = CYLS >> 8;
	ram[0xF1E70] = HEADS & 0xFF;
	ram[0xF1E7C] = SECTORS & 0xFF;
	*/
}

const char *r32names[10] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi", "fl", "eip"};

int cycle = 0;

int timer_en = 0;

extern int vmode;

int cyc = 0;

int instr_count[512] = {0};

int open_log = 0;

void step()
{
	static int nextint = 0;
	static int waitint = 0;
	static int wait_iret = 0;


	if (hlt && (irqs == 0))
		return;
	hlt = 0;

	repe = repne = 0;

	i32 = cs.big;
	a32 = cs.big;

	a32mask = cs.big ? 0xFFFFFFFFu : 0xFFFFu;

	sel = &ds;
	ssel = &ss;

	if (fault != 0)
	{
		interrupt(fault - 0x100, faultcode, INT_FLAGS_FAULT);
		fault = 0;
		// open_log = 1;
		return;
	}
	else
	{
		if ((r.eflags & F_I) && (!wait_iret))
		{
			nextint = get_next_irq_vector();
			if (nextint > 0)
			{
				interrupt(nextint, -1, 0);
				wait_iret = 1;
				return;
			}
		}

		instr_cs = cs;
		instr_eip = r.eip;
		instr_ss = ss;
		instr_esp = r.esp;
		instr_fl = r.eflags;
	}

	if ((dasm == NULL) && (open_log))
	{
		fopen_s(&dasm, "c:\\nasm\\v86\\386.dasm", "wt");

		cycle = 0;
	}

	if (!fetch8(&opcode))
		return;

	D("%.4X:%.8X       ", cs.value, instr_eip);

	if (opcode == 0xCF)
		wait_iret = 0;

	instrs[opcode]();

	cyc++;

	D("\n");

	if (DEBUG && (dasm != NULL) && 1)
	{
		D("  ax: %.8X\n", r.eax);
		D("  bx: %.8X\n", r.ebx);
		D("  cx: %.8X\n", r.ecx);
		D("  dx: %.8X\n", r.edx);
		D("  sp: %.8X\n", r.esp);
		D("  bp: %.8X\n", r.ebp);
		D("  si: %.8X\n", r.esi);
		D("  di: %.8X\n", r.edi);
		D("  ip: %.8X\n", r.eip);
		D("  fl: %.8X\n", r.eflags);
		D("  ds: %.4x\n", ds.value);
		D("  cr0: %.8x\n", cr[0]);
	}
}

extern HWND hWnd;
void undefined_instr()
{
	if (dasm == NULL)
		fopen_s(&dasm, "c:\\nasm\\v86\\386.dasm", "wt");
	D("\tundefined %.2X\n", opcode);
	terminated = 1;
	PostMessage(hWnd, WM_CLOSE, 0, 0);
}
