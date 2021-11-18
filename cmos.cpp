#include "stdafx.h"
#include "cmos.h"

// NVRAM / RTC

cmos_t cmos;

unsigned char cmos_image[64] = 
{
	0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x00, 0x26, 0x00, 0x00, 0x80, 0x00, 0x04, 
	0x00, 0x00, 0xF0, 0x00, 0x20, 0x80, 0x02, 0x00, 0x1C, 0x2F, 0x2F, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAA, 0xAA, 
	0x00, 0x1C, 0x19, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char cmos_index = 0;

int cmos_initialized = 0;

unsigned char bcd(unsigned char value)
{
	return (value / 10) * 16 + value % 10;
}

void update_rtc()
{
	SYSTEMTIME tm;
	unsigned char prev[10];

	memcpy(prev, &cmos, 10);

	GetLocalTime(&tm);

	cmos.second = bcd((unsigned char)tm.wSecond);
	cmos.min = bcd((unsigned char)tm.wMinute);
	cmos.hour = bcd((unsigned char)tm.wHour);
	cmos.dow = bcd((unsigned char)tm.wDayOfWeek);
	cmos.day = bcd((unsigned char)tm.wDay);
	cmos.month = bcd((unsigned char)tm.wMonth);
	cmos.yearmod100 = bcd((unsigned char)tm.wYear % 100);

	if (memcmp(prev, &cmos, 10))
		cmos.updateA = 1;
}

unsigned char cmos_read(int port)
{
	unsigned char v;
	if (!cmos_initialized)
	{
		memcpy(&cmos, cmos_image, sizeof(cmos));
		cmos_initialized = 1;
	}
	switch (port)
	{
		case 0x70:
			return cmos_index;
		case 0x71:
			if (cmos_index <= 0x0A)
				update_rtc();
			v = ((unsigned char *)&cmos)[cmos_index & 0x3F];
			// Clear update / int status
			if (cmos_index == 0x0A)
				cmos.updateA = 0;
			if (cmos_index == 0x0B)
				cmos.updateB = 0;
			if (cmos_index == 0x0C)
				cmos.int_status = 0;
			if (cmos_index == 0x0D)
				cmos.power_control = 0x80;
			return v;
	}
	return 0xFF;
}

void cmos_write(int port, unsigned char value)
{
	if (!cmos_initialized)
	{
		memcpy(&cmos, cmos_image, sizeof(cmos));
		cmos_initialized = 1;
	}
	switch (port)
	{
		case 0x70:
			cmos_index = value;
			break;
		case 0x71:
			((unsigned char *)&cmos)[cmos_index & 0x3F] = value;
			break;
	}
}
