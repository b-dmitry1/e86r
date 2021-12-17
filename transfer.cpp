#include "stdafx.h"
#include "cpu.h"
#include "transfer.h"
#include "memdescr.h"
#include "interrupts.h"

#ifndef offsetof
#define offsetof(s,m)   (size_t)&reinterpret_cast<const volatile char&>((((s *)0)->m))
#endif

int switch_task(unsigned int newtss, int type)
{
	descr_t tsd, ldtd;
	int i;

	tss386_t nw;
	unsigned int *nwp = (unsigned int *)&nw;
	unsigned int nwbase;

	tss386_t old;
	unsigned int *oldp = (unsigned int *)&old;
	unsigned int oldbase = tssbase;

	unsigned short oldtss = tss;

	// Return
	if (newtss == 0)
	{
		if (!read32(tssbase + offsetof(tss386_t, back), &newtss))
			return 0;
	}

	// Get new TSS descriptor
	if (!get_descr(&tsd, newtss))
		return 0;
	nwbase = get_base(&tsd);

	// Only 386+ TSS now
	if ((tsd.type != DESCR_386_TSS_AVAIL) && (tsd.type != DESCR_386_TSS_BUSY))
		return 0;

	// Read new table
	for (i = 0; i < sizeof(nw) / 4; i++)
		if (!read32(nwbase + i * 4, &nwp[i]))
			return 0;

	// Reset NT
	if (type == SWITCH_IRET)
		r.eflags &= ~F_NT;

	// Save state
	if (!write32(oldbase + offsetof(tss386_t, es), es.value)) return 0;
	if (!write32(oldbase + offsetof(tss386_t, cs), cs.value)) return 0;
	if (!write32(oldbase + offsetof(tss386_t, ss), ss.value)) return 0;
	if (!write32(oldbase + offsetof(tss386_t, ds), ds.value)) return 0;
	if (!write32(oldbase + offsetof(tss386_t, fs), fs.value)) return 0;
	if (!write32(oldbase + offsetof(tss386_t, gs), gs.value)) return 0;
	if (!write32(oldbase + offsetof(tss386_t, eax), r.eax)) return 0;
	if (!write32(oldbase + offsetof(tss386_t, ecx), r.ecx)) return 0;
	if (!write32(oldbase + offsetof(tss386_t, edx), r.edx)) return 0;
	if (!write32(oldbase + offsetof(tss386_t, ebx), r.ebx)) return 0;
	if (!write32(oldbase + offsetof(tss386_t, esp), r.esp)) return 0;
	if (!write32(oldbase + offsetof(tss386_t, ebp), r.ebp)) return 0;
	if (!write32(oldbase + offsetof(tss386_t, esi), r.esi)) return 0;
	if (!write32(oldbase + offsetof(tss386_t, edi), r.edi)) return 0;
	if (!write32(oldbase + offsetof(tss386_t, eflags), r.eflags)) return 0;
	if (!write32(oldbase + offsetof(tss386_t, eip), r.eip)) return 0;
	if (!write32(oldbase + offsetof(tss386_t, ldt), ldtr)) return 0;
	if (!write32(oldbase + offsetof(tss386_t, cr3), cr[3])) return 0;

	// On interrupt or CALL FAR TSS:0 save backlink and set NT
	if (type == SWITCH_INT_CALL)
	{
		if (!write32(nwbase + offsetof(tss386_t, back), tss))
			return 0;
		nw.eflags |= F_NT;
	}

	if (r.eflags & F_VM)
	{
		if (!set_selector(&cs, nw.cs, 1))
			return 0;
		cs.big = 0;
		cpl = 3;
	}
	else
	{
		if (nw.ldt != 0)
		{
			if (!get_descr(&ldtd, nw.ldt))
				return 0;
			ldtr = nw.ldt;
			ldt_limit = get_limit(&ldtd);
			ldt_base = get_base(&ldtd);
		}
		cpl = nw.cs & 3;

		if (!set_selector(&cs, nw.cs, 1))
			return 0;
	}

	if (newtss == tss)
	{
		nw.es = es.value;
		nw.cs = cs.value;
		nw.ss = ss.value;
		nw.ds = ds.value;
		nw.fs = fs.value;
		nw.gs = gs.value;
	}
	else
	{
		r.eax = nw.eax;
		r.ecx = nw.ecx;
		r.edx = nw.edx;
		r.ebx = nw.ebx;
		r.esp = nw.esp;
		r.ebp = nw.ebp;
		r.esi = nw.esi;
		r.edi = nw.edi;
		set_flags(nw.eflags, (F_ALL | F_IOPL | F_VM));
		r.eip = nw.eip;

		cr[3] = nw.cr3;
		dir = (unsigned int *)&ram[cr[3] & 0xFFFFF000u];
	}

	set_tss(newtss);

	if (!set_selector(&es, nw.es, 1))
		return 0;
	if (!set_selector(&ss, nw.ss, 1))
		return 0;
	if (!set_selector(&ds, nw.ds, 1))
		return 0;
	if (!set_selector(&fs, nw.fs, 1))
		return 0;
	if (!set_selector(&gs, nw.gs, 1))
		return 0;

	return 1;
}

