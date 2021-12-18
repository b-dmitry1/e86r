#ifndef BOARD_H
#define BOARD_H

#include "stm32f746xx.h"                  // Device header
#include <stdio.h>

#define NOSLEEP

#define PCLK			192000000ul

#define APB1_CLK		(PCLK / 4)
#define APB2_CLK		(PCLK / 4)
#define AHB_CLK			(PCLK / 1)

#define INPUT_PULLUP		0
#define INPUT_PULLDOWN		1
#define INPUT				2
#define OUTPUT				3
#define OPENDRAIN			4
#define ALTERNATE			5

#define LOW					0
#define HIGH				1

#define STOPBITSMASK		0x30
#define ONESTOPBIT			(0 << 4)
#define TWOSTOPBITS			(2 << 4)

#define PARITYMASK			0xC0
#define NOPARITY			(0 << 6)
#define EVENPARITY			(1 << 6)
#define ODDPARITY			(2 << 6)

#define SERIAL_8N1			(8 | ONESTOPBIT | NOPARITY)
#define SERIAL_8E1			(8 | ONESTOPBIT | EVENPARITY)
#define SERIAL_8O1			(8 | ONESTOPBIT | ODDPARITY)
#define SERIAL_8N2			(8 | TWOSTOPBIT | NOPARITY)
#define SERIAL_8E2			(8 | TWOSTOPBIT | EVENPARITY)
#define SERIAL_8O2			(8 | TWOSTOPBIT | ODDPARITY)
#define SERIAL_7N1			(7 | ONESTOPBIT | NOPARITY)
#define SERIAL_7E1			(7 | ONESTOPBIT | EVENPARITY)
#define SERIAL_7O1			(7 | ONESTOPBIT | ODDPARITY)
#define SERIAL_7N2			(7 | TWOSTOPBIT | NOPARITY)
#define SERIAL_7E2			(7 | TWOSTOPBIT | EVENPARITY)
#define SERIAL_7O2			(7 | TWOSTOPBIT | ODDPARITY)

#define MSBFIRST			0
#define LSBFIRST			1
#define SPI_MODE0			0
#define SPI_MODE1			1
#define SPI_MODE2			2
#define SPI_MODE3			3
#define SPI_CLOCK_DIV2		0
#define SPI_CLOCK_DIV4		1
#define SPI_CLOCK_DIV8		2
#define SPI_CLOCK_DIV16		3
#define SPI_CLOCK_DIV32		4
#define SPI_CLOCK_DIV64		5
#define SPI_CLOCK_DIV128	6
#define SPI_CLOCK_DIV256	7

#define PIN_MODE_INPUT		0x00
#define PIN_MODE_OUTPUT		0x01
#define PIN_MODE_AF			0x02
#define PIN_MODE_ANALOG		0x03

#define PIN_TYPE_PUSHPULL	0x00
#define PIN_TYPE_OPENDRAIN	0x01

#define PIN_SPEED_LOW		0x00
#define PIN_SPEED_MEDIUM	0x01
#define PIN_SPEED_HIGH		0x02
#define PIN_SPEED_VERYHIGH	0x03

#define PIN_PULL_FLOAT		0x00
#define PIN_PULL_UP			0x01
#define PIN_PULL_DOWN		0x02

/*

// Waveshare 10" TFT

#define H_PW				10
#define H_BP				100
#define H_VD				1024
#define H_FP				210

#define V_PW				10
#define V_BP				10
#define V_VD				600
#define V_FP				18
*/

/*

// 1024x768

#define H_PW				136
#define H_BP				160
#define H_VD				1024
#define H_FP				24

#define V_PW				6
#define V_BP				29
#define V_VD				768
#define V_FP				3
*/


// 640x480

#define H_PW				96
#define H_BP				48
#define H_VD				640
#define H_FP				16

#define V_PW				2
#define V_BP				33
#define V_VD				480
#define V_FP				10


#define DCLK_POL			0
#define DEN_POL				0
#define VSYNC_POL			0
#define HSYNC_POL			0

extern volatile unsigned long time;

void Sleep(int ms);
unsigned long GetTickCount();
void Beep(int freq, int dur_ms);

