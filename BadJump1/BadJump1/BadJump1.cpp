// BadJump1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
	int a = -1;

	_try {
		_asm {
			lea eax, a
			jmp [eax]
		}
	}
	_except(1) {
		return 0;
	}

	return 1;
}