int far_jmp(unsigned int ncs, unsigned int neip)
{
	descr_t csd;

	if ((!pmode) || (r.eflags & F_VM))
	{
		if (!set_selector(&cs, ncs, 1))
			return 0;
		if (i32)
			r.eip = neip;
		else
			r.eip = neip & 0xFFFFu;
		return 1;
	}

	GP((ncs & 0xFFFC) == 0, 0);

	if (!get_descr(&csd, ncs & 0xFFFC))
		return 0;

	switch (csd.type)
	{
		case DESCR_CODE_C:
		case DESCR_CODE_C_A:
		case DESCR_CODE_C_R:
		case DESCR_CODE_C_R_A:
			// CONFORMING-CODE-SEGMENT:
			// Descriptor DPL must be <= CPL ELSE #GP(selector);
			GP(csd.dpl > cpl, ncs & 0xFFFC);
			// Segment must be present ELSE #NP(selector);
			NP(!csd.present, ncs & 0xFFFC);
			// Instruction pointer must be within code-segment limit ELSE #GP(0);
			GP(neip > get_limit(&csd), 0);
			// Load CS:EIP from destination pointer;
			// Load CS register with new segment descriptor;
			ncs = (ncs & 0xFFFC) | cpl;
			if (!set_selector(&cs, ncs, 1))
				return 0;
			r.eip = neip;
			if (!i32)
				r.iph = 0;
			return 1;
		case DESCR_CODE:
		case DESCR_CODE_A:
		case DESCR_CODE_R:
		case DESCR_CODE_R_A:
			// NONCONFORMING-CODE-SEGMENT:
			// RPL of destination selector must be <= CPL ELSE #GP(selector);
			GP((ncs & 3) > cpl, ncs & 0xFFFC);
			// Descriptor DPL must be <= CPL ELSE #GP(selector);
			GP(csd.dpl != cpl, ncs & 0xFFFC);
			// Segment must be present ELSE #NP(selector);
			NP(!csd.present, ncs & 0xFFFC);
			// Instruction pointer must be within code-segment limit ELSE #GP(0);
			// GP(neip > get_limit(&csd), 0);
			// Load CS:EIP from destination pointer;
			// Load CS register with new segment descriptor;
			// Set RPL field of CS register to CPL;
			ncs = (ncs & 0xFFFC) | cpl;
			if (!set_selector(&cs, ncs, 1))
				return 0;
			r.eip = neip;
			if (!i32)
				r.iph = 0;
			return 1;
		case DESCR_386_TSS_AVAIL:
		case DESCR_386_TSS_BUSY:
			// TASK-STATE-SEGMENT:
			// TSS DPL must be >= CPL ELSE #GP(TSS selector);

			// TSS DPL must be >= TSS selector RPL ELSE #GP(TSS selector);

			// Descriptor AR byte must specify available TSS (bottom bits 00001) ELSE #GP(TSS selector);

			// Task State Segment must be present ELSE #NP(TSS selector);

			// SWITCH-TASKS (without nesting) to TSS;
			switch_task(ncs, SWITCH_JMP);

			// Instruction pointer must be within code-segment limit ELSE #GP(0);
			return 1;
	}

	return 0;
}

