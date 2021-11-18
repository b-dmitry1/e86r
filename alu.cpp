#include "stdafx.h"
#include "cpu.h"
#include "memdescr.h"
#include "interrupts.h"
#include "modrm.h"

/*

Instruction				OF SF ZF AF PF CF
AAA						-  -  -  TM -  M
AAS						-  -  -  TM -  M
AAD						-  M  M  -  M  -
AAM						-  M  M  -  M  -
DAA						-  M  M  TM M  TM
DAS						-  M  M  TM M  TM
ADC						M  M  M  M  M  TM
ADD						M  M  M  M  M  M
SBB						M  M  M  M  M  TM
SUB						M  M  M  M  M  M
CMP						M  M  M  M  M  M
CMPS					M  M  M  M  M  M
SCAS					M  M  M  M  M  M
NEG						M  M  M  M  M  M
DEC						M  M  M  M  M
INC						M  M  M  M  M
IMUL					M  -  -  -  -  M
MUL						M  -  -  -  -  M
RCL/RCR 1				M              TM
RCL/RCR count			-              TM
ROL/ROR 1				M              M
ROL/ROR count			-              M
SAL/SAR/SHL/SHR 1		M  M  M  -  M  M
SAL/SAR/SHL/SHR count	-  M  M  -  M  M
SHLD/SHRD				-  M  M  -  M  M
BSF/BSR					-  -  M  -  -  -
BT/BTS/BTR/BTC			-  -  -  -  -  M
AND						0  M  M  -  M  0
OR						0  M  M  -  M  0
TEST					0  M  M  -  M  0
XOR						0  M  M  -  M  0

*/

const unsigned char psz[256] = {
	F_P, 0, 0, F_P, 0, F_P, F_P, 0, 0, F_P, F_P, 0, F_P, 0, 0, F_P, 0, F_P, F_P, 0, F_P, 0, 0, F_P, F_P, 0, 0, F_P, 0, F_P, F_P, 0,
	0, F_P, F_P, 0, F_P, 0, 0, F_P, F_P, 0, 0, F_P, 0, F_P, F_P, 0, F_P, 0, 0, F_P, 0, F_P, F_P, 0, 0, F_P, F_P, 0, F_P, 0, 0, F_P,
	0, F_P, F_P, 0, F_P, 0, 0, F_P, F_P, 0, 0, F_P, 0, F_P, F_P, 0, F_P, 0, 0, F_P, 0, F_P, F_P, 0, 0, F_P, F_P, 0, F_P, 0, 0, F_P,
	F_P, 0, 0, F_P, 0, F_P, F_P, 0, 0, F_P, F_P, 0, F_P, 0, 0, F_P, 0, F_P, F_P, 0, F_P, 0, 0, F_P, F_P, 0, 0, F_P, 0, F_P, F_P, 0,
	0, F_P, F_P, 0, F_P, 0, 0, F_P, F_P, 0, 0, F_P, 0, F_P, F_P, 0, F_P, 0, 0, F_P, 0, F_P, F_P, 0, 0, F_P, F_P, 0, F_P, 0, 0, F_P,
	F_P, 0, 0, F_P, 0, F_P, F_P, 0, 0, F_P, F_P, 0, F_P, 0, 0, F_P, 0, F_P, F_P, 0, F_P, 0, 0, F_P, F_P, 0, 0, F_P, 0, F_P, F_P, 0,
	F_P, 0, 0, F_P, 0, F_P, F_P, 0, 0, F_P, F_P, 0, F_P, 0, 0, F_P, 0, F_P, F_P, 0, F_P, 0, 0, F_P, F_P, 0, 0, F_P, 0, F_P, F_P, 0,
	0, F_P, F_P, 0, F_P, 0, 0, F_P, F_P, 0, 0, F_P, 0, F_P, F_P, 0, F_P, 0, 0, F_P, 0, F_P, F_P, 0, 0, F_P, F_P, 0, F_P, 0, 0, F_P
};

