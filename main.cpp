#include "stdafx.h"
#include "config.h"
#include "main.h"
#include "cpu.h"
#include "vga.h"
#include "disk.h"
#include "ioports.h"
#include "pic_pit.h"
#include "keybmouse.h"

HINSTANCE hInst;
HWND hWnd;

int terminated = 0;

int ncycles = 2000;

extern int cyc;
unsigned long cyctime = 0;

unsigned char sys_ram[RAM_SIZE];
unsigned char *ram = sys_ram;

// Frame buffer for render_screen
unsigned int scr[SCREEN_WIDTH * SCREEN_HEIGHT];

// Video RAM. Should be at least 512KB for 640x480x256 mode
unsigned int video_ram[1024 * 1024];
unsigned int *vram = video_ram;

FILE *fdd[NUM_FDD] = {NULL};
FILE *hdd[NUM_HDD] = {NULL};

COLORREF hw_palette[256] = {0};

// Hardware set palette function. Not used on PC
void hw_set_palette(unsigned char index, unsigned char r, unsigned char g, unsigned char b)
{
	hw_palette[index] = RGB(b, g, r);
}

void hw_read_floppy(int disk, unsigned char *buffer, unsigned int lba, unsigned int count)
{
	if ((disk < 0) || (disk >= NUM_FDD))
		return;
	fseek(fdd[disk], lba * 512, SEEK_SET);
	fread(buffer, 512, count, fdd[disk]);
}

void hw_write_floppy(int disk, const unsigned char *buffer, unsigned int lba, unsigned int count)
{
	
}

void hw_read_hdd(int disk, unsigned char *buffer, unsigned int lba, unsigned int count)
{
	if ((disk < 0) || (disk >= NUM_HDD))
		return;
	fseek(hdd[disk], lba * 512, SEEK_SET);
	fread(buffer, 512, count, hdd[disk]);
}

void hw_write_hdd(int disk, const unsigned char *buffer, unsigned int lba, unsigned int count)
{
	if ((disk < 0) || (disk >= NUM_HDD))
		return;
	fseek(hdd[disk], lba * 512, SEEK_SET);
	fwrite(buffer, 512, count, hdd[disk]);
}

void set_pixel_2x2(int x, int y, unsigned int color)
{
	unsigned int *v;
	color = hw_palette[color & 0xFF];
	y = SCREEN_HEIGHT / 2 - y - 1;
	v = &scr[x * 2 + y * 2 * SCREEN_WIDTH];
	v[0] = color;
	v[1] = color;
	v[SCREEN_WIDTH] = color;
	v[SCREEN_WIDTH + 1] = color;
}

void set_pixel_2x1(int x, int y, unsigned int color)
{
	unsigned int *v;
	color = hw_palette[color & 0xFF];
	y = SCREEN_HEIGHT - y - 1;
	v = &scr[x * 2 + y * SCREEN_WIDTH];
	v[0] = color;
	v[1] = color;
}

void set_pixel_1x2(int x, int y, unsigned int color)
{
	unsigned int *v;
	color = hw_palette[color & 0xFF];
	y = SCREEN_HEIGHT / 2 - y - 1;
	v = &scr[x + y * 2 * SCREEN_WIDTH];
	v[0] = color;
	v[SCREEN_WIDTH] = color;
}

void set_pixel(int x, int y, unsigned int color)
{
	unsigned int *v;
	color = hw_palette[color & 0xFF];
	y = SCREEN_HEIGHT - y - 1;
	v = &scr[x + y * SCREEN_WIDTH];
	v[0] = color;
}

void shutdown()
{
	if (dasm == NULL)
		fopen_s(&dasm, "386.dasm", "wt");
	D("\tundefined %.2X\n", opcode);
	terminated = 1;
	PostMessage(hWnd, WM_CLOSE, 0, 0);
}

void render_screen(HDC hdc)
{
	BITMAPINFO bmi;
	ZeroMemory(&bmi, sizeof(bmi));

	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = SCREEN_WIDTH;
	bmi.bmiHeader.biHeight = SCREEN_HEIGHT;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = 0;
	bmi.bmiHeader.biXPelsPerMeter = 96;
	bmi.bmiHeader.biYPelsPerMeter = 96;
	bmi.bmiHeader.biClrImportant = 0;
	bmi.bmiHeader.biClrUsed = 0;

	SetDIBitsToDevice(hdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, SCREEN_HEIGHT, scr, &bmi, DIB_RGB_COLORS);
}


