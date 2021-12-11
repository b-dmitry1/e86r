#include "stdafx.h"
#include "interrupts.h"
#include "cpu.h"
#include "memdescr.h"
#include "disk.h"
#include "transfer.h"

int fault = 0;
unsigned int faultcode = 0;

void block_move()
{
	unsigned char *t = (unsigned char *)&ram[es.value * 16 + r.si];
	unsigned short *w = (unsigned short *)t;
	unsigned int src, dst;
	int count = r.cx * 2;
	w += 8;

	src = t[0x12] + t[0x13] * 256 + t[0x14] * 65536u;
	dst = t[0x1A] + t[0x1B] * 256 + t[0x1C] * 65536u;

	memcpy(&ram[dst], &ram[src], count);
}

void get_ss_esp(int dpl, unsigned int *nss, unsigned int *nesp)
{
	unsigned short temp16;
	unsigned int temp32;
	unsigned int a = tssbase;

	if (tss286)
	{
		a += 2 + dpl * 4;
		read16(a, &temp16);
		*nesp = temp16;
		read16(a + 2, &temp16);
		*nss = temp16;
		return;
	}
	a += 4 + dpl * 8;
	read32(a, &temp32);
	*nesp = temp32;
	read32(a + 4, &temp32);
	*nss = (unsigned short)temp32;
}

extern int cycle;