const unsigned char psz8[256] = {
	F_P | F_Z, 0, 0, F_P, 0, F_P, F_P, 0, 0, F_P, F_P, 0, F_P, 0, 0, F_P, 0, F_P, F_P, 0, F_P, 0, 0, F_P, F_P, 0, 0, F_P, 0, F_P, F_P, 0,
	0, F_P, F_P, 0, F_P, 0, 0, F_P, F_P, 0, 0, F_P, 0, F_P, F_P, 0, F_P, 0, 0, F_P, 0, F_P, F_P, 0, 0, F_P, F_P, 0, F_P, 0, 0, F_P,
	0, F_P, F_P, 0, F_P, 0, 0, F_P, F_P, 0, 0, F_P, 0, F_P, F_P, 0, F_P, 0, 0, F_P, 0, F_P, F_P, 0, 0, F_P, F_P, 0, F_P, 0, 0, F_P,
	F_P, 0, 0, F_P, 0, F_P, F_P, 0, 0, F_P, F_P, 0, F_P, 0, 0, F_P, 0, F_P, F_P, 0, F_P, 0, 0, F_P, F_P, 0, 0, F_P, 0, F_P, F_P, 0,
	F_S, F_P | F_S, F_P | F_S, F_S, F_P | F_S, F_S, F_S, F_P | F_S, F_P | F_S, F_S, F_S, F_P | F_S, F_S, F_P | F_S, F_P | F_S, F_S, F_P | F_S, F_S, F_S, F_P | F_S, F_S, F_P | F_S, F_P | F_S, F_S, F_S, F_P | F_S, F_P | F_S, F_S, F_P | F_S, F_S, F_S, F_P | F_S,
	F_P | F_S, F_S, F_S, F_P | F_S, F_S, F_P | F_S, F_P | F_S, F_S, F_S, F_P | F_S, F_P | F_S, F_S, F_P | F_S, F_S, F_S, F_P | F_S, F_S, F_P | F_S, F_P | F_S, F_S, F_P | F_S, F_S, F_S, F_P | F_S, F_P | F_S, F_S, F_S, F_P | F_S, F_S, F_P | F_S, F_P | F_S, F_S,
	F_P | F_S, F_S, F_S, F_P | F_S, F_S, F_P | F_S, F_P | F_S, F_S, F_S, F_P | F_S, F_P | F_S, F_S, F_P | F_S, F_S, F_S, F_P | F_S, F_S, F_P | F_S, F_P | F_S, F_S, F_P | F_S, F_S, F_S, F_P | F_S, F_P | F_S, F_S, F_S, F_P | F_S, F_S, F_P | F_S, F_P | F_S, F_S,
	F_S, F_P | F_S, F_P | F_S, F_S, F_P | F_S, F_S, F_S, F_P | F_S, F_P | F_S, F_S, F_S, F_P | F_S, F_S, F_P | F_S, F_P | F_S, F_S, F_P | F_S, F_S, F_S, F_P | F_S, F_S, F_P | F_S, F_P | F_S, F_S, F_S, F_P | F_S, F_P | F_S, F_S, F_P | F_S, F_S, F_S, F_P | F_S
};

void setpsz8(unsigned char value)
{
	r.flags &= ~(F_P | F_S | F_Z);
	r.flags |= psz8[value];
}

void setpsz16(unsigned short value)
{
	r.flags &= ~(F_P | F_S | F_Z);
	r.flags |= psz[value & 0xFF];
	if (value & 0x8000)
		r.flags |= F_S;
	if (value == 0)
		r.flags |= F_Z;
}

void setpsz32(unsigned int value)
{
	r.flags &= ~(F_P | F_S | F_Z);
	r.flags |= psz[value & 0xFF];
	if (value & 0x80000000u)
		r.flags |= F_S;
	if (value == 0)
		r.flags |= F_Z;
}

unsigned short signext8(unsigned char v)
{
	if (v & 0x80)
		return v | 0xFF00u;
	return v;
}

unsigned int signext16(unsigned short v)
{
	if (v & 0x8000u)
		return v | 0xFFFF0000u;
	return v;
}

unsigned char add8(unsigned char a, unsigned char b, unsigned char c)
{
	unsigned short res = a + b + c;
	setpsz8((unsigned char)res);
	r.flags &= ~(F_C | F_O | F_A);
	if ((res ^ a) & (res ^ b) & 0x80)
		r.flags |= F_O;
	if (res & 0xFF00)
		r.flags |= F_C;
	if ((res ^ a ^ b) & 0x10)
		r.flags |= F_A;
	return (unsigned char)res;
}

unsigned short add16(unsigned short a, unsigned short b, unsigned short c)
{
	unsigned int res = a + b + c;
	setpsz16(res);
	r.flags &= ~(F_C | F_O | F_A);
	if ((res ^ a) & (res ^ b) & 0x8000)
		r.flags |= F_O;
	if (res & 0xFFFF0000u)
		r.flags |= F_C;
	if ((res ^ a ^ b) & 0x10)
		r.flags |= F_A;
	return res;
}

unsigned int add32(unsigned int a, unsigned int b, unsigned int c)
{
	long long res64;
	unsigned int res;
	res64 = (long long)a + b + c;
	res = a + b + c;
	setpsz32(res);
	r.flags &= ~(F_C | F_O | F_A);
	if ((res ^ a) & (res ^ b) & 0x80000000u)
		r.flags |= F_O;
	if (res64 > 0xFFFFFFFFll)
		r.flags |= F_C;
	if ((res ^ a ^ b) & 0x10)
		r.flags |= F_A;
	return res;
}

