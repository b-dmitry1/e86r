#ifndef TRANSFER_H
#define TRANSFER_H

#define SWITCH_IRET		0
#define SWITCH_INT_CALL	1
#define SWITCH_JMP		2

int switch_task(unsigned int newtss, int type);
int far_jmp(unsigned int ncs, unsigned int neip);
int far_call(unsigned int ncs, unsigned int neip);
int far_ret(unsigned short n);

#endif
