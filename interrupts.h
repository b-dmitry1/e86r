#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#define INT_FLAGS_SOFT		1
#define INT_FLAGS_FAULT		2

void interrupt(int n, int errorcode, int intflags);
void ex(int n, int errorcode);
void ex(int n);
int iret();
int cli();
int sti();
void get_ss_esp(int dpl, unsigned int *nss, unsigned int *nesp);

extern int fault;
extern unsigned int faultcode;

#endif