unsigned char sub8(unsigned char a, unsigned char b, unsigned char c)
{
	unsigned short res;
	b += c;
	res = (unsigned short)a - (unsigned short)b;
	setpsz8((unsigned char)res);
	r.flags &= ~(F_C | F_O | F_A);
	if ((res ^ a) & (a ^ b) & 0x80)
		r.flags |= F_O;
	if (res & 0xFF00)
		r.flags |= F_C;
	if ((res ^ a ^ b) & 0x10)
		r.flags |= F_A;
	return (unsigned char)res;
}

unsigned short sub16(unsigned short a, unsigned short b, unsigned short c)
{
	b += c;
	unsigned int res;
	res = (unsigned int)a - (unsigned int)b;
	setpsz16((unsigned short)res);
	r.flags &= ~(F_C | F_O | F_A);
	if ((res ^ a) & (a ^ b) & 0x8000u)
		r.flags |= F_O;
	if (res & 0xFFFF0000u)
		r.flags |= F_C;
	if ((res ^ a ^ b) & 0x10)
		r.flags |= F_A;
	return res;
}

unsigned int sub32(unsigned int a, unsigned int b, unsigned int c)
{
	b += c;
	unsigned int res;
	res = (unsigned int)a - (unsigned int)b;
	setpsz32(res);
	r.flags &= ~(F_C | F_O | F_A);
	if ((res ^ a) & (a ^ b) & 0x80000000u)
		r.flags |= F_O;
	if (a < b)
		r.flags |= F_C;
	if ((res ^ a ^ b) & 0x10)
		r.flags |= F_A;
	return res;
}

unsigned char inc8(unsigned char a)
{
	unsigned char res;
	res = a + 1;
	setpsz8(res);
	r.flags &= ~(F_O | F_A);
	if (res == 0x80)
		r.flags |= F_O;
	if ((res & 0x0F) == 0)
		r.flags |= F_A;
	return res;
}

unsigned short inc16(unsigned short a)
{
	unsigned short res;
	res = a + 1;
	setpsz16(res);
	r.flags &= ~(F_O | F_A);
	if (res == 0x8000)
		r.flags |= F_O;
	if ((res & 0x0F) == 0)
		r.flags |= F_A;
	return res;
}

unsigned int inc32(unsigned int a)
{
	unsigned int res;
	res = a + 1;
	setpsz32(res);
	r.flags &= ~(F_O | F_A);
	if (res == 0x80000000u)
		r.flags |= F_O;
	if ((res & 0x0F) == 0)
		r.flags |= F_A;
	return res;
}

unsigned char dec8(unsigned char a)
{
	unsigned char res;
	res = a - 1;
	setpsz8(res);
	r.flags &= ~(F_O | F_A);
	if (res == 0x7F)
		r.flags |= F_O;
	if ((res & 0x0F) == 0x0F)
		r.flags |= F_A;
	return res;
}

unsigned short dec16(unsigned short a)
{
	unsigned short res;
	res = a - 1;
	setpsz16(res);
	r.flags &= ~(F_O | F_A);
	if (res == 0x7FFF)
		r.flags |= F_O;
	if ((res & 0x0F) == 0x0F)
		r.flags |= F_A;
	return res;
}

unsigned int dec32(unsigned int a)
{
	unsigned int res;
	res = a - 1;
	setpsz32(res);
	r.flags &= ~(F_O | F_A);
	if (res == 0x7FFFFFFFu)
		r.flags |= F_O;
	if ((res & 0x0F) == 0x0F)
		r.flags |= F_A;
	return res;
}

unsigned char or8(unsigned char a, unsigned char b)
{
	unsigned int res;
	res = a | b;
	setpsz8(res);
	r.flags &= ~(F_C | F_O);
	return res;
}

unsigned short or16(unsigned short a, unsigned short b)
{
	unsigned int res;
	res = a | b;
	setpsz16(res);
	r.flags &= ~(F_C | F_O);
	return res;
}

unsigned int or32(unsigned int a, unsigned int b)
{
	unsigned int res;
	res = a | b;
	setpsz32(res);
	r.flags &= ~(F_C | F_O);
	return res;
}

unsigned char and8(unsigned char a, unsigned char b)
{
	unsigned int res;
	res = a & b;
	setpsz8(res);
	r.flags &= ~(F_C | F_O);
	return res;
}

unsigned short and16(unsigned short a, unsigned short b)
{
	unsigned int res;
	res = a & b;
	setpsz16(res);
	r.flags &= ~(F_C | F_O);
	return res;
}

