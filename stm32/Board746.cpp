#include "Board.h"

CPin a0(GPIOA, 0);
CPin a1(GPIOA, 1);
CPin a2(GPIOA, 2);
CPin a3(GPIOA, 3);
CPin a4(GPIOA, 4);
CPin a5(GPIOA, 5);
CPin a6(GPIOA, 6);
CPin a7(GPIOA, 7);
CPin a8(GPIOA, 8);
CPin a9(GPIOA, 9);
CPin a10(GPIOA, 10);
CPin a11(GPIOA, 11);
CPin a12(GPIOA, 12);
CPin a13(GPIOA, 13);
CPin a14(GPIOA, 14);
CPin a15(GPIOA, 15);

CPin b0(GPIOB, 0);
CPin b1(GPIOB, 1);
CPin b2(GPIOB, 2);
CPin b3(GPIOB, 3);
CPin b4(GPIOB, 4);
CPin b5(GPIOB, 5);
CPin b6(GPIOB, 6);
CPin b7(GPIOB, 7);
CPin b8(GPIOB, 8);
CPin b9(GPIOB, 9);
CPin b10(GPIOB, 10);
CPin b11(GPIOB, 11);
CPin b12(GPIOB, 12);
CPin b13(GPIOB, 13);
CPin b14(GPIOB, 14);
CPin b15(GPIOB, 15);

CPin c0(GPIOC, 0);
CPin c1(GPIOC, 1);
CPin c2(GPIOC, 2);
CPin c3(GPIOC, 3);
CPin c4(GPIOC, 4);
CPin c5(GPIOC, 5);
CPin c6(GPIOC, 6);
CPin c7(GPIOC, 7);
CPin c8(GPIOC, 8);
CPin c9(GPIOC, 9);
CPin c10(GPIOC, 10);
CPin c11(GPIOC, 11);
CPin c12(GPIOC, 12);
CPin c13(GPIOC, 13);
CPin c14(GPIOC, 14);
CPin c15(GPIOC, 15);

CPin d0(GPIOD, 0);
CPin d1(GPIOD, 1);
CPin d2(GPIOD, 2);
CPin d3(GPIOD, 3);
CPin d4(GPIOD, 4);
CPin d5(GPIOD, 5);
CPin d6(GPIOD, 6);
CPin d7(GPIOD, 7);
CPin d8(GPIOD, 8);
CPin d9(GPIOD, 9);
CPin d10(GPIOD, 10);
CPin d11(GPIOD, 11);
CPin d12(GPIOD, 12);
CPin d13(GPIOD, 13);
CPin d14(GPIOD, 14);
CPin d15(GPIOD, 15);

CPin e0(GPIOE, 0);
CPin e1(GPIOE, 1);
CPin e2(GPIOE, 2);
CPin e3(GPIOE, 3);
CPin e4(GPIOE, 4);
CPin e5(GPIOE, 5);
CPin e6(GPIOE, 6);
CPin e7(GPIOE, 7);
CPin e8(GPIOE, 8);
CPin e9(GPIOE, 9);
CPin e10(GPIOE, 10);
CPin e11(GPIOE, 11);
CPin e12(GPIOE, 12);
CPin e13(GPIOE, 13);
CPin e14(GPIOE, 14);
CPin e15(GPIOE, 15);

CPin f0(GPIOF, 0);
CPin f1(GPIOF, 1);
CPin f2(GPIOF, 2);
CPin f3(GPIOF, 3);
CPin f4(GPIOF, 4);
CPin f5(GPIOF, 5);
CPin f6(GPIOF, 6);
CPin f7(GPIOF, 7);
CPin f8(GPIOF, 8);
CPin f9(GPIOF, 9);
CPin f10(GPIOF, 10);
CPin f11(GPIOF, 11);
CPin f12(GPIOF, 12);
CPin f13(GPIOF, 13);
CPin f14(GPIOF, 14);
CPin f15(GPIOF, 15);

CPin g0(GPIOG, 0);
CPin g1(GPIOG, 1);
CPin g2(GPIOG, 2);
CPin g3(GPIOG, 3);
CPin g4(GPIOG, 4);
CPin g5(GPIOG, 5);
CPin g6(GPIOG, 6);
CPin g7(GPIOG, 7);
CPin g8(GPIOG, 8);
CPin g9(GPIOG, 9);
CPin g10(GPIOG, 10);
CPin g11(GPIOG, 11);
CPin g12(GPIOG, 12);
CPin g13(GPIOG, 13);
CPin g14(GPIOG, 14);
CPin g15(GPIOG, 15);

CPin h0(GPIOH, 0);
CPin h1(GPIOH, 1);
CPin h2(GPIOH, 2);
CPin h3(GPIOH, 3);
CPin h4(GPIOH, 4);
CPin h5(GPIOH, 5);
CPin h6(GPIOH, 6);
CPin h7(GPIOH, 7);
CPin h8(GPIOH, 8);
CPin h9(GPIOH, 9);
CPin h10(GPIOH, 10);
CPin h11(GPIOH, 11);
CPin h12(GPIOH, 12);
CPin h13(GPIOH, 13);
CPin h14(GPIOH, 14);
CPin h15(GPIOH, 15);

CPin i0(GPIOI, 0);
CPin i1(GPIOI, 1);
CPin i2(GPIOI, 2);
CPin i3(GPIOI, 3);
CPin i4(GPIOI, 4);
CPin i5(GPIOI, 5);
CPin i6(GPIOI, 6);
CPin i7(GPIOI, 7);
CPin i8(GPIOI, 8);
CPin i9(GPIOI, 9);
CPin i10(GPIOI, 10);
CPin i11(GPIOI, 11);
CPin i12(GPIOI, 12);
CPin i13(GPIOI, 13);
CPin i14(GPIOI, 14);
CPin i15(GPIOI, 15);

CUSART Serial1(1, USART1);
CUSART Serial2(2, USART2);
CUSART Serial3(3, USART3);
CUSART Serial4(4, UART4);
CUSART Serial5(5, UART5);
CUSART Serial6(6, USART6);

CUSART &Serial = Serial1;

CGPTimer Timer2(2, TIM2);
CGPTimer Timer5(5, TIM5);

CSPI spi5(5, SPI5, &f6);

volatile unsigned long time = 0;

extern "C" void TIM2_IRQHandler(void)
{
	TIM2->SR = 0;
	Timer2.IRQ();
}

extern "C" void TIM5_IRQHandler(void)
{
	TIM5->SR = 0;
	time++;
	Timer5.IRQ();
}

extern "C" void USART1_IRQHandler(void)
{
	Serial1.IRQ();
}

unsigned long GetTickCount()
{
	return time;
}

void Beep(int freq, int dur_ms)
{
}

void IntSleep(int ms)
{
	volatile int i, j;
	for (j = 0; j < ms; j++)
		for (i = 0; i < 100000; i++)
			__NOP();
}

void Sleep(int ms)
{
	IntSleep(ms);
	/*
	unsigned long t = time;
	
	while (time - t < ms);
	*/
}

void DisableIRQ(IRQn_Type irq)
{
	if (irq != 0)
	{
		NVIC_DisableIRQ(irq);
		__DSB();
		__ISB();
	}
}

void EnableIRQ(IRQn_Type irq)
{
	if (irq != 0)
	{
		NVIC_EnableIRQ(irq);
	}
}