class CPin
{
	GPIO_TypeDef *m_base;
	int m_pin;
	unsigned long m_mask;
	unsigned long m_resetmask;
public:
	CPin(GPIO_TypeDef *base, int pin)
	{
		m_base = base;
		m_pin = pin;
		m_mask = (1ul << pin);
		m_resetmask = m_mask << 16ul;
	}
	
	static void SetConfig(GPIO_TypeDef *base, unsigned long pin, int func, int mode, int type, int pull, int speed)
	{
		if (pin < 8)
			base->AFR[0] = (base->AFR[0] & (~(15ul << (pin * 4)))) | (func << (pin * 4ul));
		else
			base->AFR[1] = (base->AFR[1] & (~(15ul << ((pin - 8) * 4)))) | (func << ((pin - 8) * 4ul));
		base->OTYPER = (base->OTYPER & (~(1ul << pin))) | (type << pin);
		base->OSPEEDR = (base->PUPDR & (~(3ul << (pin * 2)))) | (speed << (pin * 2));
		base->MODER = (base->MODER & (~(3ul << (pin * 2)))) | (mode << (pin * 2));
		base->PUPDR = (base->PUPDR & (~(3ul << (pin * 2)))) | (pull << (pin * 2));		
	}
	
	void SetPinConfig(int func, int mode, int type, int pull, int speed)
	{
		if (m_pin < 8)
			m_base->AFR[0] = (m_base->AFR[0] & (~(15ul << (m_pin * 4)))) | (func << (m_pin * 4ul));
		else
			m_base->AFR[1] = (m_base->AFR[1] & (~(15ul << ((m_pin - 8) * 4)))) | (func << ((m_pin - 8) * 4ul));
		m_base->OTYPER = (m_base->OTYPER & (~(1ul << m_pin))) | (type << m_pin);
		m_base->OSPEEDR = (m_base->PUPDR & (~(3ul << (m_pin * 2)))) | (speed << (m_pin * 2));
		m_base->MODER = (m_base->MODER & (~(3ul << (m_pin * 2)))) | (mode << (m_pin * 2));
		m_base->PUPDR = (m_base->PUPDR & (~(3ul << (m_pin * 2)))) | (pull << (m_pin * 2));		
	}
	
	void DDRIn()
	{
		m_base->MODER = (m_base->MODER & (~(3ul << (m_pin * 2)))) | (PIN_MODE_INPUT << (m_pin * 2));
	}
	
	void DDROut()
	{
		m_base->MODER = (m_base->MODER & (~(3ul << (m_pin * 2)))) | (PIN_MODE_OUTPUT << (m_pin * 2));
	}
	
	void Input()
	{
		SetPinConfig(0, PIN_MODE_INPUT, PIN_TYPE_PUSHPULL, PIN_PULL_UP, PIN_SPEED_MEDIUM);
	}
	
	void Analog()
	{
		SetPinConfig(0, PIN_MODE_ANALOG, PIN_TYPE_PUSHPULL, PIN_PULL_FLOAT, PIN_SPEED_MEDIUM);
	}
	
	void Output()
	{
		SetPinConfig(0, PIN_MODE_OUTPUT, PIN_TYPE_PUSHPULL, PIN_PULL_FLOAT, PIN_SPEED_MEDIUM);
	}
	
	void Opendrain()
	{
		SetPinConfig(0, PIN_MODE_OUTPUT, PIN_TYPE_OPENDRAIN, PIN_PULL_FLOAT, PIN_SPEED_MEDIUM);
	}
	
	void Alternate(int func)
	{
		SetPinConfig(func, PIN_MODE_AF, PIN_TYPE_PUSHPULL, PIN_PULL_UP, PIN_SPEED_MEDIUM);
	}
	
	void AlternateFast(int func)
	{
		SetPinConfig(func, PIN_MODE_AF, PIN_TYPE_PUSHPULL, PIN_PULL_UP, PIN_SPEED_VERYHIGH);
	}
	
	void high()
	{
		m_base->BSRR = m_mask;
	}

	void low()
	{
		m_base->BSRR = m_resetmask;
	}
	
	void set(int value)
	{
		if (value)
			high();
		else
			low();
	}
	
	bool get()
	{
		return (m_base->IDR & m_mask) != 0;
	}
	
	CPin & operator =(int value)
	{
		if (value)
			m_base->BSRR = m_mask;
		else
			m_base->BSRR = m_resetmask;
		return *this;
	}
	