unsigned int and32(unsigned int a, unsigned int b)
{
	unsigned int res;
	res = a & b;
	setpsz32(res);
	r.flags &= ~(F_C | F_O);
	return res;
}

void test8(unsigned char a, unsigned char b)
{
	setpsz8(a & b);
	r.flags &= ~(F_C | F_O);
}

void test16(unsigned short a, unsigned short b)
{
	setpsz16(a & b);
	r.flags &= ~(F_C | F_O);
}

void test32(unsigned int a, unsigned int b)
{
	setpsz32(a & b);
	r.flags &= ~(F_C | F_O);
}

unsigned char xor8(unsigned char a, unsigned char b)
{
	unsigned int res;
	res = a ^ b;
	setpsz8(res);
	r.flags &= ~(F_C | F_O);
	return res;
}

unsigned short xor16(unsigned short a, unsigned short b)
{
	unsigned int res;
	res = a ^ b;
	setpsz16(res);
	r.flags &= ~(F_C | F_O);
	return res;
}

unsigned int xor32(unsigned int a, unsigned int b)
{
	unsigned int res;
	res = a ^ b;
	setpsz32(res);
	r.flags &= ~(F_C | F_O);
	return res;
}

unsigned short imul8(char a, char b)
{
	unsigned short res = (unsigned short)(a * b);

	if (((res & 0xFF80) == 0xFF80) || ((res & 0xFF80) == 0))
		r.flags &= ~(F_C | F_O);
	else
		r.flags |= F_C | F_O;
	return res;
}

unsigned int imul16(short a, short b)
{
	unsigned int res = (unsigned int)(a * b);

	if (((res & 0xFFFF8000u) == 0xFFFF8000u) || ((res & 0xFFFF8000u) == 0))
		r.flags &= ~(F_C | F_O);
	else
		r.flags |= F_C | F_O;
	return res;
}

unsigned long long imul32(int a, int b)
{
	unsigned long long res = (unsigned long long)(a * (long long)b);

	if (((res & 0xFFFFFFFF80000000ull) == 0xFFFFFFFF80000000ull) || ((res & 0xFFFFFFFF80000000ull) == 0))
		r.flags &= ~(F_C | F_O);
	else
		r.flags |= F_C | F_O;
	return res;
}

unsigned short mul8(unsigned char a, unsigned char b)
{
	unsigned int res;
	res = a * b;
	if (res == 0)
		r.flags |= F_Z;
	else
		r.flags &= ~F_Z;
	if (res & 0xFF00u)
		r.flags |= F_C | F_O;
	else
		r.flags &= ~(F_C | F_O);
	return res;
}

unsigned int mul16(unsigned short a, unsigned short b)
{
	unsigned int res;
	res = a * b;
	if (res == 0)
		r.flags |= F_Z;
	else
		r.flags &= ~F_Z;
	if (res & 0xFFFF0000u)
		r.flags |= F_C | F_O;
	else
		r.flags &= ~(F_C | F_O);
	return res;
}

unsigned long long mul32(unsigned int a, unsigned int b)
{
	unsigned long long res;
	res = a * (unsigned long long)b;
	if (res == 0)
		r.flags |= F_Z;
	else
		r.flags &= ~F_Z;
	if (res & 0xFFFFFFFF00000000ull)
		r.flags |= F_C | F_O;
	else
		r.flags &= ~(F_C | F_O);
	return res;
}

int idiv8(short a, char b, char *q, char *r)
{
	int qu;

	if (b == 0)
	{
		ex(0);
		return 0;
	}

	qu = a / b;
	*q = qu;
	*r = a % b;
	if (qu != *q)
	{
		ex(0);
		return 0;
	}
	return 1;
}

int idiv16(int a, short b, short *q, short *r)
{
	int qu;

	if (b == 0)
	{
		ex(0);
		return 0;
	}

	qu = a / b;
	*q = qu;
	*r = a % b;
	if (qu != *q)
	{
		ex(0);
		return 0;
	}
	return 1;
}

int idiv32(long long a, int b, int *q, int *r)
{
	long long qu;

	if (b == 0)
	{
		ex(0);
		return 0;
	}

	qu = a / b;
	*q = (int)qu;
	*r = (int)(a - *q * (long long)b);
	if (qu != *q)
	{
		ex(0);
		return 0;
	}
	return 1;
}

int div8(unsigned short a, unsigned char b, unsigned char *q, unsigned char *r)
{
	unsigned short qu;
	if (b == 0)
	{
		ex(0);
		return 0;
	}
	qu = a / b;
	*q = (unsigned char)qu;
	*r = a % b;
	if (qu != *q)
	{
		ex(0);
		return 0;
	}
	return 1;
}

