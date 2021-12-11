#pragma once

#include "config.h"

#if (PC)

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <varargs.h>

#include <thread>
#include <mutex>

using namespace std;

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#if (STM32)

// #include <RTL.h>

#ifdef __cplusplus
class mutex
{
public:
	// OS_MUT m;

	mutex()
	{
		// os_mut_init(&m);
	}
	
	void lock()
	{
		// os_mut_wait(&m, 0xFFFFu);
	}
	
	void unlock()
	{
		// os_mut_release(&m);
	}
};
#endif

#endif