	operator int()
	{
		return (m_base->IDR & m_mask) != 0;
	}
};

extern CPin a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15;
extern CPin b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12, b13, b14, b15;
extern CPin c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15;
extern CPin d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12, d13, d14, d15;
extern CPin e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13, e14, e15;
extern CPin f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15;
extern CPin g0, g1, g2, g3, g4, g5, g6, g7, g8, g9, g10, g11, g12, g13, g14, g15;
extern CPin h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11, h12, h13, h14, h15;
extern CPin i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15;

void DisableIRQ(IRQn_Type irq);
void EnableIRQ(IRQn_Type irq);

class CSerialQueue {
private:
public:
	unsigned char m_data[32];
	int m_readp;
	int m_writep;
	int m_count;
	IRQn_Type m_irq;

	CSerialQueue()
	{
		m_readp = 0;
		m_writep = 0;
		m_count = 0;
		m_irq = (IRQn_Type)0;
	}

	void SetIRQ(IRQn_Type irq)
	{
		m_irq = irq;
	}
	
	void put(int data)
	{
		while (m_count >= sizeof(m_data))
			Sleep(1);
		DisableIRQ(m_irq);
		m_data[m_writep++] = data;
		m_writep %= sizeof(m_data);
		m_count++;
		EnableIRQ(m_irq);
	}
	
	void irqput(int data)
	{
		while (m_count >= sizeof(m_data))
		{
			m_readp++;
			m_readp %= sizeof(m_data);
			m_count--;
		}
		m_data[m_writep++] = data;
		m_writep %= sizeof(m_data);
		m_count++;
	}
	
	int get()
	{
		unsigned char res;
		if (m_count == 0)
			return -1;		
		DisableIRQ(m_irq);
		res = m_data[m_readp++];
		m_readp %= sizeof(m_data);
		m_count--;
		EnableIRQ(m_irq);
		return res;
	}
	
	int peek()
	{
		return m_data[m_readp];
	}
	
	void flush()
	{
		DisableIRQ(m_irq);
		m_count = 0;
		m_readp = 0;
		m_writep = 0;
		EnableIRQ(m_irq);
	}
	
	int count()
	{
		return m_count;
	}
};

class CUSART {
private:
	volatile USART_TypeDef *m_base;
	CSerialQueue m_in;
	CPin *m_rs485pin;
	unsigned long m_timeout;
	int m_index;
	unsigned int m_datamask;
	IRQn_Type m_irq;

	void sendbyte(unsigned char data)
	{
		volatile int w;
		if (m_rs485pin != 0)
			m_rs485pin->high();
		m_base->TDR = data;
		while (!(m_base->ISR & USART_ISR_TC))
			Sleep(0);
		if (m_rs485pin != 0)
			m_rs485pin->low();
	}
	
public:
	CUSART(int index, USART_TypeDef *base)
	{
		m_index = index;
		m_base = base;
		m_rs485pin = 0;
		m_timeout = 0xFFFFFFFFul;
		
		switch (m_index)
		{
			case 1:
				m_irq = USART1_IRQn;
				break;
			case 2:
				m_irq = USART2_IRQn;
				break;
			case 3:
				m_irq = USART3_IRQn;
				break;
			case 4:
				m_irq = UART4_IRQn;
				break;
			case 5:
				m_irq = UART5_IRQn;
				break;
			case 6:
				m_irq = USART6_IRQn;
				break;
			case 7:
				m_irq = UART7_IRQn;
				break;
			case 8:
				m_irq = UART8_IRQn;
				break;
		}
		m_in.SetIRQ(m_irq);
	}

	void IRQ()
	{
		unsigned char data;
		if (m_base->ISR & 0x3F)
		{
			// RXNE
			data = m_base->RDR;
			m_in.irqput(data);
		}
	}

	void setRS485DE(CPin &pin)
	{
		m_rs485pin = &pin;
	}
	
	void DisableIRQ()
	{
		::DisableIRQ(m_irq);
	}
	
	void EnableIRQ()
	{
		::EnableIRQ(m_irq);
	}
	
