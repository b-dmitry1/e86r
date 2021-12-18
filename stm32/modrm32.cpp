#include "stdafx.h"
#include "cpu.h"
#include "modrm.h"
#include "memdescr.h"

static int m_eax() { ofs = r.eax; return 1; }
static int m_ecx() { ofs = r.ecx; return 1; }
static int m_edx() { ofs = r.edx; return 1; }
static int m_ebx() { ofs = r.ebx; return 1; }
static int m_ebp() { sel = ssel; ofs = r.ebp; return 1; }
static int m_esi() { ofs = r.esi; return 1; }
static int m_edi() { ofs = r.edi; return 1; }
static int m_sib() { ofs = sib_ea(); return 1; }
static int m_disp32() { if (!fetch32s(&modd)) return 0; ofs = modd; return 1; }

static int m_eax_d8() { ofs = r.eax; if (!fetch8s(&modd)) return 0; ofs += modd; return 1; }
static int m_ecx_d8() { ofs = r.ecx; if (!fetch8s(&modd)) return 0; ofs += modd; return 1; }
static int m_edx_d8() { ofs = r.edx; if (!fetch8s(&modd)) return 0; ofs += modd; return 1; }
static int m_ebx_d8() { ofs = r.ebx; if (!fetch8s(&modd)) return 0; ofs += modd; return 1; }
static int m_ebp_d8() { sel = ssel; ofs = r.ebp; if (!fetch8s(&modd)) return 0; ofs += modd; return 1; }
static int m_esi_d8() { ofs = r.esi; if (!fetch8s(&modd)) return 0; ofs += modd; return 1; }
static int m_edi_d8() { ofs = r.edi; if (!fetch8s(&modd)) return 0; ofs += modd; return 1; }
static int m_sib_d8() { ofs = sib_ea(); if (!fetch8s(&modd)) return 0; ofs += modd; return 1; }

static int m_eax_d32() { ofs = r.eax; if (!fetch32s(&modd)) return 0; ofs += modd; return 1; }
static int m_ecx_d32() { ofs = r.ecx; if (!fetch32s(&modd)) return 0; ofs += modd; return 1; }
static int m_edx_d32() { ofs = r.edx; if (!fetch32s(&modd)) return 0; ofs += modd; return 1; }
static int m_ebx_d32() { ofs = r.ebx; if (!fetch32s(&modd)) return 0; ofs += modd; return 1; }
static int m_ebp_d32() { sel = ssel; ofs = r.ebp; if (!fetch32s(&modd)) return 0; ofs += modd; return 1; }
static int m_esi_d32() { ofs = r.esi; if (!fetch32s(&modd)) return 0; ofs += modd; return 1; }
static int m_edi_d32() { ofs = r.edi; if (!fetch32s(&modd)) return 0; ofs += modd; return 1; }
static int m_sib_d32() { ofs = sib_ea(); if (!fetch32s(&modd)) return 0; ofs += modd; return 1; }

int (*modofs32[256])() =
{
	&m_eax, &m_ecx, &m_edx, &m_ebx, &m_sib, &m_disp32, &m_esi, &m_edi,
	&m_eax, &m_ecx, &m_edx, &m_ebx, &m_sib, &m_disp32, &m_esi, &m_edi,
	&m_eax, &m_ecx, &m_edx, &m_ebx, &m_sib, &m_disp32, &m_esi, &m_edi,
	&m_eax, &m_ecx, &m_edx, &m_ebx, &m_sib, &m_disp32, &m_esi, &m_edi,
	&m_eax, &m_ecx, &m_edx, &m_ebx, &m_sib, &m_disp32, &m_esi, &m_edi,
	&m_eax, &m_ecx, &m_edx, &m_ebx, &m_sib, &m_disp32, &m_esi, &m_edi,
	&m_eax, &m_ecx, &m_edx, &m_ebx, &m_sib, &m_disp32, &m_esi, &m_edi,
	&m_eax, &m_ecx, &m_edx, &m_ebx, &m_sib, &m_disp32, &m_esi, &m_edi,

	&m_eax_d8, &m_ecx_d8, &m_edx_d8, &m_ebx_d8, &m_sib_d8, &m_ebp_d8, &m_esi_d8, &m_edi_d8, 
	&m_eax_d8, &m_ecx_d8, &m_edx_d8, &m_ebx_d8, &m_sib_d8, &m_ebp_d8, &m_esi_d8, &m_edi_d8, 
	&m_eax_d8, &m_ecx_d8, &m_edx_d8, &m_ebx_d8, &m_sib_d8, &m_ebp_d8, &m_esi_d8, &m_edi_d8, 
	&m_eax_d8, &m_ecx_d8, &m_edx_d8, &m_ebx_d8, &m_sib_d8, &m_ebp_d8, &m_esi_d8, &m_edi_d8, 
	&m_eax_d8, &m_ecx_d8, &m_edx_d8, &m_ebx_d8, &m_sib_d8, &m_ebp_d8, &m_esi_d8, &m_edi_d8, 
	&m_eax_d8, &m_ecx_d8, &m_edx_d8, &m_ebx_d8, &m_sib_d8, &m_ebp_d8, &m_esi_d8, &m_edi_d8, 
	&m_eax_d8, &m_ecx_d8, &m_edx_d8, &m_ebx_d8, &m_sib_d8, &m_ebp_d8, &m_esi_d8, &m_edi_d8, 
	&m_eax_d8, &m_ecx_d8, &m_edx_d8, &m_ebx_d8, &m_sib_d8, &m_ebp_d8, &m_esi_d8, &m_edi_d8, 

	&m_eax_d32, &m_ecx_d32, &m_edx_d32, &m_ebx_d32, &m_sib_d32, &m_ebp_d32, &m_esi_d32, &m_edi_d32, 
	&m_eax_d32, &m_ecx_d32, &m_edx_d32, &m_ebx_d32, &m_sib_d32, &m_ebp_d32, &m_esi_d32, &m_edi_d32, 
	&m_eax_d32, &m_ecx_d32, &m_edx_d32, &m_ebx_d32, &m_sib_d32, &m_ebp_d32, &m_esi_d32, &m_edi_d32, 
	&m_eax_d32, &m_ecx_d32, &m_edx_d32, &m_ebx_d32, &m_sib_d32, &m_ebp_d32, &m_esi_d32, &m_edi_d32, 
	&m_eax_d32, &m_ecx_d32, &m_edx_d32, &m_ebx_d32, &m_sib_d32, &m_ebp_d32, &m_esi_d32, &m_edi_d32, 
	&m_eax_d32, &m_ecx_d32, &m_edx_d32, &m_ebx_d32, &m_sib_d32, &m_ebp_d32, &m_esi_d32, &m_edi_d32, 
	&m_eax_d32, &m_ecx_d32, &m_edx_d32, &m_ebx_d32, &m_sib_d32, &m_ebp_d32, &m_esi_d32, &m_edi_d32, 
	&m_eax_d32, &m_ecx_d32, &m_edx_d32, &m_ebx_d32, &m_sib_d32, &m_ebp_d32, &m_esi_d32, &m_edi_d32
};