void interrupt(int n, int errorcode, int intflags)
{
	unsigned short cs16, ip16;
	gate_t g;
	descr_t csd, ssd;
	unsigned int fl = r.eflags;

	unsigned int oss = ss.value, oesp = r.esp, nss, nesp;
	unsigned int ocs = cs.value, oeip = r.eip;

	if (!pmode)
	{
		if (n == 0x13)
		{
			bios_disk();
			r.flags |= F_I;
			return;
		}

		if ((n == 0x15) && (r.ah == 0x87))
		{
			block_move();
			r.ah = 0;
			r.flags &= ~F_C;
			return;
		}

		push16(r.flags);
		push16(cs.value);
		push16(r.ip);
		r.flags &= ~(F_I | F_T);
		read16(idt_base + n * 4, &ip16);
		read16(idt_base + n * 4 + 2, &cs16);
		set_selector(&cs, cs16, 1);
		r.eip = ip16;
		return;
	}

	if (n <= 0x2F)
		D("\nInterrupt 0x%.2x", n);

	if ((r.eflags & F_VM) && (intflags & INT_FLAGS_SOFT))
	{
		if (IOPL < 3)
		{
			ex(EX_GP, 0);
			return;
		}
	}

	// PROTECTED-MODE:
	// Interrupt vector must be within IDT table limits, else #GP(vector number * 8+2+EXT);
	// Descriptor AR byte must indicate interrupt gate, trap gate, or task gate, else #GP(vector number * 8+2+EXT);
	if (!get_gate(&g, n))
	{
		ex(EX_GP, n * 8 + 2 + ((intflags & INT_FLAGS_SOFT) ? 0 : 1));
		return;
	}

	// IF software interrupt (* i.e. caused by INT n, INT 3, or INTO *) THEN
	//	IF gate descriptor DPL < CPL
	// 	THEN #GP(vector number * 8+2+EXT);
	//	FI;
	// FI;
	
	if ((g.dpl < cs.dpl) && (intflags & INT_FLAGS_SOFT))
	{
		ex(EX_GP, n * 8 + 2 + ((intflags & INT_FLAGS_SOFT) ? 0 : 1));
		return;
	}
	

	// Gate must be present, else #NP(vector number * 8+2+EXT);
	if (!g.present)
	{
		ex(EX_SEGMENT_NOT_PRESENT, n * 8 + 2 + ((intflags & INT_FLAGS_SOFT) ? 0 : 1));
		return;
	}

	switch (g.type)
	{
		case DESCR_286_INT_GATE:
		case DESCR_286_TRAP_GATE:
			// TRAP-GATE-OR-INTERRUPT-GATE:

			// Selector must be non-null, else #GP (EXT);
			if ((g.selector & 0xFFFC) == 0)
			{
				ex(EX_GP, (intflags & INT_FLAGS_SOFT) ? 0 : 1);
				return;
			}

			// Selector must be within its descriptor table limits ELSE #GP(selector+EXT);
			if (!get_descr(&csd, g.selector))
				return;

			// Descriptor AR byte must indicate code segment ELSE #GP(selector + EXT);

			// Segment must be present, else #NP(selector+EXT);
			if (!csd.present)
			{
				ex(EX_SEGMENT_NOT_PRESENT, g.selector + ((intflags & INT_FLAGS_SOFT) ? 0 : 1));
				return;
			}

			// D("int 0x%.2X, type = %x\n", n, g.type);

			// IF code segment is non-conforming AND DPL < CPL THEN GOTO INTERRUPT-TO-INNER-PRIVILEGE;
			if (((csd.type == DESCR_CODE) || (csd.type == DESCR_CODE_A) || (csd.type == DESCR_CODE_R) || (csd.type == DESCR_CODE_R_A)) && (csd.dpl < cs.dpl))
			{
				// INTERRUPT-TO-INNER-PRIVILEGE:
				// Check selector and descriptor for new stack in current TSS;
				get_ss_esp(csd.dpl, &nss, &nesp);

				// Selector must be non-null, else #GP(EXT);
				if ((nss & 0xFFFC) == 0)
				{
					ex(EX_GP, (intflags & INT_FLAGS_SOFT) ? 0 : 1);
					return;
				}

				// Selector index must be within its descriptor table limits ELSE #TS(SS selector+EXT);

				// Selector's RPL must equal DPL of code segment, else #TS(SS selector+EXT);

				// Stack segment DPL must equal DPL of code segment, else #TS(SS selector+EXT);

				// Descriptor must indicate writable data segment, else #TS(SS selector+EXT);

				if (!get_descr(&ssd, nss))
					return;
				
				// Segment must be present, else #SS(SS selector+EXT);
				if (!ssd.present)
				{
					ex(EX_SEGMENT_NOT_PRESENT, nss + ((intflags & INT_FLAGS_SOFT) ? 0 : 1));
					return;
				}

				// New stack must have room for 20 bytes else #SS(0)
				
				
				if (!write16(get_base(&ssd) + nesp - 20, 0))
				{
					ex(EX_STACK, nss + ((intflags & INT_FLAGS_SOFT) ? 0 : 1));
					return;
				}
				

				// Instruction pointer must be within CS segment boundaries else #GP(0);

				// Load new SS and eSP value from TSS;
				// CS:EIP <- selector:offset from gate;
				// Load CS descriptor into invisible portion of CS register;
				// Load SS descriptor into invisible portion of SS register;
				if (!set_selector(&ss, nss, 1))
					return;
				r.esp = nesp;
				if (!set_selector(&cs, g.selector, 1))
					return;
				r.eip = g.offset0_15 + g.offset_16_31 * 65536u;

				// Push (long pointer to old stack) (* 3 words padded to 4 *);
				push16(oss);
				push16(oesp);

				// Push (EFLAGS);
				push16(r.eflags);

				// Push (long pointer to return location) (* 3 words padded to 4*);
				push16(ocs);
				push16(oeip);

				
				if ((errorcode >= 0) && (n >= 8) && (n <= 16))
					push16(errorcode);
				
				r.eflags &= ~F_VM;

				// Set CPL to new code segment DPL;

				// Set RPL of CS to CPL;
				cs.value &= 0xFFFC;
				cs.value |= cs.dpl;

				// IF interrupt gate THEN IF <- 0 (* interrupt flag to 0 (disabled) *); FI;
				if ((g.type == DESCR_286_INT_GATE) || (g.type == DESCR_386_INT_GATE))
					r.eflags &= ~(F_I);

				// TF <- 0;
				// NT <- 0;
				r.eflags &= ~(F_T | F_NT);
				return;
			}

			// IF code segment is conforming OR code segment DPL = CPL THEN GOTO INTERRUPT-TO-SAME-PRIVILEGE-LEVEL;
			if ((csd.type == DESCR_CODE_C) || (csd.type == DESCR_CODE_C_A) || (csd.type == DESCR_CODE_C_R) || (csd.type == DESCR_CODE_C_R_A) || (csd.dpl == cs.dpl))
			{
				// INTERRUPT-TO-SAME-PRIVILEGE-LEVEL:
				// Current stack limits must allow pushing 12 bytes, else #SS(0);

				
				if (!write16(&ss, r.esp - 10, 0))
					return;
				

				// IF interrupt was caused by exception with error code
				// THEN Stack limits must allow push of two more bytes;
				// ELSE #SS(0);
				// FI;

				// Instruction pointer must be in CS limit, else #GP(0);

				// Push (EFLAGS);
				push16(r.eflags);

				// Push (long pointer to return location) (* 3 words padded to 4*);
				push16(ocs);
				push16(oeip);

				// CS:EIP <- selector:offset from gate;
				if (!set_selector(&cs, g.selector, 1))
					return;
				r.eip = g.offset0_15 + g.offset_16_31 * 65536u;

				// Load CS descriptor into invisible portion of CS register;
				// Set the RPL field of CS to CPL;
				// cs.value &= 0xFFFC;
				// cs.value |= cs.dpl;

				// Push (error code); (* if any *)
				if ((errorcode >= 0) && (n >= 8) && (n <= 16))
					push16(errorcode);

				// IF interrupt gate THEN IF <- 0 (* interrupt flag to 0 (disabled) *); FI;
				if ((g.type == DESCR_286_INT_GATE) || (g.type == DESCR_386_INT_GATE))
					r.eflags &= ~(F_I);

				// TF <- 0;
				// NT <- 0;
				r.eflags &= ~(F_T | F_NT);
				return;
			}
			return;
		case DESCR_386_INT_GATE:
		case DESCR_386_TRAP_GATE:
			// TRAP-GATE-OR-INTERRUPT-GATE:

			// Selector must be non-null, else #GP (EXT);
			if ((g.selector & 0xFFFC) == 0)
			{
				ex(EX_GP, (intflags & INT_FLAGS_SOFT) ? 0 : 1);
				return;
			}

			// Selector must be within its descriptor table limits ELSE #GP(selector+EXT);
			if (!get_descr(&csd, g.selector))
				return;

			// Descriptor AR byte must indicate code segment ELSE #GP(selector + EXT);

			// Segment must be present, else #NP(selector+EXT);
			if (!csd.present)
			{
				ex(EX_SEGMENT_NOT_PRESENT, g.selector + ((intflags & INT_FLAGS_SOFT) ? 0 : 1));
				return;
			}

			// D("int 0x%.2X, type = %x\n", n, g.type);

			// IF code segment is non-conforming AND DPL < CPL THEN GOTO INTERRUPT-TO-INNER-PRIVILEGE;
			if (((csd.type == DESCR_CODE) || (csd.type == DESCR_CODE_A) || (csd.type == DESCR_CODE_R) || (csd.type == DESCR_CODE_R_A)) && (csd.dpl < cs.dpl))
			{
				// INTERRUPT-TO-INNER-PRIVILEGE:
				// Check selector and descriptor for new stack in current TSS;
				get_ss_esp(csd.dpl, &nss, &nesp);

				// Selector must be non-null, else #GP(EXT);
				if ((nss & 0xFFFC) == 0)
				{
					ex(EX_INVALID_TSS, nss);
					return;
				}

				// Selector index must be within its descriptor table limits ELSE #TS(SS selector+EXT);

				// Selector's RPL must equal DPL of code segment, else #TS(SS selector+EXT);

				// Stack segment DPL must equal DPL of code segment, else #TS(SS selector+EXT);

				// Descriptor must indicate writable data segment, else #TS(SS selector+EXT);

				if (!get_descr(&ssd, nss))
					return;
				
				// Segment must be present, else #SS(SS selector+EXT);
				if (!ssd.present)
				{
					ex(EX_SEGMENT_NOT_PRESENT, nss + ((intflags & INT_FLAGS_SOFT) ? 0 : 1));
					return;
				}

				// New stack must have room for 20 bytes else #SS(0)
				
				
				if (!write32(get_base(&ssd) + nesp - 24, 0))
				{
					ex(EX_STACK, nss + ((intflags & INT_FLAGS_SOFT) ? 0 : 1));
					return;
				}
				

				// Instruction pointer must be within CS segment boundaries else #GP(0);

				r.eflags &= ~F_VM;
				
				// Load new SS and eSP value from TSS;
				// CS:EIP <- selector:offset from gate;
				// Load CS descriptor into invisible portion of CS register;
				// Load SS descriptor into invisible portion of SS register;
				if (!set_selector(&cs, g.selector, 1))
					return;
				r.eip = g.offset0_15 + g.offset_16_31 * 65536u;
				if (!set_selector(&ss, nss, 1))
					return;
				r.esp = nesp;

				cpl = cs.dpl;

				if (fl & F_VM)
				{
					push32(gs.value);
					push32(fs.value);
					push32(ds.value);
					push32(es.value);
				}

				// Push (long pointer to old stack) (* 3 words padded to 4 *);
				push32(oss);
				push32(oesp);

				// Push (EFLAGS);
				push32(fl);

				// Push (long pointer to return location) (* 3 words padded to 4*);
				push32(ocs);
				push32(oeip);

				
				if ((errorcode >= 0) && (n >= 8) && (n <= 16))
					push32(errorcode);
				
				// Set CPL to new code segment DPL;
				

				// Set RPL of CS to CPL;
				cs.value &= 0xFFFC;
				cs.value |= cpl;

				// IF interrupt gate THEN IF <- 0 (* interrupt flag to 0 (disabled) *); FI;
				if ((g.type == DESCR_286_INT_GATE) || (g.type == DESCR_386_INT_GATE))
					r.eflags &= ~(F_I);

				// TF <- 0;
				// NT <- 0;
				r.eflags &= ~(F_T | F_NT | F_VM);
				return;
			}

			// IF code segment is conforming OR code segment DPL = CPL THEN GOTO INTERRUPT-TO-SAME-PRIVILEGE-LEVEL;
			if ((csd.type == DESCR_CODE_C) || (csd.type == DESCR_CODE_C_A) || (csd.type == DESCR_CODE_C_R) || (csd.type == DESCR_CODE_C_R_A) || (csd.dpl == cs.dpl))
			{
				// INTERRUPT-TO-SAME-PRIVILEGE-LEVEL:
				// Current stack limits must allow pushing 12 bytes, else #SS(0);

				
				if (!write32(&ss, r.esp - 16, 0))
					return;
				

				// IF interrupt was caused by exception with error code
				// THEN Stack limits must allow push of two more bytes;
				// ELSE #SS(0);
				// FI;

				// Instruction pointer must be in CS limit, else #GP(0);

				// Push (EFLAGS);
				push32(r.eflags);

				// Push (long pointer to return location) (* 3 words padded to 4*);
				push32(ocs);
				push32(oeip);

				// CS:EIP <- selector:offset from gate;
				if (!set_selector(&cs, g.selector, 1))
					return;
				r.eip = g.offset0_15 + g.offset_16_31 * 65536u;

				// Load CS descriptor into invisible portion of CS register;
				// Set the RPL field of CS to CPL;
				cs.value &= 0xFFFC;
				cs.value |= cpl;

				// Push (error code); (* if any *)
				if ((errorcode >= 0) && (n >= 8) && (n <= 16))
					push32(errorcode);

				// IF interrupt gate THEN IF <- 0 (* interrupt flag to 0 (disabled) *); FI;
				if ((g.type == DESCR_286_INT_GATE) || (g.type == DESCR_386_INT_GATE))
					r.eflags &= ~(F_I);

				// TF <- 0;
				// NT <- 0;
				r.eflags &= ~(F_T | F_NT | F_VM);
				return;
			}
		default:
			// TASK-GATE
			break;
	}
}

