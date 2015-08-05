// LongInstruction2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"

int cnt = -1;

#define PAGE_SIZE 4096

int _tmain(int argc, _TCHAR* argv[])
{
	char *buffer = (char *) VirtualAlloc(NULL, PAGE_SIZE * 2, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	uintptr_t start_;
	uintptr_t end_;
	uintptr_t pb_;

	_asm {
		lea eax, start
		mov start_, eax
		lea eax, end
		mov end_, eax
		lea eax, pb
		mov pb_, eax
		jmp end
	}

	_asm {
start:
		jmp skip
out_:
		ret
skip:
		inc cnt
		jz out_
pb:
		rep	rep	rep	rep	rep	rep	rep	rep	rep	rep	rep	rep	rep	rep	rep	ret
end:
	}

	uintptr_t f = (uintptr_t) &buffer[PAGE_SIZE - (pb_ - start_)];
	memcpy((void *) f, (void *) start_, end_ - start_);
	uintptr_t newpb = (uintptr_t) &buffer[PAGE_SIZE];

	_try {
		_asm call f
	} _except(1) {
		return 1;
	}

	_try {
		_asm {
			mov eax, newpb
			mov [eax], 0xc3
			call f
		}
	} _except(1) {
		return 2;
	}

	return 0;
}

