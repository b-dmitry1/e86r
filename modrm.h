#ifndef MODRM_H
#define MODRM_H

extern unsigned char modrm;
extern unsigned int ofs;
extern int modrm_byte;
extern int modd;
extern int sib_modd;
extern int mod_vga;

unsigned int sib_ea();

int mod(int byte);
void disasm_mod();
void disasm_modreg();
void disasm_modsreg();
unsigned int readmodreg();
void writemodreg(unsigned int value);
unsigned short readmodsreg();
int writemodsreg(unsigned short value);
int readmod(unsigned int *v);
int writemod(unsigned int value);

#endif