void ex(int n, int errorcode)
{
	if (fault != 0)
		return;

	D("EX %d\n", n);

	if ((n != 8) && (n != 10) && (n != 11) && (n != 12) && (n != 13) && (n != 14))
		errorcode = -1;

	fault = 0x100 + n;
	faultcode = errorcode;

	if (n == EX_PAGE)
		num_pf++;
	else if (n == EX_GP)
		num_gp++;
	else
		num_ex++;

	
	cs = instr_cs;
	ss = instr_ss;
	r.esp = instr_esp;
	set_flags(instr_fl, 0xFFFFFFFFu);
	

	if ((n != 3) && (n != 4) && (n != 9))
		r.eip = instr_eip;
}

void ex(int n)
{
	ex(n, -1);
}

int iret()
{
	unsigned int ncs, neip, neflags, nss, nesp, nes, nds, nfs, ngs;
	unsigned short ncs16, nip, nflags, nss16, nsp;
	unsigned int mask = 0xFFFFFFFFu;
	descr_t csd, ssd;

	if (!pmode)
	{
		if (i32)
		{
			if (!pop32(&neip))
				return 0;
			if (!pop32(&ncs))
				return 0;
			if (!pop32(&neflags))
				return 0;
			if (!set_selector(&cs, ncs, 1))
				return 0;
			r.eip = neip;
#if (CPU < 486)
			neflags &= ~F_ID;
#endif			
			set_flags(neflags, 0xFFFFFFFFu);

			return 1;
		}
		if (!pop16(&nip))
			return 0;
		if (!pop16(&ncs16))
			return 0;
		if (!pop16(&nflags))
			return 0;
		if (!set_selector(&cs, ncs16, 1))
			return 0;

		r.eip = nip;
		set_flags16(nflags, 0xFFFFFFFFu);
		return 1;
	}


	// IF (VM = 1) & (IOPL < 3) THEN #GP(0);
	if (r.eflags & F_VM)
	{
		GP(IOPL < 3, 0);

		if (i32)
		{
			if (!read32(ss.base + (r.esp & ss_mask), &neip))
				return 0;
			if (!read32(ss.base + ((r.esp + 4) & ss_mask), &ncs))
				return 0;
			if (!read32(ss.base + ((r.esp + 8) & ss_mask), &neflags))
				return 0;

			if (!set_selector(&cs, ncs, 1))
				return 0;
			r.esp += 12;
			r.eip = neip;
			set_flags(neflags, F_ALL | F_NT);
		}
		else
		{
			if (!read16(ss.base + (r.esp & ss_mask), &nip))
				return 0;
			if (!read16(ss.base + ((r.esp + 2) & ss_mask), &ncs16))
				return 0;
			if (!read16(ss.base + ((r.esp + 4) & ss_mask), &nflags))
				return 0;

			if (!set_selector(&cs, ncs16, 1))
				return 0;
			r.esp += 6;
			r.eip = nip;
			set_flags16(nflags, F_ALL | F_NT);
		}
		return 1;
	}

	// IF NT = 1 THEN GOTO TASK-RETURN;
	if (r.eflags & F_NT)
	{
		// Examine Back Link Selector in TSS addressed by the current task register:
		// Must specify global in the local/global bit, else #TS(new TSS selector);
		switch_task(0, SWITCH_IRET);
		return 1;
	}

	if (i32)
	{
		if (!read32(&ss, r.esp, &neip))
			return 0;
		if (!read32(&ss, r.esp + 4, &ncs))
			return 0;
		if (!read32(&ss, r.esp + 8, &neflags))
			return 0;

		if (r.eflags & F_VM)
		{
			// In V86 iret can change only {RF, 15:0}
			neflags = (r.eflags & 0xFFFF0000u) | (neflags & 0xFFFFu);
		}

		// IF VM = 1 in flags image on stack THEN GO TO STACK-RETURN-TO-V86;
		if (neflags & F_VM)
		{
			if (!read32(&ss, r.esp + 12, &nesp))
				return 0;
			if (!read32(&ss, r.esp + 16, &nss))
				return 0;

			if (!read32(&ss, r.esp + 20, &nes))
				return 0;
			if (!read32(&ss, r.esp + 24, &nds))
				return 0;
			if (!read32(&ss, r.esp + 28, &nfs))
				return 0;
			if (!read32(&ss, r.esp + 32, &ngs))
				return 0;

			set_flags(neflags, 0xFFFFFFFFu);

			cpl = 3;

			set_selector(&ss, nss, 1);
			r.esp = nesp;
			
			set_selector(&es, nes, 1);
			set_selector(&ds, nds, 1);
			set_selector(&fs, nfs, 1);
			set_selector(&gs, ngs, 1);

			set_selector(&cs, ncs, 1);
			r.eip = neip & 0xFFFFu;

			return 1;
		}
	}
	else
	{
		if (!read16(&ss, (r.esp) & ss_mask, &nip))
			return 0;
		if (!read16(&ss, (r.esp + 2) & ss_mask, &ncs16))
			return 0;
		if (!read16(&ss, (r.esp + 4) & ss_mask, &nflags))
			return 0;

		neip = nip;
		ncs = ncs16;
		neflags = (r.eflags & 0xFFFF0000u) | nflags;
	}

	GP(ncs == 0, 0);

	if (!get_descr(&csd, ncs))
		return 0;

	GP((ncs & 3) < cpl, ncs & 0xFFFC);

	NP(!csd.present, ncs & 0xFFFC);

	if ((ncs & 3) == cpl)
	{
		// RETURN-SAME-LEVEL:
		if (!set_selector(&cs, ncs, 1))
			return 0;
		r.eip = neip;
		// Load EFLAGS with third doubleword from stack;
		mask = cpl > 0 ? F_ALL | F_NT : F_ALL | F_NT | F_IOPL;
		if (IOPL < cpl)
			mask &= ~F_I;
		set_flags(neflags, mask);
		// Increment eSP by 12;
		r.esp += i32 ? 12 : 6;
		return 1;
	}

	// RETURN-OUTER-LEVEL:
	if (i32)
	{
		if (!read32(&ss, (r.esp + 12) & ss_mask, &nesp))
			return 0;
		if (!read32(&ss, (r.esp + 16) & ss_mask, &nss))
			return 0;
	}
	else
	{
		if (!read16(&ss, (r.esp + 6) & ss_mask, &nsp))
			return 0;
		if (!read16(&ss, (r.esp + 8) & ss_mask, &nss16))
			return 0;
		nesp = nsp;
		nss = nss16;
	}

	if (!get_descr(&ssd, nss))
		return 0;

	NP(!ssd.present, nss);

	// Instruction pointer must be within code segment limit ELSE #GP(0);
	GP(neip > get_limit(&csd), 0);

	// Load CS:EIP from stack;
	// Load EFLAGS with values at (eSP+8);
	if (!set_selector(&cs, ncs, 1))
		return 0;
	r.eip = neip;

	mask = cpl > 0 ? F_ALL | F_NT : F_ALL | F_NT | F_IOPL;
	mask |= F_IOPL;
	if (IOPL < cpl)
		mask &= ~F_I;
	set_flags(neflags, mask);

	// Load SS:eSP from stack;
	if (!set_selector(&ss, nss, 1))
		return 0;
	r.esp = nesp;

	cpl = ncs & 3;

	return 1;
}

int cli()
{
	
	if (pmode)
	{
		if (r.eflags & F_VM)
		{
			GP(IOPL < 3, 0);
		}
		else
		{
			GP(IOPL < cpl, 0);
		}
	}
	

	r.eflags &= ~F_I;
	return 1;
}

int sti()
{
	
	if (pmode)
	{
		if (r.eflags & F_VM)
		{
			GP(IOPL < 3, 0);
		}
		else
		{
			GP(IOPL < cpl, 0);
		}
	}
	

	r.eflags |= F_I;
	return 1;
}