int div16(unsigned int a, unsigned short b, unsigned short *q, unsigned short *r)
{
	unsigned int qu;
	if (b == 0)
	{
		ex(0);
		return 0;
	}
	qu = a / b;
	*q = qu;
	*r = a % b;
	if (qu != *q)
	{
		ex(0);
		return 0;
	}
	return 1;
}

int div32(unsigned long long a, unsigned long long b, unsigned int *q, unsigned int *r)
{
	unsigned long long qu;
	if (b == 0)
	{
		ex(0);
		return 0;
	}
	qu = a / b;
	*q = (unsigned int)qu;
	*r = (unsigned int)(a % b);
	if (qu != *q)
	{
		ex(0);
		return 0;
	}
	return 1;
}

unsigned short dshl16(unsigned short a, unsigned short b, unsigned short n)
{
	unsigned short v;
	n &= 15;
	v = (a << n) | (b >> (16 - n));
	if ((a << (n - 1)) & 0x8000u)
		r.eflags |= F_C;
	else
		r.eflags &= ~F_C;
	setpsz16(v);
	return v;
}

unsigned int dshl32(unsigned int a, unsigned int b, unsigned int n)
{
	unsigned int v;
	n &= 31;
	v = (a << n) | (b >> (32 - n));
	if ((a << (n - 1)) & 0x80000000u)
		r.eflags |= F_C;
	else
		r.eflags &= ~F_C;
	setpsz32(v);
	return v;
}

unsigned short dshr16(unsigned short a, unsigned short b, unsigned short n)
{
	unsigned short v;
	n &= 15;
	v = (a >> n) | (b << (16 - n));
	if ((a >> (n - 1)) & 1)
		r.eflags |= F_C;
	else
		r.eflags &= ~F_C;
	setpsz16(v);
	return v;
}

unsigned int dshr32(unsigned int a, unsigned int b, unsigned int n)
{
	unsigned int v;
	n &= 31;
	v = (a >> n) | (b << (32 - n));
	if ((a >> (n - 1)) & 1)
		r.eflags |= F_C;
	else
		r.eflags &= ~F_C;
	setpsz32(v);
	return v;
}

unsigned char rol8(unsigned char v, unsigned char n)
{
	unsigned char res;
	n &= 7;
	res = (v << n) | (v >> (8 - n));
	if (res & 1)
		r.flags |= F_C;
	else
		r.flags &= ~F_C;
	if ((res ^ (res >> 7)) & 0x01)
		r.flags |= F_O;
	else
		r.flags &= ~F_O;
	return res;
}

unsigned char ror8(unsigned char v, unsigned char n)
{
	unsigned char res;
	n &= 7;
	res = (v >> n) | (v << (8 - n));
	if (res & 0x80)
		r.flags |= F_C;
	else
		r.flags &= ~F_C;
	if ((res ^ (res << 1)) & 0x80)
		r.flags |= F_O;
	else
		r.flags &= ~F_O;
	return res;
}

unsigned char rcl8(unsigned char v, unsigned char n)
{
	unsigned char res, c;
	c = r.eflags & F_C ? 1 : 0;
	n %= 9;
	res = (v << n) | (c << (n - 1)) | (v >> (9 - n));
	if ((v >> (8 - n)) & 1)
		r.flags |= F_C;
	else
		r.flags &= ~F_C;
	c = r.eflags & F_C ? 1 : 0;
	if ((c ^ (res >> 7)) & 0x01)
		r.flags |= F_O;
	else
		r.flags &= ~F_O;
	return res;
}

unsigned char rcr8(unsigned char v, unsigned char n)
{
	unsigned char res, c;
	c = r.eflags & F_C ? 1 : 0;
	n %= 9;
	res = (v >> n) | (c << (8 - n)) | (v << (9 - n));
	if ((v >> (n - 1)) & 1)
		r.flags |= F_C;
	else
		r.flags &= ~F_C;
	if ((res ^ (res << 1)) & 0x80)
		r.flags |= F_O;
	else
		r.flags &= ~F_O;
	return res;
}

unsigned char shl8(unsigned char v, unsigned char n)
{
	int i;

	for (i = 0; i < n; i++)
	{
		if (v & 0x80)
			r.flags |= F_C;
		else
			r.flags &= ~F_C;
		v = v << 1;
	}

	setpsz8(v);
	r.flags &= ~F_O;

	if (n == 1)
		if ((v ^ (v << 1)) & 0x80)
			r.flags |= F_O;

	return v;

	if ((v << (n - 1)) & 0x80)
		r.flags |= F_C;
	else
		r.flags &= ~F_C;
	v = v << n;
	setpsz8(v);
	r.flags &= ~F_O;
	if ((v ^ (v << 1)) & 0x80)
		r.flags |= F_O;
	return v;
}