int far_call(unsigned int ncs, unsigned int neip)
{
	unsigned int nss, nesp, oss, oesp;
	descr_t csd;
	gate_t g;

	if ((!pmode) || (r.eflags & F_VM))
	{
		if (i32)
		{
			push32(cs.value);
			push32(r.eip);
		}
		else
		{
			push16(cs.value);
			push16(r.ip);
		}

		if (!set_selector(&cs, ncs, 1))
			return 0;
		if (i32)
			r.eip = neip;
		else
			r.eip = neip & 0xFFFFu;
		return 1;
	}

	oss = ss.value;
	oesp = r.esp;

	GP(ncs == 0, 0);

	if (!get_descr(&csd, ncs), 0)
		return 0;

	switch (csd.type)
	{
		case DESCR_CODE_C:
		case DESCR_CODE_C_A:
		case DESCR_CODE_C_R:
		case DESCR_CODE_C_R_A:
			// CONFORMING-CODE-SEGMENT:
			// Descriptor DPL must be <= CPL ELSE #GP(selector);
			GP(csd.dpl > cpl, ncs & 0xFFFC);
			// Segment must be present ELSE #NP(selector);
			NP(!csd.present, ncs & 0xFFFC);
			// Instruction pointer must be within code-segment limit ELSE #GP(0);
			GP(neip > get_limit(&csd), 0);
			// Load CS:EIP from destination pointer;
			// Load CS register with new segment descriptor;

			if (i32)
			{
				if (!push32(cs.value))
					return 0;
				if (!push32(r.eip))
					return 0;
			}
			else
			{
				if (!push16(cs.value))
					return 0;
				if (!push16(r.ip))
					return 0;
			}

			ncs = (ncs & 0xFFFC) | cpl;
			if (!set_selector(&cs, ncs, 1))
				return 0;
			r.eip = neip;
			if (!i32)
				r.iph = 0;
			return 1;
		case DESCR_CODE:
		case DESCR_CODE_A:
		case DESCR_CODE_R:
		case DESCR_CODE_R_A:
			// NONCONFORMING-CODE-SEGMENT:
			// RPL of destination selector must be <= CPL ELSE #GP(selector);
			GP((ncs & 3) > cpl, ncs);
			// Descriptor DPL must be <= CPL ELSE #GP(selector);
			//GP(csd.dpl != cpl, ncs);
			// Segment must be present ELSE #NP(selector);
			NP(!csd.present, ncs);
			// Instruction pointer must be within code-segment limit ELSE #GP(0);
			GP(neip > get_limit(&csd), 0);
			// Load CS:EIP from destination pointer;
			// Load CS register with new segment descriptor;
			// Set RPL field of CS register to CPL;

			if (i32)
			{
				if (!push32(cs.value))
					return 0;
				if (!push32(r.eip))
					return 0;
			}
			else
			{
				if (!push16(cs.value))
					return 0;
				if (!push16(r.ip))
					return 0;
			}

			ncs = (ncs & 0xFFFC) | cpl;
			if (!set_selector(&cs, ncs, 1))
				return 0;
			r.eip = neip;
			if (!i32)
				r.iph = 0;
			return 1;
		case DESCR_386_TSS_AVAIL:
		case DESCR_386_TSS_BUSY:
			// TASK-STATE-SEGMENT:
			// TSS DPL must be >= CPL ELSE #GP(TSS selector);

			// TSS DPL must be >= TSS selector RPL ELSE #GP(TSS selector);

			// Descriptor AR byte must specify available TSS (bottom bits 00001) ELSE #GP(TSS selector);

			// Task State Segment must be present ELSE #NP(TSS selector);

			// SWITCH-TASKS (without nesting) to TSS;
			switch_task(ncs, SWITCH_INT_CALL);

			// Instruction pointer must be within code-segment limit ELSE #GP(0);
			return 1;
		case DESCR_286_CALL_GATE:
		case DESCR_386_CALL_GATE:
			memcpy(&g, &csd, sizeof(g));
			if (g.paramcount > 0)
			{
				opcode = 1;
				return 0;
			}

			D("\n call gate\n");

			get_ss_esp(g.selector & 3, &nss, &nesp);

			if (!set_selector(&ss, nss, 1))
				return 0;
			r.esp = nesp;

			if (i32)
			{
				push32(oss);
				push32(oesp);
				push32(cs.value);
				push32(r.eip);
			}
			else
			{
				push16(oss);
				push16(oesp);
				push16(cs.value);
				push16(r.ip);
			}
			
			if (!set_selector(&cs, g.selector, 1))
				return 0;
			r.eip = g.offset0_15 + (i32 ? g.offset_16_31 * 65536u : 0);

			return 1;
	}
	return 0;
}

