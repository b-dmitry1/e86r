#pragma once

typedef struct
{
	
	unsigned char imr;
	unsigned char irr;
	unsigned char isr;
	unsigned char icwstep;
	unsigned char icw[5];
	unsigned char readmode;
} pic_t;

extern pic_t pic;
extern pic_t pic2;

typedef struct
{
	unsigned char access[3];
	unsigned char toggle[3];
	unsigned short preset[3];
	unsigned short value[3];
} pit_t;

extern pit_t pit;

void irq(int n);

unsigned char pic_read(int port);
void pic_write(int port, unsigned char value);
unsigned char pit_read(int port);
void pit_write(int port, unsigned char value);

void pit_step();
int get_next_irq_vector();