unsigned short shl16(unsigned short v, unsigned char n)
{
	int i;

	for (i = 0; i < n; i++)
	{
		if (v & 0x8000)
			r.flags |= F_C;
		else
			r.flags &= ~F_C;
		v = v << 1;
	}

	setpsz16(v);
	r.flags &= ~F_O;

	if (n == 1)
		if ((v ^ (v << 1)) & 0x8000)
			r.flags |= F_O;

	return v;

	if ((v << (n - 1)) & 0x8000)
		r.flags |= F_C;
	else
		r.flags &= ~F_C;
	v = v << n;
	setpsz16(v);
	r.flags &= ~F_O;
	if ((v ^ (v << 1)) & 0x8000)
		r.flags |= F_O;
	return v;
}

unsigned int shl32(unsigned int v, unsigned char n)
{
	int i;

	for (i = 0; i < n; i++)
	{
		if (v & 0x80000000u)
			r.flags |= F_C;
		else
			r.flags &= ~F_C;
		v = v << 1;
	}

	setpsz32(v);
	r.flags &= ~F_O;

	if (n == 1)
		if ((v ^ (v << 1)) & 0x80000000u)
			r.flags |= F_O;

	return v;

	if ((v << (n - 1)) & 0x80000000u)
		r.flags |= F_C;
	else
		r.flags &= ~F_C;
	v = v << n;
	setpsz32(v);
	r.flags &= ~F_O;
	if ((v ^ (v << 1)) & 0x80000000u)
		r.flags |= F_O;
	return v;
}

unsigned short rol16(unsigned short v, unsigned char n)
{
	unsigned short res;
	n &= 15;
	res = (v << n) | (v >> (16 - n));
	if (res & 1)
		r.flags |= F_C;
	else
		r.flags &= ~F_C;
	if ((res ^ (res >> 15)) & 0x01)
		r.flags |= F_O;
	else
		r.flags &= ~F_O;
	return res;
}

unsigned int rol32(unsigned int v, unsigned char n)
{
	unsigned int res;
	n &= 31;
	res = (v << n) | (v >> (32 - n));
	if (res & 1)
		r.flags |= F_C;
	else
		r.flags &= ~F_C;
	if ((res ^ (res >> 31u)) & 0x01)
		r.flags |= F_O;
	else
		r.flags &= ~F_O;
	return res;
}

unsigned short ror16(unsigned short v, unsigned char n)
{
	unsigned short res;
	n &= 15;
	res = (v >> n) | (v << (16 - n));
	if (res & 0x8000)
		r.flags |= F_C;
	else
		r.flags &= ~F_C;
	if ((res ^ (res << 1)) & 0x8000)
		r.flags |= F_O;
	else
		r.flags &= ~F_O;
	return res;
}

unsigned int ror32(unsigned int v, unsigned char n)
{
	unsigned int res;
	n &= 31;
	res = (v >> n) | (v << (32 - n));
	if (res & 0x80000000u)
		r.flags |= F_C;
	else
		r.flags &= ~F_C;
	if ((res ^ (res << 1)) & 0x80000000u)
		r.flags |= F_O;
	else
		r.flags &= ~F_O;
	return res;
}

unsigned short rcl16(unsigned short v, unsigned char n)
{
	unsigned short res, c;
	c = r.eflags & F_C ? 1 : 0;
	n %= 17;
	res = (v << n) | (c << (n - 1)) | (v >> (17 - n));
	if ((v >> (16 - n)) & 1)
		r.flags |= F_C;
	else
		r.flags &= ~F_C;
	c = r.eflags & F_C ? 1 : 0;
	if ((c ^ (res >> 15)) & 0x01)
		r.flags |= F_O;
	else
		r.flags &= ~F_O;
	return res;
}

unsigned int rcl32(unsigned int v, unsigned char n)
{
	unsigned int res, c;
	c = r.eflags & F_C ? 1 : 0;
	if (n == 1)
		res = (v << 1u) | c;
	else
		res = (v << n) | (c << (n - 1)) | (v >> (33u - n));
	if ((v >> (32u - n)) & 1)
		r.flags |= F_C;
	else
		r.flags &= ~F_C;
	c = r.eflags & F_C ? 1 : 0;
	if ((c ^ (res >> 31)) & 0x01)
		r.flags |= F_O;
	else
		r.flags &= ~F_O;
	return res;
}

