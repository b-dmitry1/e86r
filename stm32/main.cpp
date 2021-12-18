#include <stdlib.h>
#include <string.h>
#include "stm32f7xx.h"                  // Device header
#include "board.h"
#include "cpu.h"
#include "vga.h"
#include "ioports.h"
#include "keybmouse.h"
#include "pic_pit.h"
#include "diskio.h"
#include "disk.h"

#define SCREEN_WIDTH		640
#define SCREEN_HEIGHT		480


unsigned char *ram = (unsigned char *)0x70000000u;
unsigned int *vram = (unsigned int *)0x71000000u;

unsigned char *scr = (unsigned char *)0x71800000u;


SDRAM sdram;

CLTDC ltdc;

CSerialQueue keyb_buf;

int diskledr = 0, diskledg = 0;

uint8_t mmc_mount(void);
void disk_timerproc(void);

void draw_debug_str(int x, int y, const char *s);

#include "bios.cpp"

void tty(unsigned char ch)
{
	if (ch > 0)
		Serial1.write(ch);
}

void show_regs()
{
	char s[50];
	sprintf(s, "eax = %08X", r.eax);
	draw_debug_str(0, 0, s);
	
	sprintf(s, "ebx = %08X", r.ebx);
	draw_debug_str(0, 1, s);
	sprintf(s, "ecx = %08X", r.ecx);
	draw_debug_str(0, 2, s);
	sprintf(s, "edx = %08X", r.edx);
	draw_debug_str(0, 3, s);
	
	sprintf(s, "esp = %08X", r.esp);
	draw_debug_str(20, 0, s);
	sprintf(s, "ebp = %08X", r.ebp);
	draw_debug_str(20, 1, s);
	sprintf(s, "esi = %08X", r.esi);
	draw_debug_str(20, 2, s);
	sprintf(s, "edi = %08X", r.edi);
	draw_debug_str(20, 3, s);

	sprintf(s, "es = %08X", es.value);
	draw_debug_str(40, 0, s);
	sprintf(s, "cs = %08X", cs.value);
	draw_debug_str(40, 1, s);
	sprintf(s, "ss = %08X", ss.value);
	draw_debug_str(40, 2, s);
	sprintf(s, "ds = %08X", ds.value);
	draw_debug_str(40, 3, s);

	sprintf(s, "eip = %08X", r.eip);
	draw_debug_str(60, 0, s);
	sprintf(s, "efl = %08X", r.eflags);
	draw_debug_str(60, 1, s);
	sprintf(s, "cr0 = %08X", cr[0]);
	draw_debug_str(60, 2, s);
	sprintf(s, "cr3 = %08X", cr[3]);
	draw_debug_str(60, 3, s);

	sprintf(s, "ins = %08X", instr_eip);
	draw_debug_str(0, 4, s);
}

void shutdown()
{
	Serial1.print("error");
	while (1)
	{
		update_screen();
		show_regs();
	}
}

void hw_set_palette(unsigned char index, unsigned char r, unsigned char g, unsigned char b)
{
	ltdc.SetPalette(index, r, g, b);
}

unsigned char disk_cache[64][512];
unsigned int disk_cache_lba[64];
int disk_cache_hit[64] = {0};

void hw_read_floppy(int disk, unsigned char *buffer, unsigned int lba, unsigned int count)
{
}

void hw_write_floppy(int disk, const unsigned char *buffer, unsigned int lba, unsigned int count)
{
}

void hw_read_hdd(int disk, unsigned char *buffer, unsigned int lba, unsigned int count)
{
	unsigned int i;
	
	for (i = 0; i < count; i++)
	{
		if (disk_cache_lba[lba & 63] == (lba & ~63))
		{
			memcpy(buffer, disk_cache[lba & 63], 512);
			if (disk_cache_hit[lba & 63] < 5)
				disk_cache_hit[lba & 63] += 3;
		}
		else
		{
			disk_read(0, buffer, lba, 1);
			if (disk_cache_hit[lba & 63] == 0)
			{
				memcpy(disk_cache[lba & 63], buffer, 512);
				disk_cache_lba[lba & 63] = lba & ~63;
				disk_cache_hit[lba & 63] = 1;
			}
			else
			{
				disk_cache_hit[lba & 63]--;
			}
		}
		buffer += 512;
		lba++;
	}
}

void hw_write_hdd(int disk, const unsigned char *buffer, unsigned int lba, unsigned int count)
{
}

void set_pixel_2x2(int x, int y, unsigned int color)
{
	unsigned char *v;
	v = &scr[x * 2 + y * 2 * SCREEN_WIDTH];
	v[0] = color;
	v[1] = color;
	v[SCREEN_WIDTH] = color;
	v[SCREEN_WIDTH + 1] = color;
}