void loop()
{
	int i;
	FILE *f;

	// Loading "bios.bin" (size = 8 KB) to 0xF0000 and 0xFE000
	fopen_s(&f, "bios.bin", "rb");
	fread(&ram[0xF0000], 8192, 1, f);
	fseek(f, 0, SEEK_SET);
	fread(&ram[0xFE000], 8192, 1, f);
	fclose(f);

	fopen_s(&f, "rombasic.bin", "rb");
	if (f != NULL)
	{
		// ROM Basic found -> load it to 0xF6000
		fread(&ram[0xF6000], 32768, 1, f);
		fclose(f);
	}

	fopen_s(&f, "videorom.bin", "rb");
	if (f != NULL)
	{
		// Video ROM found -> load it to 0xC0000
		fread(&ram[0xC0000], 32768, 1, f);
		fclose(f);
	}

	// Debug output
	fopen_s(&c0, DEBUG_FILE_NAME, "wt");

	// fopen_s(&dasm, "386.dasm", "wt");

	// Reset the CPU
	reset();

	// Disk setup
	disk_init();

	fdd[0] = fopen("i:\\linuxfd.img", "rb+");
	disk_set_fdd(0, 80, 2, 18);

	
	hdd[0] = fopen("c:\\hd500-3.img", "rb+");
	disk_set_hdd(0, 1023, 16, 63);
	
	
	/*
	hdd[0] = fopen("c:\\hd10meg.img", "rb+");
	disk_set_hdd(0, 306, 4, 17);

	hdd[1] = fopen("c:\\hd10meg.img", "rb+");
	disk_set_hdd(1, 306, 4, 17);
	*/

	/*
	hdd[0] = fopen("c:\\hd_oldlinux.img", "rb+");
	disk_set_hdd(0, 1023, 4, 20);
	hdd[1] = fopen("c:\\hd_oldlinux.img", "rb+");
	disk_set_hdd(1, 1023, 4, 20);
	*/

	// Main emulator loop
	while (!terminated)
	{
		for (i = 0; (i < ncycles) && (!terminated); i++)
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
			step();

			check_irqs();

			ports[0x3da] ^= 1;

			pit_step();

			if ((i & 0x3F) == 0)
			{
				check_mouse();
			}
		}

		ports[0x3da] ^= 8;

		check_keyb();

		if (ports[0x3da] & 8)
		{
			InvalidateRect(hWnd, NULL, false);
			// Sleep(5);
		}
	}

	disk_deinit();

	for (i = 0; i < NUM_FDD; i++)
	{
		if (fdd[i] != NULL)
		{
			fclose(fdd[i]);
		}
	}

	for (i = 0; i < NUM_HDD; i++)
	{
		if (hdd[i] != NULL)
		{
			fclose(hdd[i]);
		}
	}

	if (c0 != NULL)
		fclose(c0);
	c0 = NULL;

	if (dasm != NULL)
		fclose(dasm);
	dasm = NULL;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int mouse_buttons = 0;
	PAINTSTRUCT ps;
	HDC hdc;
	unsigned long t;
	static int ncyc = 0, acyc = 0;

#if (SET_WINDOW_CLIENT_SIZE == 0)
	char s[2000];
	RECT r1;
#endif

	switch (message)
	{
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);

			update_screen();

			render_screen(hdc);
			
			ncyc += cyc;

			t = GetTickCount();
			if (t - cyctime > 5000)
			{
				acyc = ncyc * 5ll / (t - cyctime);
				ncyc = 0;
				cyc = 0;
				cyctime = t;
			}

#if (SET_WINDOW_CLIENT_SIZE == 0)
			sprintf_s(s, sizeof(s) - 1, "%d\nd = %d\n\ncs:ip = %.4X:%.8X\nss:sp = %.4X:%.8X\nds = %.4X\nes = %.4X\nfs = %.4X\ngs = %.4X\n\n"
				"ax = %.8X\ncx = %.8X\ndx = %.8X\nbx = %.8X\nbp = %.8X\nsi = %.8X\ndi = %.8X\nfl = %.8X\n\n"
				"cr0 = %.8X\ncr3 = %.8X\n\ngdtr:\n  %.8X / %.4X\nldtr: (%.4X)\n  %.8X / %.4X\ntss: (%.4X)\n  %.8X / %.4X\n\n"
				"pf: %d\ngp: %d\nex: %d\nmath: %d\n",
				acyc, ncycles, cs.value, r.eip, ss.value, r.esp, ds.value, es.value, fs.value, gs.value,
				r.eax, r.ecx, r.edx, r.ebx, r.ebp, r.esi, r.edi, r.eflags,
				cr[0], cr[3], gdt_base, gdt_limit, ldtr, ldt_base, ldt_limit, tss, tssbase, tsslimit, num_pf, num_gp, num_ex, num_math);
			r1.left = 642;
			r1.top = 2;
			r1.right = 800;
			r1.bottom = 600;
			FillRect(hdc, &r1, (HBRUSH)(COLOR_WINDOW+1));
			DrawTextA(hdc, s, strlen(s), &r1, DT_LEFT | DT_TOP);
#endif

			EndPaint(hWnd, &ps);
			break;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			if (wParam == VK_F12)
			{
				if (dasm == NULL)
					fopen_s(&dasm, DISASM_FILE_NAME, "wt");
			}
			else if (wParam == VK_PRIOR)
			{
				ncycles -= 200;
				if (ncycles < 200)
					ncycles = 200;
			}
			else if (wParam == VK_NEXT)
			{
				ncycles += 200;
			}
			else
				keydown(wParam);
			return 1;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			keyup(wParam);
			break;
		case WM_LBUTTONDOWN:
			mouse_buttons |= 1;
			mousereport(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), mouse_buttons);
			break;
		case WM_RBUTTONDOWN:
			mouse_buttons |= 2;
			mousereport(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), mouse_buttons);
			break;
		case WM_LBUTTONUP:
			mouse_buttons &= ~1;
			mousereport(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), mouse_buttons);
			break;
		case WM_RBUTTONUP:
			mouse_buttons &= ~2;
			mousereport(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), mouse_buttons);
			break;
		case WM_MOUSEMOVE:
			mousereport(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), mouse_buttons);
			break;
		case WM_DESTROY:
			terminated = 1;
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	memset(&wcex, 0, sizeof(wcex));

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.hInstance		= hInstance;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszClassName	= L"e86r";

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   RECT r;
   r.left = 0;
   r.top = 0;
   r.right = 900;
   r.bottom = 600;

#if (SET_WINDOW_CLIENT_SIZE == 1)
   r.right = 640;
   r.bottom = 480;
   AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, FALSE);
#endif

   hWnd = CreateWindow(L"e86r", L"e86r", WS_OVERLAPPEDWINDOW,
	   CW_USEDEFAULT, 0, r.right, r.bottom, NULL, NULL, hInstance, NULL);
   
   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;

	MyRegisterClass(hInstance);

	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	thread th1(loop);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	terminated = 1;
	th1.join();

	return (int) msg.wParam;
}

