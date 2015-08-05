// Escape1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"

volatile int a = 0;

void escape(void)
{
	printf("Escaped!\n");
	_exit(1);
}

__declspec(noinline) void trap(void)
{
	_asm {
		lea eax, dword ptr [a]
		lea eax, dword ptr [a]
	}

	printf("Hello world!\n");
}

int _tmain(int argc, _TCHAR* argv[])
{
	trap();

	MEMORY_BASIC_INFORMATION mbi;
	void *addr = 0;

	unsigned char pattern[] = {
		0x8d,
		0x5,
		((uintptr_t) &a >> 0) & 0xff,
		((uintptr_t) &a >> 8) & 0xff,
		((uintptr_t) &a >> 16) & 0xff,
		((uintptr_t) &a >> 24) & 0xff,
		0x8d,
		0x5,
		((uintptr_t) &a >> 0) & 0xff,
		((uintptr_t) &a >> 8) & 0xff,
		((uintptr_t) &a >> 16) & 0xff,
		((uintptr_t) &a >> 24) & 0xff
	};

	while (VirtualQuery(addr, &mbi, sizeof(mbi))) {
		if (mbi.State == MEM_COMMIT && mbi.Protect == PAGE_EXECUTE_READWRITE) {
			for (unsigned char *p = (unsigned char *) mbi.BaseAddress; p <= (unsigned char *) mbi.BaseAddress + mbi.RegionSize - sizeof(pattern); p++) {
				int j;

				for (j = 0; j < sizeof(pattern); j++) {
					if (p[j] != pattern[j])
						break;
				}
				if (j == sizeof(pattern)) {
					printf("Translation found at %p\n", p);

					p[0] = 0x68;
					p[1] = (((uintptr_t) &escape) >> 0) & 0xff;
					p[2] = (((uintptr_t) &escape) >> 8) & 0xff;
					p[3] = (((uintptr_t) &escape) >> 16) & 0xff;
					p[4] = (((uintptr_t) &escape) >> 24) & 0xff;
					p[5] = 0xc3;
				}
			}

		}
		addr = (char *) mbi.BaseAddress + mbi.RegionSize;
	}

	trap();

	return 0;
}

