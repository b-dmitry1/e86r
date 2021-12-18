#include "stdafx.h"
#include "cpu.h"
#include "modrm.h"
#include "memdescr.h"

static int m_bxsi() { ofs = r.bx + r.si; ofs &= 0xFFFFu; return 1; }
static int m_bxdi() { ofs = r.bx + r.di; ofs &= 0xFFFFu; return 1; }
static int m_bpsi() { sel = ssel; ofs = r.bp + r.si; ofs &= 0xFFFFu; return 1; }
static int m_bpdi() { sel = ssel; ofs = r.bp + r.di; ofs &= 0xFFFFu; return 1; }
static int m_si() { ofs = r.si; return 1; }
static int m_di() { ofs = r.di; return 1; }
static int m_disp16() { unsigned short ofs16; if (!fetch16(&ofs16)) return 0; ofs = modd = ofs16; return 1; }
static int m_bx() { ofs = r.bx; return 1; }

static int m_bxsi_d8() { ofs = r.bx + r.si; if (!fetch8s(&modd)) return 0; ofs += modd; ofs &= 0xFFFFu; return 1; }
static int m_bxdi_d8() { ofs = r.bx + r.di; if (!fetch8s(&modd)) return 0; ofs += modd; ofs &= 0xFFFFu; return 1; }
static int m_bpsi_d8() { sel = ssel; ofs = r.bp + r.si; if (!fetch8s(&modd)) return 0; ofs += modd; ofs &= 0xFFFFu; return 1; }
static int m_bpdi_d8() { sel = ssel; ofs = r.bp + r.di; if (!fetch8s(&modd)) return 0; ofs += modd; ofs &= 0xFFFFu; return 1; }
static int m_si_d8() { ofs = r.si; if (!fetch8s(&modd)) return 0; ofs += modd; ofs &= 0xFFFFu; return 1; }
static int m_di_d8() { ofs = r.di; if (!fetch8s(&modd)) return 0; ofs += modd; ofs &= 0xFFFFu; return 1; }
static int m_bp_d8() { sel = ssel; ofs = r.bp; if (!fetch8s(&modd)) return 0; ofs += modd; ofs &= 0xFFFFu; return 1; }
static int m_bx_d8() { ofs = r.bx; if (!fetch8s(&modd)) return 0; ofs += modd; ofs &= 0xFFFFu; return 1; }

static int m_bxsi_d16() { ofs = r.bx + r.si; if (!fetch16s(&modd)) return 0; ofs += modd; ofs &= 0xFFFFu; return 1; }
static int m_bxdi_d16() { ofs = r.bx + r.di; if (!fetch16s(&modd)) return 0; ofs += modd; ofs &= 0xFFFFu; return 1; }
static int m_bpsi_d16() { sel = ssel; ofs = r.bp + r.si; if (!fetch16s(&modd)) return 0; ofs += modd; ofs &= 0xFFFFu; return 1; }
static int m_bpdi_d16() { sel = ssel; ofs = r.bp + r.di; if (!fetch16s(&modd)) return 0; ofs += modd; ofs &= 0xFFFFu; return 1; }
static int m_si_d16() { ofs = r.si; if (!fetch16s(&modd)) return 0; ofs += modd; ofs &= 0xFFFFu; return 1; }
static int m_di_d16() { ofs = r.di; if (!fetch16s(&modd)) return 0; ofs += modd; ofs &= 0xFFFFu; return 1; }
static int m_bp_d16() { sel = ssel; ofs = r.bp; if (!fetch16s(&modd)) return 0; ofs += modd; ofs &= 0xFFFFu; return 1; }
static int m_bx_d16() { ofs = r.bx; if (!fetch16s(&modd)) return 0; ofs += modd; ofs &= 0xFFFFu; return 1; }