	void begin(unsigned long speed, unsigned char config = SERIAL_8N1)
	{
		unsigned long clk = APB1_CLK;
		unsigned char parity = config & PARITYMASK;
		unsigned char stopbits = config & STOPBITSMASK;
		unsigned char bytesize = config & 0x0F;

		end();
		
		m_datamask = 0xFF;
		if (bytesize < 8)
			m_datamask = 0x7F;
		
		switch (m_index)
		{
			case 1:
				RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
				a9.SetPinConfig(7, PIN_MODE_AF, PIN_TYPE_PUSHPULL, PIN_PULL_UP, PIN_SPEED_MEDIUM);
				a10.SetPinConfig(7, PIN_MODE_AF, PIN_TYPE_PUSHPULL, PIN_PULL_UP, PIN_SPEED_MEDIUM);
				break;
			case 2:
				RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
				a2.SetPinConfig(7, PIN_MODE_AF, PIN_TYPE_PUSHPULL, PIN_PULL_UP, PIN_SPEED_MEDIUM);
				a3.SetPinConfig(7, PIN_MODE_AF, PIN_TYPE_PUSHPULL, PIN_PULL_UP, PIN_SPEED_MEDIUM);
				break;
			case 3:
				RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
				c11.SetPinConfig(7, PIN_MODE_AF, PIN_TYPE_PUSHPULL, PIN_PULL_UP, PIN_SPEED_MEDIUM);
				c10.SetPinConfig(7, PIN_MODE_AF, PIN_TYPE_PUSHPULL, PIN_PULL_UP, PIN_SPEED_MEDIUM);
				break;
			case 4:
				RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
				a0.SetPinConfig(8, PIN_MODE_AF, PIN_TYPE_PUSHPULL, PIN_PULL_UP, PIN_SPEED_MEDIUM);
				a1.SetPinConfig(8, PIN_MODE_AF, PIN_TYPE_PUSHPULL, PIN_PULL_UP, PIN_SPEED_MEDIUM);
				break;
			case 5:
				RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
				d2.SetPinConfig(8, PIN_MODE_AF, PIN_TYPE_PUSHPULL, PIN_PULL_UP, PIN_SPEED_MEDIUM);
				c12.SetPinConfig(8, PIN_MODE_AF, PIN_TYPE_PUSHPULL, PIN_PULL_UP, PIN_SPEED_MEDIUM);
				break;
			case 6:
				RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
				c7.SetPinConfig(8, PIN_MODE_AF, PIN_TYPE_PUSHPULL, PIN_PULL_UP, PIN_SPEED_MEDIUM);
				c6.SetPinConfig(8, PIN_MODE_AF, PIN_TYPE_PUSHPULL, PIN_PULL_UP, PIN_SPEED_MEDIUM);
				break;
		}
		
		m_base->CR1 = 0;
		m_base->CR2 = 0;
		m_base->CR3 = 0;
		
		m_base->BRR = clk / speed;

		switch (parity)
		{
			case EVENPARITY:
				m_base->CR1 |= (1 << 10);
				if (bytesize == 8)
					m_base->CR1 |= (1 << 12);
				break;
			case ODDPARITY:
				m_base->CR1 |= (1 << 10) | (1 << 9);
				if (bytesize == 8)
					m_base->CR1 |= (1 << 12);
				break;
		}
		
		switch (stopbits)
		{
			case TWOSTOPBITS:
				m_base->CR2 |= (2 << 12);
				break;
		}
		
		m_base->CR1 |= (1 << 5);
		m_base->CR1 |= (1 << 3) | (1 << 2);
		m_base->CR1 |= (1 << 0);
		
		m_base->TDR = '>';

		EnableIRQ();
	}
	
	void end()
	{
		DisableIRQ();
		
		m_base->CR1 = 0;

		switch (m_index)
		{
			case 1:
				RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN;
				break;
			case 2:
				RCC->APB1ENR &= ~RCC_APB1ENR_USART2EN;
				break;
			case 3:
				RCC->APB1ENR &= ~RCC_APB1ENR_USART3EN;
				break;
			case 4:
				RCC->APB1ENR &= ~RCC_APB1ENR_UART4EN;
				break;
			case 5:
				RCC->APB1ENR &= ~RCC_APB1ENR_UART5EN;
				break;
			case 6:
				RCC->APB2ENR &= ~RCC_APB2ENR_USART6EN;
				break;
		}
	}
	
	void write(unsigned char data)
	{
		sendbyte(data);
	}
	
