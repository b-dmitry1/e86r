#ifndef CPU_H
#define CPU_H

#include "config.h"

#if (PC)
extern FILE *dasm;
extern FILE *c0;

#define D(...)		if ((DEBUG && dasm != NULL && 1)) { fprintf(dasm, __VA_ARGS__); }

#ifndef GP
#define GP(cond, code)	if ((cond)) { ex(EX_GP, code); if (DEBUG && dasm != NULL) fprintf(dasm, "\texception GP(%x): %s\n", code, #cond); return 0; }
#endif

#ifndef SS
#define SS(cond, code)	if ((cond)) { ex(EX_STACK, code); if (DEBUG && dasm != NULL) fprintf(dasm, "\texception SS(%x): %s\n", code, #cond); return 0; }
#endif

#ifndef NP
#define NP(cond, code)	if ((cond)) { ex(EX_SEGMENT_NOT_PRESENT, code); if (DEBUG && dasm != NULL) fprintf(dasm, "\texception NP(%x): %s\n", code, #cond); return 0; }
#endif

#ifndef TS
#define TS(cond, code)	if ((cond)) { ex(EX_INVALID_TSS, code); if (DEBUG && dasm != NULL) fprintf(dasm, "\texception TS(%x): %s\n", code, #cond); return 0; }
#endif

#ifndef GPV
#define GPV(cond, code)	if ((cond)) { ex(EX_GP, code); if (DEBUG && dasm != NULL) fprintf(dasm, "\texception GP(%x): %s\n", code, #cond); return; }
#endif

#else

#define D(...)		{}

#ifndef GP
#define GP(cond, code)	if ((cond)) { ex(EX_GP, code); return 0; }
#endif

#ifndef SS
#define SS(cond, code)	if ((cond)) { ex(EX_STACK, code); return 0; }
#endif

#ifndef NP
#define NP(cond, code)	if ((cond)) { ex(EX_SEGMENT_NOT_PRESENT, code); return 0; }
#endif

#ifndef TS
#define TS(cond, code)	if ((cond)) { ex(EX_INVALID_TSS, code); return 0; }
#endif

#ifndef GPV
#define GPV(cond, code)	if ((cond)) { ex(EX_GP, code); return; }
#endif

#endif


#define EX_DIVIDE				0
#define EX_DEBUG				1
#define EX_NMI					2
#define EX_BREAKPOINT			3
#define EX_OVERFLOW				4
#define EX_BOUND				5
#define EX_OPCODE				6
#define EX_COPROCESSOR_NA		7
#define EX_DOUBLE				8
#define EX_COPROCESSOR_SEG		9
#define EX_INVALID_TSS			10
#define EX_SEGMENT_NOT_PRESENT	11				
#define EX_STACK				12
#define EX_GP					13
#define EX_PAGE					14
#define EX_COPROCESSOR			16

#define F_C				0x01
#define F_P				0x04
#define F_A				0x10
#define F_Z				0x40
#define F_S				0x80
#define F_T				0x100
#define F_I				0x200
#define F_D				0x400
#define F_O				0x800
#define F_IOPL			0x3000
#define F_NT			0x4000
#define F_RF			0x10000
#define F_VM			0x20000
#define F_AC			0x40000
#define F_ID			0x200000

#define F_ARITH			(2|F_C|F_P|F_A|F_Z|F_S|F_O)
#define F_ALL			(F_ARITH|F_T|F_I|F_D|F_AC)
#define F_FULL			(F_ALL|F_IOPL|F_NT)

#define IOPL			((r.eflags >> 12) & 3)

#define CR0_PE			0x00000001
#define CR0_MP			0x00000002
#define CR0_EM			0x00000004
#define CR0_TS			0x00000008
#define CR0_ET			0x00000010
#define CR0_PG			0x80000000


#define DESCR_INVALID		0x00
#define DESCR_286_TSS_AVAIL	0x01
#define DESCR_LDT			0x02
#define DESCR_286_TSS_BUSY	0x03
#define DESCR_286_CALL_GATE 0x04
#define DESCR_TASK_GATE		0x05
#define DESCR_286_INT_GATE	0x06
#define DESCR_286_TRAP_GATE	0x07
#define DESCR_386_TSS_AVAIL	0x09
#define DESCR_386_TSS_BUSY	0x0B
#define DESCR_386_CALL_GATE	0x0C
#define DESCR_386_INT_GATE	0x0E
#define DESCR_386_TRAP_GATE	0x0F

#define DESCR_DATA_UP		0x10
#define DESCR_DATA_UP_A		0x11
#define DESCR_DATA_UP_W		0x12
#define DESCR_DATA_UP_W_A	0x13
#define DESCR_DATA_DN		0x14
#define DESCR_DATA_DN_A		0x15
#define DESCR_DATA_DN_W		0x16
#define DESCR_DATA_DN_W_A	0x17

#define DESCR_CODE			0x18
#define DESCR_CODE_A		0x19
#define DESCR_CODE_R		0x1A
#define DESCR_CODE_R_A		0x1B
#define DESCR_CODE_C		0x1C
#define DESCR_CODE_C_A		0x1D
#define DESCR_CODE_C_R		0x1E
#define DESCR_CODE_C_R_A	0x1F