unsigned short rcr16(unsigned short v, unsigned char n)
{
	unsigned short res, c;
	c = r.eflags & F_C ? 1 : 0;
	n %= 17;
	res = (v >> n) | (c << (16 - n)) | (v << (17 - n));
	if ((v >> (n - 1)) & 1)
		r.flags |= F_C;
	else
		r.flags &= ~F_C;
	if ((res ^ (res << 1)) & 0x8000)
		r.flags |= F_O;
	else
		r.flags &= ~F_O;
	return res;
}

unsigned int rcr32(unsigned int v, unsigned char n)
{
	unsigned int res, c;
	c = r.eflags & F_C ? 1 : 0;
	if (n == 1)
		res = (v >> 1) | (v << 31u);
	else
		res = (v >> n) | (c << (32 - n)) | (v << (33 - n));
	if ((v >> (n - 1)) & 1)
		r.flags |= F_C;
	else
		r.flags &= ~F_C;
	if ((res ^ (res << 1)) & 0x80000000u)
		r.flags |= F_O;
	else
		r.flags &= ~F_O;
	return res;
}

unsigned char shr8(unsigned char v, unsigned char n)
{
	if ((v >> (n - 1)) & 0x01)
		r.flags |= F_C;
	else
		r.flags &= ~F_C;
	v = v >> n;
	setpsz8(v);
	r.flags &= ~F_O;
	if ((v ^ (v << 1)) & 0x80)
		r.flags |= F_O;
	return v;
}

unsigned short shr16(unsigned short v, unsigned char n)
{
	if ((v >> (n - 1)) & 0x01)
		r.flags |= F_C;
	else
		r.flags &= ~F_C;
	v = v >> n;
	setpsz16(v);
	r.flags &= ~F_O;
	if ((v ^ (v << 1)) & 0x8000)
		r.flags |= F_O;
	return v;
}

unsigned int shr32(unsigned int v, unsigned char n)
{
	if ((v >> (n - 1)) & 0x01)
		r.flags |= F_C;
	else
		r.flags &= ~F_C;
	v = v >> n;
	setpsz32(v);
	r.flags &= ~F_O;
	if ((v ^ (v << 1)) & 0x80000000u)
		r.flags |= F_O;
	return v;
}

unsigned char sar8(unsigned char v, unsigned char n)
{
	unsigned char res;
	if (n > 8)
		n = 8;
	if (v & 0x80)
	{
		res = (v >> n) | (0xff << (8 - n));
		n--;
		if (((v >> n) | (0xff << (8 - n))) & 1)
			r.flags |= F_C;
		else
			r.flags &= ~F_C;
	}
	else
	{
		res = v >> n;
		n--;
		if ((v >> n) & 1)
			r.flags |= F_C;
		else
			r.flags &= ~F_C;
	}
	setpsz8(res);
	r.flags &= ~F_O;
	return res;
}

unsigned short sar16(unsigned short v, unsigned char n)
{
	unsigned short res;
	if (n > 16)
		n = 16;
	if (v & 0x8000)
	{
		res = (v >> n) | (0xffff << (16 - n));
		n--;
		if (((v >> n) | (0xffff << (16 - n))) & 1)
			r.flags |= F_C;
		else
			r.flags &= ~F_C;
	}
	else
	{
		res = v >> n;
		n--;
		if ((v >> n) & 1)
			r.flags |= F_C;
		else
			r.flags &= ~F_C;
	}
	setpsz16(res);
	r.flags &= ~F_O;
	return res;
}

unsigned int sar32(unsigned int v, unsigned char n)
{
	unsigned int res;
	if (n > 32)
		n = 32;
	if (v & 0x80000000u)
	{
		res = (v >> n) | (0xffffffffu << (32 - n));
		n--;
		if (((v >> n) | (0xffffffffu << (32 - n))) & 1)
			r.flags |= F_C;
		else
			r.flags &= ~F_C;
	}
	else
	{
		res = v >> n;
		n--;
		if ((v >> n) & 1)
			r.flags |= F_C;
		else
			r.flags &= ~F_C;
	}
	setpsz32(res);
	r.flags &= ~F_O;
	return res;
}

unsigned short bswap16(unsigned short v)
{
	// Undefined for 16 bit
	return v;
}

unsigned int bswap32(unsigned int v)
{
	v = (v >> 24) | ((v >> 8) & 0xFF00) | ((v << 8) & 0xFF0000u) | ((v << 24) & 0xFF000000u);
	return v;
}

void bts16(int n)
{
	unsigned int v, c, mask;
	mask = (unsigned int)(1u << (n & 15u));
	n /= 16;
	n *= 2;
	ofs += (unsigned int)n;
	if (!readmod(&v))
		return;
	c = v & mask ? 1 : 0;
	v |= mask;
	if (!writemod(v))
		return;
	if (c)
		r.eflags |= F_C;
	else
		r.eflags &= ~F_C;
}

