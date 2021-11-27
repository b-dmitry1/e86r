#include "stdafx.h"
#include "cpu.h"
#include "interrupts.h"
#include "memdescr.h"
#include "modrm.h"
#include "ioports.h"

void verr(unsigned short sel)
{
	r.eflags &= ~F_Z;

	if (sel < 4)
		return;

	descr_t d;
	if (!get_descr(&d, sel, 1))
		return;

	switch (d.type)
	{
		case DESCR_CODE_C_R:
		case DESCR_CODE_C_R_A:
			r.eflags |= F_Z;
			break;
		case DESCR_DATA_DN:
		case DESCR_DATA_DN_A:
		case DESCR_DATA_DN_W:
		case DESCR_DATA_DN_W_A:
		case DESCR_DATA_UP:
		case DESCR_DATA_UP_A:
		case DESCR_DATA_UP_W:
		case DESCR_DATA_UP_W_A:
		case DESCR_CODE_R:
		case DESCR_CODE_R_A:
			if (!((d.dpl < cpl) || (d.dpl < (sel & 3u))))
				r.eflags |= F_Z;
			break;
	}
}

void verw(unsigned short sel)
{
	r.eflags &= ~F_Z;

	if (sel < 4)
		return;

	descr_t d;
	if (!get_descr(&d, sel, 1))
		return;

	switch (d.type)
	{
		case DESCR_DATA_DN_W:
		case DESCR_DATA_DN_W_A:
		case DESCR_DATA_UP_W:
		case DESCR_DATA_UP_W_A:
			if (!((d.dpl < cpl) || (d.dpl < (sel & 3u))))
				r.eflags |= F_Z;
			break;
	}
}

unsigned int lar(unsigned short sel)
{
	r.eflags &= ~F_Z;

	if (sel < 4)
		return 0;

	descr_t d;
	unsigned int *p = (unsigned int *)&d;
	if (!get_descr(&d, sel, 1))
		return 0;

	switch (d.type)
	{
		case DESCR_CODE_C:
		case DESCR_CODE_C_A:
		case DESCR_CODE_C_R:
		case DESCR_CODE_C_R_A:
			break;
		case DESCR_DATA_DN:
		case DESCR_DATA_DN_A:
		case DESCR_DATA_DN_W:
		case DESCR_DATA_DN_W_A:
		case DESCR_DATA_UP:
		case DESCR_DATA_UP_A:
		case DESCR_DATA_UP_W:
		case DESCR_DATA_UP_W_A:
		case DESCR_CODE:
		case DESCR_CODE_A:
		case DESCR_CODE_R:
		case DESCR_CODE_R_A:
		case DESCR_LDT:
		case DESCR_TASK_GATE:
		case DESCR_286_TSS_AVAIL:
		case DESCR_286_TSS_BUSY:
		case DESCR_386_TSS_AVAIL:
		case DESCR_386_TSS_BUSY:
		case DESCR_286_CALL_GATE:
		case DESCR_386_CALL_GATE:
			if ((d.dpl < cpl) || (d.dpl < (sel & 3u)))
				return 0;
			break;
		default:
			return 0;
	}

	r.eflags |= F_Z;
	return p[1] & 0x00FFFF00u;
}

unsigned int lsl(unsigned short sel)
{
	r.eflags &= ~F_Z;

	if (sel < 4)
		return 0;

	descr_t d;
	if (!get_descr(&d, sel, 1))
		return 0;

	switch (d.type)
	{
		case DESCR_CODE_C:
		case DESCR_CODE_C_A:
		case DESCR_CODE_C_R:
		case DESCR_CODE_C_R_A:
			break;
		case DESCR_DATA_DN:
		case DESCR_DATA_DN_A:
		case DESCR_DATA_DN_W:
		case DESCR_DATA_DN_W_A:
		case DESCR_DATA_UP:
		case DESCR_DATA_UP_A:
		case DESCR_DATA_UP_W:
		case DESCR_DATA_UP_W_A:
		case DESCR_CODE:
		case DESCR_CODE_A:
		case DESCR_CODE_R:
		case DESCR_CODE_R_A:
		case DESCR_LDT:
		case DESCR_286_TSS_AVAIL:
		case DESCR_286_TSS_BUSY:
		case DESCR_386_TSS_AVAIL:
		case DESCR_386_TSS_BUSY:
			if ((d.dpl < cpl) || (d.dpl < (sel & 3u)))
				return 0;
			break;
		default:
			return 0;
	}

	r.eflags |= F_Z;
	return get_limit(&d);
}

void lmsw(unsigned int v)
{
	if ((cr[0] & 1) && (cpl > 0))
	{
		ex(EX_GP, 0);
		return;
	}
	v &= 0x0F;
	v |= cr[0] & 0xFFFFFFF1;
	cr[0] = v;
#if (FPU == 1)
	cr[0] |= CR0_MP | CR0_ET;
#else
	cr[0] &= ~(CR0_MP | CR0_ET);
#endif
	pmode = cr[0] & 1;
	paging = (cr[0] & 0x80000000u) != 0;
	dir = (unsigned int *)&ram[cr[3] & 0xFFFFF000u];
}