	void print(const char *s)
	{
		while (*s)
		{
			if (*s == '\n')
				sendbyte('\r');
			sendbyte(*s++);
		}
	}

	void print(int n)
	{
		char buf[12];
		sprintf(buf, "%d", n);
		print(buf);
	}

	void print(unsigned int n)
	{
		char buf[12];
		sprintf(buf, "%u", n);
		print(buf);
	}

	void print(long n)
	{
		char buf[12];
		sprintf(buf, "%ld", n);
		print(buf);
	}

	void print(unsigned long n)
	{
		char buf[12];
		sprintf(buf, "%lu", n);
		print(buf);
	}

	void print(char n)
	{
		write(n);
	}

	int available()
	{
		return m_in.count();
	}

	void setTimeout(unsigned long timeout)
	{
		m_timeout = timeout;
	}

	int read()
	{
		unsigned long t = time + m_timeout;
		while (m_in.count() == 0)
		{
			if (t <= time)
				return -1;
			Sleep(1);
		}
		return m_in.get();
	}

	int readBytes(void *buffer, int count)
	{
		unsigned char *p = (unsigned char *)buffer;
		int data;
		int res = 0;
		while (count --> 0)
		{
			data = read();
			if (data == -1)
				break;
			*p++ = data;
		}
		return res;
	}

	int readBytesUntil(unsigned char terminator, void *buffer, int count)
	{
		unsigned char *p = (unsigned char *)buffer;
		int data;
		int res = 0;
		while (count --> 0)
		{
			data = read();
			if (data == -1)
				break;
			if (data == terminator)
				break;
			*p++ = data;
		}
		return res;
	}
};

extern CUSART &Serial;
extern CUSART Serial1;
extern CUSART Serial2;
extern CUSART Serial3;
extern CUSART Serial4;
extern CUSART Serial5;
extern CUSART Serial6;

class CGPTimer
{
private:
	TIM_TypeDef *m_base;
	int m_index;
	void (*m_func)();
public:
	CGPTimer(int index, TIM_TypeDef *base)
	{
		m_base = base;
		m_index = index;
		m_func = 0;
	}
	
	void Init(unsigned int freq)
	{
		switch (m_index)
		{
			case 2:
				RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
				break;
			case 5:
				RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
				break;
		}
		
		unsigned int v = APB1_CLK / freq * 2;
		unsigned int psc = 1;
		
		while (v > 65535)
		{
			v /= 2;
			psc *= 2;
		}
		
		m_base->PSC =  psc - 1;
		m_base->ARR = v;
		m_base->DIER = 0x01;
		m_base->CR1 = 0x01;

		switch (m_index)
		{
			case 2:
				EnableIRQ(TIM2_IRQn);
				break;
			case 5:
				EnableIRQ(TIM5_IRQn);
				break;
		}
	}
	
	void SetFunc(void (*f)())
	{
		m_func = f;
	}
	
	void IRQ()
	{
		if (m_func != 0)
			m_func();
	}
};

extern CGPTimer Timer2;
extern CGPTimer Timer5;

class CSPI
{
private:
	SPI_TypeDef *m_spi;
	int m_index;
	CPin *m_cs;
public:
	CSPI(int index, SPI_TypeDef *spi, CPin *cs)
	{
		m_spi = spi;
		m_index = index;
		m_cs = cs;
		
		cs->Output();
		cs->high();
	}
	
	void Open(int speed)
	{
		switch (m_index)
		{
			case 5:
				f7.SetPinConfig(5, PIN_MODE_AF, PIN_TYPE_PUSHPULL, PIN_PULL_UP, PIN_SPEED_HIGH);
				f8.SetPinConfig(5, PIN_MODE_AF, PIN_TYPE_PUSHPULL, PIN_PULL_UP, PIN_SPEED_HIGH);
				f9.SetPinConfig(5, PIN_MODE_AF, PIN_TYPE_PUSHPULL, PIN_PULL_UP, PIN_SPEED_HIGH);
				RCC->APB2ENR |= RCC_APB2ENR_SPI5EN;
				break;
		}
		
		m_spi->CR2 = (7 << 8);
		
		m_spi->CR1 = (0 << 11) | (1 << 9) | (1 << 8) | (0 << 7) | (0 << 6) | ((speed & 7) << 3) | (1 << 2) | (0 & 3);
		m_spi->CR1 = (0 << 11) | (1 << 9) | (1 << 8) | (0 << 7) | (1 << 6) | ((speed & 7) << 3) | (1 << 2) | (0 & 3);
		
		Transmit(0xFF);
	}
	
