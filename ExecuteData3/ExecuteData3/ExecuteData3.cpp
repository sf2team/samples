// ExecuteData3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "excpt.h"

char *buffer;

int _tmain(int argc, _TCHAR* argv[])
{
	buffer = (char *) VirtualAlloc(NULL, 1, MEM_COMMIT, PAGE_NOACCESS);
	struct _EXCEPTION_POINTERS *info = NULL;

	bool dep_enabled = false;

	DWORD flags;
	BOOL permanent;
	if (GetProcessDEPPolicy((HANDLE) -1, &flags, &permanent)) {
		if (flags & 1)
			dep_enabled = true;
	}

	bool pass = false;
	__try {
		_asm {
			call dword ptr [buffer]
		}
	}
	__except(info = GetExceptionInformation(), EXCEPTION_EXECUTE_HANDLER) {
		int expected_av_mode = dep_enabled ? 8 : 0;
		if ((GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION) &&
			(info->ExceptionRecord->ExceptionInformation[0] == expected_av_mode))
			pass = true;
	}

	if (!pass) {
		printf("emulated\n");
		return 1;
	}

	DWORD  oldProt;
	VirtualProtect(buffer, 1, PAGE_READONLY | PAGE_GUARD, &oldProt);

	pass = false;
	__try {
		_asm {
			call dword ptr [buffer]
		}
	}
	__except(info = GetExceptionInformation(), EXCEPTION_EXECUTE_HANDLER) {
		int expected_gp_mode = 8;
		if ((GetExceptionCode() == EXCEPTION_GUARD_PAGE) &&
			(info->ExceptionRecord->ExceptionInformation[0] == expected_gp_mode))
			pass = true;
	}

	if (!pass) {
		printf("emulated\n");
		return 2;
	}

	printf("native\n");
	return 0;
}