int far_ret(unsigned short n)
{
	unsigned int ncs, neip, nesp, nss;
	unsigned short ncs16, nip, nss16, nsp;
	descr_t csd, ssd;

	if ((!pmode) || (r.eflags & F_VM))
	{
		if (i32)
		{
			pop32(&neip);
			pop32(&ncs);
			if (!set_selector(&cs, ncs, 1))
				return 0;
			r.eip = neip;
			r.esp += n;
		}
		else
		{
			pop16(&nip);
			pop16(&ncs16);
			if (!set_selector(&cs, ncs16, 1))
				return 0;
			r.eip = nip;
			r.sp += n;
		}
		return 1;
	}

	if (i32)
	{
		if (!read32(&ss, r.esp, &neip))
			return 0;
		if (!read32(&ss, r.esp + 4, &ncs))
			return 0;

		// IF OperandSize=32
		//	THEN Third word on stack must be within stack limits, else #SS(0);
		//	ELSE Second word on stack must be within stack limits, else #SS(0);
		// FI;

		// Return CS selector RPL must be >= CPL, else #GP(Return selector);

		// IF return selector RPL = CPL THEN GOTO RETURN-SAME-LEVEL;
		// ELSE GOTO RETURN-OUTER-LEVEL;
		if ((ncs & 3) == cpl)
		{
			// RETURN-SAME-LEVEL:
			// Return CS selector (at eSP+4) must be non-null, else #GP(0);
			GP(ncs == 0, 0);

			// Selector index must be within its descriptor table limits, else #GP(Return selector);
			if (!get_descr(&csd, ncs))
				return 0;
			
			// AR byte must indicate code segment, else #GP(Return selector);

			// IF non-conforming THEN code segment DPL must = CPL; ELSE #GP(Return selector); FI;

			// IF conforming THEN
			//	code segment DPL must be <= CPL, else #GP(Return selector);
			// FI;

			//	Segment must be present, else #NP(Return selector);
			NP(!csd.present, ncs);

			//	Instruction pointer must be within code segment boundaries, else #GP(0);

			// Load CS:EIP from stack;
			// Load CS-register with new code segment descriptor;
			if (!set_selector(&cs, ncs, 1))
				return 0;
			r.eip = neip;
			// Increment eSP by 8;
			r.esp += 8 + n;
			return 1;
		}
		// RETURN-OUTER-LEVEL:
		// Top 20 bytes on stack must be within limits, else #SS(0);

		if (!read32(&ss, r.esp + 8 + n, &nesp))
			return 0;
		if (!read32(&ss, r.esp + 12 + n, &nss))
			return 0;

		// Examine return CS selector and associated descriptor:
		// Selector must be non-null, else #GP(0);
		GP(ncs == 0, 0);

		// Selector index must be within its descriptor table limits; ELSE #GP(Return selector);
		if (!get_descr(&csd, ncs))
			return 0;

		// AR byte must indicate code segment, else #GP(Return selector);

		// IF non-conforming THEN code segment DPL must = CS selector RPL; ELSE #GP(Return selector); FI;

		// IF conforming THEN code segment DPL must be > CPL; ELSE #GP(Return selector); FI;

		// Segment must be present, else #NP(Return selector);
		NP(!csd.present, ncs);

		// Examine return SS selector and associated descriptor:
		// Selector must be non-null, else #GP(0);
		GP(ncs == 0, 0);

		// Selector index must be within its descriptor table limits; ELSE #GP(SS selector);
		if (!get_descr(&ssd, nss))
			return 0;

		// Selector RPL must equal the RPL of the return CS selector ELSE #GP(SS selector);


		// AR byte must indicate a writable data segment, else #GP(SS selector);

		// Stack segment DPL must equal the RPL of the return CS selector ELSE #GP(SS selector);

		// SS must be present, else #NP(SS selector);
		NP(!ssd.present, nss);

		// Instruction pointer must be within code segment limit ELSE #GP(0);
		GP(neip > get_limit(&csd), 0);

		// Load CS:EIP from stack;
		if (!set_selector(&cs, ncs, 1))
			return 0;
		r.eip = neip;

		// Load SS:eSP from stack;
		if (!set_selector(&ss, nss, 1))
			return 0;
		r.esp = nesp + n;

		// Set CPL to the RPL of the return CS selector;

		// Load the CS register with the CS descriptor;
		// Load the SS register with the SS descriptor;

		/*
		FOR each of ES, FS, GS, and DS
		DO;
		IF the current value of the register is not valid for the outer level;
		THEN zero the register and clear the valid flag;
		FI;
		To be valid, the register setting must satisfy the following
		properties:
		Selector index must be within descriptor table limits;
		AR byte must indicate data or readable code segment;
		IF segment is data or non-conforming code,
		THEN DPL must be >= CPL, or DPL must be >= RPL;
		OD;
		*/
		return 1;
	}






	if (!read16(&ss, (r.esp) & ss_mask, &nip))
		return 0;
	if (!read16(&ss, (r.esp + 2) & ss_mask, &ncs16))
		return 0;

	neip = nip;
	ncs = ncs16;

	// IF OperandSize=32
	//	THEN Third word on stack must be within stack limits, else #SS(0);
	//	ELSE Second word on stack must be within stack limits, else #SS(0);
	// FI;

	// Return CS selector RPL must be >= CPL, else #GP(Return selector);
	GP((ncs & 3) < cpl, ncs & 0xFFFC);

	// IF return selector RPL = CPL THEN GOTO RETURN-SAME-LEVEL;
	// ELSE GOTO RETURN-OUTER-LEVEL;
	if ((ncs & 3) == cpl)
	{
		// RETURN-SAME-LEVEL:
		// Return CS selector (at eSP+4) must be non-null, else #GP(0);
		GP(ncs == 0, 0);

		// Selector index must be within its descriptor table limits, else #GP(Return selector);
		if (!get_descr(&csd, ncs))
			return 0;
			
		// AR byte must indicate code segment, else #GP(Return selector);

		// IF non-conforming THEN code segment DPL must = CPL; ELSE #GP(Return selector); FI;

		// IF conforming THEN
		//	code segment DPL must be <= CPL, else #GP(Return selector);
		// FI;

		//	Segment must be present, else #NP(Return selector);
		NP(!csd.present, ncs & 0xFFFC);

		//	Instruction pointer must be within code segment boundaries, else #GP(0);

		// Load CS:EIP from stack;
		// Load CS-register with new code segment descriptor;
		if (!set_selector(&cs, ncs, 1))
			return 0;
		r.eip = neip;
		// Increment eSP by 8;
		r.esp += 4 + n;
		return 1;
	}
	// RETURN-OUTER-LEVEL:
	// Top 20 bytes on stack must be within limits, else #SS(0);

	if (!read16(&ss, (r.esp + 4 + n) & ss_mask, &nsp))
		return 0;
	if (!read16(&ss, (r.esp + 6 + n) & ss_mask, &nss16))
		return 0;

	nesp = nsp;
	nss = nss16;

	// Examine return CS selector and associated descriptor:
	// Selector must be non-null, else #GP(0);
	GP(ncs == 0, 0);

	// Selector index must be within its descriptor table limits; ELSE #GP(Return selector);
	if (!get_descr(&csd, ncs))
		return 0;

	// AR byte must indicate code segment, else #GP(Return selector);

	// IF non-conforming THEN code segment DPL must = CS selector RPL; ELSE #GP(Return selector); FI;

	// IF conforming THEN code segment DPL must be > CPL; ELSE #GP(Return selector); FI;

	// Segment must be present, else #NP(Return selector);
	NP(!csd.present, ncs & 0xFFFC);

	// Examine return SS selector and associated descriptor:
	// Selector must be non-null, else #GP(0);
	GP(ncs == 0, 0);

	// Selector index must be within its descriptor table limits; ELSE #GP(SS selector);
	if (!get_descr(&ssd, nss))
		return 0;

	// Selector RPL must equal the RPL of the return CS selector ELSE #GP(SS selector);


	// AR byte must indicate a writable data segment, else #GP(SS selector);

	// Stack segment DPL must equal the RPL of the return CS selector ELSE #GP(SS selector);

	// SS must be present, else #NP(SS selector);
	NP(!ssd.present, nss);

	// Instruction pointer must be within code segment limit ELSE #GP(0);
	GP(neip > get_limit(&csd), 0);

	// Load CS:EIP from stack;
	if (!set_selector(&cs, ncs, 1))
		return 0;
	r.eip = neip;

	// Load SS:eSP from stack;
	if (!set_selector(&ss, nss, 1))
		return 0;
	r.esp = nesp + n;
	if (!ss.big)
		r.esp &= 0xFFFFu;

	// Set CPL to the RPL of the return CS selector;
	cpl = ncs & 3;


	// Load the CS register with the CS descriptor;
	// Load the SS register with the SS descriptor;

	/*
	FOR each of ES, FS, GS, and DS
	DO;
	IF the current value of the register is not valid for the outer level;
	THEN zero the register and clear the valid flag;
	FI;
	To be valid, the register setting must satisfy the following
	properties:
	Selector index must be within descriptor table limits;
	AR byte must indicate data or readable code segment;
	IF segment is data or non-conforming code,
	THEN DPL must be >= CPL, or DPL must be >= RPL;
	OD;
	*/
	return 1;

	if (!read16(&ss, (r.esp) & ss_mask, &nip))
		return 0;
	neip = nip;
	if (!read16(&ss, (r.esp + 2) & ss_mask, &ncs16))
		return 0;
	ncs = ncs16;

	if ((ncs & 3) == cpl)
	{
		if (!set_selector(&cs, ncs, 1))
			return 0;
		r.eip = neip;
		// Increment eSP by 4 + n;
		r.esp += 4 + n;
		return 1;
	}

	return 0;
}