	void Select()
	{
		m_cs->low();
	}
	
	void Deselect()
	{
		m_cs->high();
	}
	
	int Transmit(int data)
	{
		m_spi->DR = data;
		while ((m_spi->SR & 0x01) == 0);
		return m_spi->DR;
	}
	
	int Transmit(const void *data, int count)
	{
		const unsigned char *d = (const unsigned char *)data;
		int i, res;
		for (i = 0; i < count; i++)
			res = Transmit(d[i]);
		return res;
	}

	void Receive(void *buffer, int count)
	{
		unsigned char *d = (unsigned char *)buffer;
		int i;
		for (i = 0; i < count; i++)
			d[i] = Transmit(0xFF);
	}
};

extern CSPI spi5;

class SDRAM
{
private:
	void cmd(unsigned int mrd, unsigned int nrfs, unsigned int mode)
	{
		FMC_Bank5_6->SDCMR = (mrd << 9ul) | (nrfs << 5ul) | (3 << 3ul) | (mode << 0ul);
		
		while (FMC_Bank5_6->SDSR & 0x20);
	}
public:
	void Init()
	{
		RCC->AHB3ENR |= RCC_AHB3ENR_FMCEN;
		
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOIEN;
		
		// A0 - A12
		f0.AlternateFast(12);
		f1.AlternateFast(12);
		f2.AlternateFast(12);
		f3.AlternateFast(12);
		f4.AlternateFast(12);
		f5.AlternateFast(12);
		f12.AlternateFast(12);
		f13.AlternateFast(12);
		f14.AlternateFast(12);
		f15.AlternateFast(12);
		g0.AlternateFast(12);
		g1.AlternateFast(12);
		g2.AlternateFast(12);
		
		// D0 - D15
		d14.AlternateFast(12);
		d15.AlternateFast(12);
		d0.AlternateFast(12);
		d1.AlternateFast(12);
		e7.AlternateFast(12);
		e8.AlternateFast(12);
		e9.AlternateFast(12);
		e10.AlternateFast(12);
		e11.AlternateFast(12);
		e12.AlternateFast(12);
		e13.AlternateFast(12);
		e14.AlternateFast(12);
		e15.AlternateFast(12);
		d8.AlternateFast(12);
		d9.AlternateFast(12);
		d10.AlternateFast(12);
		
		// BA1
		g5.AlternateFast(12);
		
		// BA0
		g4.AlternateFast(12);
		
		// NE1
		h6.AlternateFast(12);
		
		// NWE
		h5.AlternateFast(12);
		
		// NRAS
		f11.AlternateFast(12);
		
		// NCAS
		g15.AlternateFast(12);
		
		// CLK
		g8.AlternateFast(12);
		
		// CKE1
		h7.AlternateFast(12);
		
		// NBL1
		e1.AlternateFast(12);
		
		// NBL0
		e0.AlternateFast(12);

		FMC_Bank5_6->SDCR[0] =
			(1 << 13ul) |	// Read pipe 1 clk
			(2 << 10ul) |	// 2 HCLK
			(3 << 7ul) |	// CAS 3
			(1 << 6ul) |	// 4 banks
			(1 << 4ul) |	// 16 bit
			(2 << 2ul) |	// 13 row bits
			(1 << 0ul);		// 9 col bits

		FMC_Bank5_6->SDCR[1] =
			(0 << 13ul) |	// Read pipe 1 clk
			(1 << 12ul) |	// Read burst
			(1 << 10ul) |	// 2 HCLK
			(2 << 7ul) |	// CAS 3
			(1 << 6ul) |	// 4 banks
			(1 << 4ul) |	// 16 bit
			(2 << 2ul) |	// 13 row bits
			(1 << 0ul);		// 9 col bits
		
		FMC_Bank5_6->SDTR[1] =
			(1 << 24ul) |	// RCD 2
			(1 << 20ul) |	// RP 2
			(1 << 16ul) |	// TWR 2
			(7 << 12ul) |	// RCD 6
			(3 << 8ul) |	// SRT 4
			(6 << 4ul) |	// ESRD 7
			(1 << 0ul);		// TMRD 2

		cmd(0, 1, 1);
		Sleep(2);
		
		cmd(0, 1, 2);
		Sleep(2);
		
		cmd(0, 8, 3);
		Sleep(2);
		
		cmd(
			(1 << 9ul) |	// Write birst single
			(2 << 4ul),		// CAS 3
			1, 4);
			
		FMC_Bank5_6->SDRTR = ((600 - 20) << 1ul) | 1ul;
		Sleep(2);
	}
};