void set_pixel_2x1(int x, int y, unsigned int color)
{
	unsigned char *v;
	v = &scr[x * 2 + y * SCREEN_WIDTH];
	v[0] = color;
	v[1] = color;
}

void set_pixel_1x2(int x, int y, unsigned int color)
{
	unsigned char *v;
	v = &scr[x + y * 2 * SCREEN_WIDTH];
	v[0] = color;
	v[SCREEN_WIDTH] = color;
}

void set_pixel(int x, int y, unsigned int color)
{
	unsigned char *v;
	v = &scr[x + y * SCREEN_WIDTH];
	v[0] = color;
}


extern "C" unsigned long get_fattime(void)
{
	return 0;
}

int getch1()
{
	if (keyb_buf.count() > 0)
		return keyb_buf.get();
	return -1;
}

int kbhit1()
{
	return keyb_buf.count() > 0;
}

void showerror(const char *, ...)
{

}

void timer2()
{
}

void CGADrawChar8x16(int x, int y, int ch, int attr);

void draw_debug_str(int x, int y, const char *s)
{
	int dx = x * 8, dy = (y + 25) * 16;
	while (*s)
	{
		CGADrawChar8x16(dx, dy, *s, 15);
		dx += 8;
		s++;
	}
}

void main_task()
{
	int i;

	memset(scr, 0, 640 * 480);
	
	// Manually load BIOS image from bios.cpp file
	memcpy(&ram[0xF0000], bios, sizeof(bios));
	memcpy(&ram[0xFE000], bios, sizeof(bios));
	
	// Init disk cache
	memset(disk_cache_lba, 0xFF, sizeof(disk_cache_lba));
	
	disk_init();
	
	disk_set_fdd(0, 80, 2, 18);

	disk_set_hdd(0, 1023, 16, 63);
	
	reset();

	while (1)
	{
		for (i = 0; i < 2000; i++)
		{
			step();
			step();
			step();

			step();
			step();
			step();
			step();

			step();
			step();
			step();
			step();

			step();
			step();
			step();
			step();

			step();
			step();
			step();
			step();
			step();

			check_irqs();

			// VGA hsync / vsync
			ports[0x3da] = (i >> 3) & 9;

			// Serial keyboard
			if (i % 200 == 199)
			{
				if (Serial1.available())
				{
					keydown(Serial1.read());
				
					check_keyb();
				}
			}

			// Update system timer
			pit_step();
		}

		disk_timerproc();
		
		if (vmode != 0x14)
		{
			// Redraw the screen for all video modes except SVGA
			update_screen();
		}
	}
}

int main()
{
	volatile int w;
	unsigned short *m;

	// Disable Systick timer
	SysTick->CTRL = 0;
	
	Serial1.begin(115200, SERIAL_8N1);
	
	Serial1.print("\nSerial port: OK\n");
	
	for (w = 0; w < 10000; w++);
	
	Serial1.print("\nMPU: ");

	// Allow unaligned access
	SCB->CCR = SCB->CCR & ~SCB_CCR_UNALIGN_TRP_Msk;

	// Setup MPU for a system RAM region
	MPU->RNR = 0;
	MPU->RBAR = 0x70000000u;
	MPU->RASR = (3 << 24) | (7 << 19) | (23 << 1) | 1 | MPU_RASR_C_Msk | MPU_RASR_B_Msk | MPU_RASR_S_Msk ;
	MPU->CTRL = 1 | 4;
	
	__DSB();
	__ISB();

	Serial1.print("OK\n");

	Serial1.print("SDRAM: ");
	sdram.Init();
	
	for (w = 0; w < 10000; w++);

	// Remap SDRAM from 0xD0000000 to 0x70000000
	RCC->APB2ENR   |= RCC_APB2ENR_SYSCFGEN;
	SYSCFG->MEMRMP |= SYSCFG_MEMRMP_SWP_FMC_0;

	Serial1.print("OK\n");

	Serial1.print("\nUnaligned access: ");

	// We need unaligned memory access enabled
	m = (unsigned short *)0x70000001u;
	
	// This may cause a hard fault / bus fault if the system isn't configured properly
	m[0] = 12345;
	
	Serial1.print("OK\n");

	Serial1.print("LTDC: ");

	ltdc.Init();

	Serial1.print("OK\n");

	scr = (unsigned char *)0x71800000u;

	Serial1.print("Memory card: ");

	mmc_mount();
	
	Serial1.print("OK\n");

	NVIC_DisableIRQ(LTDC_IRQn);

	Serial1.print("Starting\n");

	main_task();
	
	while (1);
}
