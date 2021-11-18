#ifndef MEMDESCR_H
#define MEMDESCR_H

#include "cpu.h"

unsigned int gdtbase();
unsigned int idtbase();
unsigned int ldtbase();
unsigned int lin(selector_t *s, unsigned int addr);
int get_phys_addr(unsigned int addr, unsigned int *phys);
int get_phys_addr_write(unsigned int addr, unsigned int *phys);
int readphys8(unsigned int addr, unsigned char *v);
int readphys16(unsigned int addr, unsigned short *v);
int readphys32(unsigned int addr, unsigned int *v);
int writephys8(unsigned int addr, unsigned char v);
int writephys16(unsigned int addr, unsigned short v);
int writephys32(unsigned int addr, unsigned int v);
int read8(unsigned int addr, unsigned char *v);
int read16(unsigned int addr, unsigned short *v);
int read32(unsigned int addr, unsigned int *v);
int write8(unsigned int addr, unsigned char v);
int write16(unsigned int addr, unsigned short v);
int write32(unsigned int addr, unsigned int v);
int read8(selector_t *s, unsigned int addr, unsigned char *v);
int read16(selector_t *s, unsigned int addr, unsigned short *v);
int read32(selector_t *s, unsigned int addr, unsigned int *v);
int write8(selector_t *s, unsigned int addr, unsigned char v);
int write16(selector_t *s, unsigned int addr, unsigned short v);
int write32(selector_t *s, unsigned int addr, unsigned int v);
unsigned int get_limit(descr_t * d);
unsigned int get_base(descr_t *d);
unsigned int get_base_phys(descr_t *d);
int get_descr(descr_t *d, unsigned short value, int no_exceptions = 0);
int get_gate(gate_t *g, unsigned char n);
int set_selector(selector_t *s, unsigned short value, int all = 0);

int push16(unsigned short value);
int push32(unsigned int value);
int pop16(unsigned short *value);
int pop32(unsigned int *value);

int set_tss(unsigned short value);

int fetch8(unsigned char *b);
int fetch16(unsigned short *b);
int fetch32(unsigned int *b);
int fetch8s(int *b);
int fetch16s(int *b);
int fetch32s(int *b);

extern unsigned int ss_mask;
extern unsigned int ss_inv_mask;

#endif
