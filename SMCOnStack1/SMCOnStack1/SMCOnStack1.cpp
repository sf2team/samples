// SMCOnStack1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"

LONG WINAPI f(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
	return 1;
}

int _tmain(int argc, _TCHAR* argv[])
{
	SetErrorMode(SEM_NOGPFAULTERRORBOX);

	DWORD old;
	VirtualProtect((void *) _tmain, 8192, PAGE_EXECUTE_READWRITE, &old);
	_try {
		_asm {
			mov eax, esp
			lea esp, target
			push 0x90909090
			ud2
			ud2
target:
			mov esp, eax
		}
	}
	_except(1) {
		return 1;
	}
	return 0;
}