void btr16(int n)
{
	unsigned int v, c, mask;
	mask = (unsigned int)(1u << (n & 15u));
	n /= 16;
	n *= 2;
	ofs += (unsigned int)n;
	if (!readmod(&v))
		return;
	c = v & mask ? 1 : 0;
	v &= ~mask;
	if (!writemod(v))
		return;
	if (c)
		r.eflags |= F_C;
	else
		r.eflags &= ~F_C;
}

void btc16(int n)
{
	unsigned int v, c, mask;
	mask = (unsigned int)(1u << (n & 15u));
	n /= 16;
	n *= 2;
	ofs += (unsigned int)n;
	if (!readmod(&v))
		return;
	c = v & mask ? 1 : 0;
	v ^= mask;
	if (!writemod(v))
		return;
	if (c)
		r.eflags |= F_C;
	else
		r.eflags &= ~F_C;
}

void bt16(int n)
{
	unsigned int v, c, mask;
	mask = (unsigned int)(1u << (n & 15u));
	n /= 16;
	n *= 2;
	ofs += (unsigned int)n;
	if (!readmod(&v))
		return;
	c = v & mask ? 1 : 0;
	if (c)
		r.eflags |= F_C;
	else
		r.eflags &= ~F_C;
}

void bts32(int n)
{
	unsigned int v, c, mask;
	mask = (unsigned int)(1u << (n & 31u));
	n /= 32;
	n *= 4;
	ofs += (unsigned int)n;
	if (!readmod(&v))
		return;
	c = v & mask ? 1 : 0;
	v |= mask;
	if (!writemod(v))
		return;
	if (c)
		r.eflags |= F_C;
	else
		r.eflags &= ~F_C;
}

void btr32(int n)
{
	unsigned int v, c, mask;
	mask = (unsigned int)(1u << (n & 31u));
	n /= 32;
	n *= 4;
	ofs += (unsigned int)n;
	if (!readmod(&v))
		return;
	c = v & mask ? 1 : 0;
	v &= ~mask;
	if (!writemod(v))
		return;
	if (c)
		r.eflags |= F_C;
	else
		r.eflags &= ~F_C;
}

void btc32(int n)
{
	unsigned int v, c, mask;
	mask = (unsigned int)(1u << (n & 31u));
	n /= 32;
	n *= 4;
	ofs += (unsigned int)n;
	if (!readmod(&v))
		return;
	c = v & mask ? 1 : 0;
	v ^= mask;
	if (!writemod(v))
		return;
	if (c)
		r.eflags |= F_C;
	else
		r.eflags &= ~F_C;
}

void bt32(int n)
{
	unsigned int v, c, mask;
	mask = (unsigned int)(1u << (n & 31u));
	n /= 32;
	n *= 4;
	ofs += (unsigned int)n;
	if (!readmod(&v))
		return;
	c = v & mask ? 1 : 0;
	if (c)
		r.eflags |= F_C;
	else
		r.eflags &= ~F_C;
}

unsigned short bsf16(unsigned short dst, unsigned short src)
{
	unsigned short bit, mask = 1;
	if (src == 0)
	{
		r.eflags |= F_Z;
	}
	else
	{
		r.eflags &= ~F_Z;
		for (bit = 0; bit < 16; bit++, mask <<= 1)
		{
			if (src & mask)
			{
				dst = bit;
				break;
			}
		}
	}
	return dst;
}

unsigned int bsf32(unsigned int dst, unsigned int src)
{
	unsigned int bit, mask = 1;
	if (src == 0)
	{
		r.eflags |= F_Z;
	}
	else
	{
		r.eflags &= ~F_Z;
		for (bit = 0; bit < 32; bit++, mask <<= 1)
		{
			if (src & mask)
			{
				dst = bit;
				break;
			}
		}
	}
	return dst;
}

unsigned short bsr16(unsigned short dst, unsigned short src)
{
	unsigned short bit, mask = 0x8000u;
	if (src == 0)
	{
		r.eflags |= F_Z;
	}
	else
	{
		r.eflags &= ~F_Z;
		for (bit = 0; bit < 16; bit++, mask >>= 1)
		{
			if (src & mask)
			{
				dst = 15 - bit;
				break;
			}
		}
	}
	return dst;
}

unsigned int bsr32(unsigned int dst, unsigned int src)
{
	unsigned int bit, mask = 0x80000000u;
	if (src == 0)
	{
		r.eflags |= F_Z;
	}
	else
	{
		r.eflags &= ~F_Z;
		for (bit = 0; bit < 32; bit++, mask >>= 1)
		{
			if (src & mask)
			{
				dst = 31 - bit;
				break;
			}
		}
	}
	return dst;
}
