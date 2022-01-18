
extern "C" void _exit(int code)
{
	for (;;);
}

extern "C" void _sbrk(int code)
{
}

extern "C" void _kill(int code)
{
}

extern "C" int _getpid(int code)
{
	return 0;
}
extern "C" void _write(int code)
{
}
extern "C" void _close(int code)
{
}
extern "C" void _fstat(int code)
{
}
extern "C" int _isatty(int code)
{
	return 0;
}
extern "C" int _read(int code)
{
	return 0;
}
extern "C" int _lseek(int code)
{
	return 0;
}
