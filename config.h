#pragma once

// Platform and compiler
#define PC						1
#define STM32					0


// System RAM size
#define RAM_SIZE				16777216u

// CPU type: 86 / 286 / 386 / 486
#define CPU						486

// Set to 1 to enable FPU detection
#define FPU						0


// Set to 1 to enable debugging
#define DEBUG					1

// Set to 1 to add memory access to disasm file
#define DEBUGMEM				1

#define DEBUG_FILE_NAME			"debug.txt"

// To activate disasm recording press F12 in the main window
#define DISASM_FILE_NAME		"debug.dasm"


// Disk drives
#define NUM_FDD					1
#define NUM_HDD					1


// Set to 1 if you don't want to see registers in the main window
#define SET_WINDOW_CLIENT_SIZE	0