union regs_t
{
	struct
	{
		unsigned char al;
		unsigned char ah;
		unsigned char reg_fill_a[2];
		unsigned char cl;
		unsigned char ch;
		unsigned char reg_fill_c[2];
		unsigned char dl;
		unsigned char dh;
		unsigned char reg_fill_d[2];
		unsigned char bl;
		unsigned char bh;
		unsigned char reg_fill_b[2];
	};
	struct
	{
		unsigned short ax;
		unsigned short axh;
		unsigned short cx;
		unsigned short cxh;
		unsigned short dx;
		unsigned short dxh;
		unsigned short bx;
		unsigned short bxh;
		unsigned short sp;
		unsigned short sph;
		unsigned short bp;
		unsigned short bph;
		unsigned short si;
		unsigned short sih;
		unsigned short di;
		unsigned short dih;
		unsigned short flags;
		unsigned short flagsh;
		unsigned short ip;
		unsigned short iph;
	};
	struct
	{
		unsigned int eax;
		unsigned int ecx;
		unsigned int edx;
		unsigned int ebx;
		unsigned int esp;
		unsigned int ebp;
		unsigned int esi;
		unsigned int edi;
		unsigned int eflags;
		unsigned int eip;
	};
	unsigned char r8[16];
	unsigned short r16[20];
	unsigned int r32[10];
};

typedef struct
{
	unsigned short back;
	unsigned short sp0;
	unsigned short ss0;
	unsigned short sp1;
	unsigned short ss1;
	unsigned short sp2;
	unsigned short ss2;
	unsigned short ip;
	unsigned short flags;
	unsigned short ax, cx, dx, bx;
	unsigned short sp, bp, si, di;
	unsigned short es, cs, ss, ds;
	unsigned short ldt;
} tss286_t;

typedef struct
{
	unsigned int back;
	unsigned int esp0;
	unsigned int ss0;
	unsigned int esp1;
	unsigned int ss1;
	unsigned int esp2;
	unsigned int ss2;
	unsigned int cr3;
	unsigned int eip;
	unsigned int eflags;
	unsigned int eax, ecx, edx, ebx;
	unsigned int esp, ebp, esi, edi;
	unsigned int es, cs, ss, ds, fs, gs;
	unsigned int ldt;
	unsigned int iomap;
} tss386_t;

typedef struct
{
	unsigned int limit0_15 : 16;
	unsigned int base0_15 : 16;
	unsigned int base16_23 : 8;
	unsigned int type : 5;
	unsigned int dpl : 2;
	unsigned int present : 1;
	unsigned int limit16_19 : 4;
	unsigned int user : 1;
	unsigned int reserved : 1;
	unsigned int big : 1;
	unsigned int gran4k : 1;
	unsigned int base24_31 : 8;
} descr_t;

typedef struct
{
	unsigned int offset0_15 : 16;
	unsigned int selector : 16;
	unsigned int paramcount : 5;
	unsigned int reserved : 3;
	unsigned int type : 5;
	unsigned int dpl : 2;
	unsigned int present : 1;
	unsigned int offset_16_31 : 16;
} gate_t;


typedef struct
{
	unsigned short value;
	unsigned int base;
	unsigned int limit;
	int big;
	unsigned int dpl;
	int type;
	int present;
	unsigned int mask;
	const char *name;
} selector_t;

extern unsigned char *ram;

extern regs_t r;
extern selector_t *sel;
extern selector_t *ssel;
extern selector_t es, cs, ss, ds, fs, gs;

extern unsigned int cr[8];
extern unsigned int dr[8];
extern unsigned int tr[8];

extern selector_t instr_cs, instr_ss;
extern unsigned int instr_eip;
extern unsigned int instr_esp;
extern unsigned int instr_fl;

extern unsigned int *dir;

extern unsigned short ldtr;

extern unsigned int gdt_base;
extern unsigned int gdt_limit;
extern unsigned int ldt_base;
extern unsigned int ldt_limit;
extern unsigned int idt_base;
extern unsigned int idt_limit;

extern unsigned short tss;
extern int tss286;
extern unsigned int tssbase;
extern unsigned int tsslimit;

extern int pmode;
extern int paging;
extern unsigned int cpl;

extern unsigned int stack_mask;
extern unsigned int stack_not_mask;

extern int i32;
extern int a32;
extern unsigned int a32mask;

extern int num_pf;
extern int num_gp;
extern int num_ex;
extern int num_math;

extern int dir1;
extern int dir2;
extern int dir4;

extern int repne;
extern int repe;

extern unsigned char opcode;

extern int terminated;

extern int irqs;

extern int hlt;

extern int a20;
extern unsigned int a20mask;

void dump(unsigned int addr);
void dump_descr(unsigned short sel);

void set_flags(unsigned int value, unsigned int mask);
void set_flags16(unsigned short value, unsigned int mask);

void reset();
void step();
void check_irqs();
void undefined_instr();

#endif