class CLTDC
{
public:
	void SetPalette(unsigned char index, unsigned char r, unsigned char g, unsigned char b)
	{
		LTDC_Layer1->CLUTWR = (index << 24ul) | (r << 16ul) | (g << 8ul) | b;
		LTDC->SRCR |= LTDC_SRCR_VBR;
	}

	void Init()
	{
		RCC->APB2ENR |= RCC_APB2ENR_LTDCEN;
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOIEN;
	

		h12.AlternateFast(14);
		g6.AlternateFast(14);

		i1.AlternateFast(14);
		i2.AlternateFast(14);
		
		i6.AlternateFast(14);
		i7.AlternateFast(14);
		
		i10.AlternateFast(14);	// HSYNC
		i9.AlternateFast(14);	// VSYNC
		
		LTDC->SSCR = ((H_PW - 1) << 16ul) + (V_PW - 1);
		LTDC->BPCR = ((H_PW + H_BP - 1) << 16ul) + (V_PW + V_BP - 1);
		LTDC->AWCR = ((H_PW + H_BP + H_VD - 1) << 16ul) + (V_PW + V_BP + V_VD - 1);
		LTDC->TWCR = ((H_PW + H_BP + H_VD + H_FP - 1) << 16ul) + (V_PW + V_BP + V_VD + V_FP - 1);

		int i, r, g, b;

		SetPalette(0, 0, 0, 0);
		SetPalette(1, 0, 0, 128);
		SetPalette(2, 0, 96, 0);
		SetPalette(3, 0, 160, 96);
		SetPalette(4, 128, 0, 0);
		SetPalette(5, 128, 0, 160);
		SetPalette(6, 160, 96, 0);
		SetPalette(7, 160, 160, 160);
		SetPalette(8, 80, 80, 80);
		SetPalette(9, 0, 0, 255);
		SetPalette(10, 0, 255, 0);
		SetPalette(11, 0, 255, 255);
		SetPalette(12, 255, 0, 0);
		SetPalette(13, 255, 0, 255);
		SetPalette(14, 255, 255, 0);
		SetPalette(15, 255, 255, 255);

		for (i=16,r=0;r<6;r++)
			for (g=0;g<6;g++)
				for (b=0;b<6;b++)
					SetPalette(i++, r * 51, g * 51, b * 51);

		for (b=0;b<32;b++)
			SetPalette(224 + b, b * 8, b * 8, b * 8);

		
		/*
		
		// rrrgggbb palette
		
		for (i=0,r=0;r<8;r++)
			for (g=0;g<8;g++)
				for (b=0;b<4;b++)
					SetPalette(i++, r * 36, g * 36, b * 85);
		*/
		
		LTDC_Layer1->DCCR = 0xFF;
		LTDC_Layer1->WHPCR = (H_PW + H_BP) | ((H_PW + H_BP + H_VD - 1) << 16ul);
		LTDC_Layer1->WVPCR = (V_PW + V_BP) | ((V_PW + V_BP + V_VD - 1) << 16ul);
		LTDC_Layer1->PFCR = 5;
		LTDC_Layer1->CFBAR = 0x71800000u;
		LTDC_Layer1->CFBLR = (H_VD << 16ul) | (H_VD + 3);
		LTDC_Layer1->CFBLNR = V_VD + 20;
		LTDC_Layer1->CR |= 0x10 | 0x01;
		
		LTDC_Layer2->CR = 0;
		
		LTDC->SRCR |= 1;

		LTDC->GCR = (HSYNC_POL << 31ul) | (VSYNC_POL << 30ul) | (DEN_POL << 29ul) | (DCLK_POL << 28ul) | 0x2221;
		
		LTDC->IER = 0;
	}
};

#endif
