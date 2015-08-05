// LongInstruction1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"

int cnt = -1;

#define PAGE_SIZE 4096

int _tmain(int argc, _TCHAR* argv[])
{
	char *buffer = (char *) VirtualAlloc(NULL, PAGE_SIZE, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	uintptr_t start_;
	uintptr_t end_;

	_asm {
		lea eax, start
		mov start_, eax
		lea eax, end
		mov end_, eax
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
		rep	rep	rep	rep	rep	rep	rep	rep	rep	rep	rep	rep	rep	rep	rep	ret
end:
	}

	uintptr_t f = (uintptr_t) buffer;
	memcpy((void *) f, (void *) start_, end_ - start_);
	uintptr_t inst = ((end_ - 5) - start_) + f;

	_try {
		_asm call f
	} _except(1) {
		return 1;
	}

	_try {
		_asm {
			mov eax, inst
			mov [eax], 0xc3
			call f
		}
	} _except(1) {
		return 2;
	}

	return 0;
}