int (*modofs16[256])() =
{
	&m_bxsi, &m_bxdi, &m_bpsi, &m_bpdi, &m_si, &m_di, &m_disp16, &m_bx,
	&m_bxsi, &m_bxdi, &m_bpsi, &m_bpdi, &m_si, &m_di, &m_disp16, &m_bx,
	&m_bxsi, &m_bxdi, &m_bpsi, &m_bpdi, &m_si, &m_di, &m_disp16, &m_bx,
	&m_bxsi, &m_bxdi, &m_bpsi, &m_bpdi, &m_si, &m_di, &m_disp16, &m_bx,
	&m_bxsi, &m_bxdi, &m_bpsi, &m_bpdi, &m_si, &m_di, &m_disp16, &m_bx,
	&m_bxsi, &m_bxdi, &m_bpsi, &m_bpdi, &m_si, &m_di, &m_disp16, &m_bx,
	&m_bxsi, &m_bxdi, &m_bpsi, &m_bpdi, &m_si, &m_di, &m_disp16, &m_bx,
	&m_bxsi, &m_bxdi, &m_bpsi, &m_bpdi, &m_si, &m_di, &m_disp16, &m_bx,

	&m_bxsi_d8, &m_bxdi_d8, &m_bpsi_d8, &m_bpdi_d8, &m_si_d8, &m_di_d8, &m_bp_d8, &m_bx_d8,
	&m_bxsi_d8, &m_bxdi_d8, &m_bpsi_d8, &m_bpdi_d8, &m_si_d8, &m_di_d8, &m_bp_d8, &m_bx_d8,
	&m_bxsi_d8, &m_bxdi_d8, &m_bpsi_d8, &m_bpdi_d8, &m_si_d8, &m_di_d8, &m_bp_d8, &m_bx_d8,
	&m_bxsi_d8, &m_bxdi_d8, &m_bpsi_d8, &m_bpdi_d8, &m_si_d8, &m_di_d8, &m_bp_d8, &m_bx_d8,
	&m_bxsi_d8, &m_bxdi_d8, &m_bpsi_d8, &m_bpdi_d8, &m_si_d8, &m_di_d8, &m_bp_d8, &m_bx_d8,
	&m_bxsi_d8, &m_bxdi_d8, &m_bpsi_d8, &m_bpdi_d8, &m_si_d8, &m_di_d8, &m_bp_d8, &m_bx_d8,
	&m_bxsi_d8, &m_bxdi_d8, &m_bpsi_d8, &m_bpdi_d8, &m_si_d8, &m_di_d8, &m_bp_d8, &m_bx_d8,
	&m_bxsi_d8, &m_bxdi_d8, &m_bpsi_d8, &m_bpdi_d8, &m_si_d8, &m_di_d8, &m_bp_d8, &m_bx_d8,

	&m_bxsi_d16, &m_bxdi_d16, &m_bpsi_d16, &m_bpdi_d16, &m_si_d16, &m_di_d16, &m_bp_d16, &m_bx_d16,
	&m_bxsi_d16, &m_bxdi_d16, &m_bpsi_d16, &m_bpdi_d16, &m_si_d16, &m_di_d16, &m_bp_d16, &m_bx_d16,
	&m_bxsi_d16, &m_bxdi_d16, &m_bpsi_d16, &m_bpdi_d16, &m_si_d16, &m_di_d16, &m_bp_d16, &m_bx_d16,
	&m_bxsi_d16, &m_bxdi_d16, &m_bpsi_d16, &m_bpdi_d16, &m_si_d16, &m_di_d16, &m_bp_d16, &m_bx_d16,
	&m_bxsi_d16, &m_bxdi_d16, &m_bpsi_d16, &m_bpdi_d16, &m_si_d16, &m_di_d16, &m_bp_d16, &m_bx_d16,
	&m_bxsi_d16, &m_bxdi_d16, &m_bpsi_d16, &m_bpdi_d16, &m_si_d16, &m_di_d16, &m_bp_d16, &m_bx_d16,
	&m_bxsi_d16, &m_bxdi_d16, &m_bpsi_d16, &m_bpdi_d16, &m_si_d16, &m_di_d16, &m_bp_d16, &m_bx_d16,
	&m_bxsi_d16, &m_bxdi_d16, &m_bpsi_d16, &m_bpdi_d16, &m_si_d16, &m_di_d16, &m_bp_d16, &m_bx_d16

};
