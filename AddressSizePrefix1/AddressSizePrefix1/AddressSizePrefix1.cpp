// AddressSizePrefix1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <excpt.h> 


unsigned int bxsi(unsigned short _bx, unsigned short _si)
{
	volatile unsigned int res = 0;

	_try {
		_asm {
			mov ebx, 0x10000
			mov esi, 0x10000
			mov bx, _bx
			mov si, _si
			_emit 0x67
			mov ax, [eax]	// [bx+si]
		}
	}
	_except(res = (GetExceptionInformation())->ExceptionRecord->ExceptionInformation[1], 1) {
	}

	return res;
}

unsigned int bxdi(unsigned short _bx, unsigned short _di)
{
	volatile unsigned int res = 0;

	_try {
		_asm {
			mov ebx, 0x10000
			mov edi, 0x10000
			mov bx, _bx
			mov di, _di	
			_emit 0x67
			mov ax, [ecx]	// [bx+di]
		}
	}
	_except(res = (GetExceptionInformation())->ExceptionRecord->ExceptionInformation[1], 1) {
	}

	return res;
}


unsigned int bpsi(unsigned short _bp, unsigned short _si)
{
	volatile unsigned int res = 0;

	_try {
		_asm {
			mov esi, 0x10000
			mov si, _si	
			mov bp, _bp
			_emit 0x67
			mov ax, [edx]	// [bp+si]
		}
	}
	_except(res = (GetExceptionInformation())->ExceptionRecord->ExceptionInformation[1], 1) {
	}

	return res;
}

unsigned int bpdi(unsigned short _bp, unsigned short _di)
{
	volatile unsigned int res = 0;

	_try {
		_asm {
			mov di, _di	
			mov bp, _bp
			_emit 0x67
			mov ax, [ebx]	// [bp+di]
		}
	}
	_except(res = (GetExceptionInformation())->ExceptionRecord->ExceptionInformation[1], 1) {
	}

	return res;
}

unsigned int si(unsigned short _si)
{
	volatile unsigned int res = 0;

	_try {
		_asm {
			mov si, _si	
			_emit 0x67
			_emit 0x66
			_emit 0x8b
			_emit 0x04
		}
	}
	_except(res = (GetExceptionInformation())->ExceptionRecord->ExceptionInformation[1], 1) {
	}

	return res;
}

unsigned int di(unsigned short _di)
{
	volatile unsigned int res = 0;

	_try {
		_asm {
			mov di, _di	
			_emit 0x67
			_emit 0x66
			_emit 0x8b
			_emit 0x05
		}
	}
	_except(res = (GetExceptionInformation())->ExceptionRecord->ExceptionInformation[1], 1) {
	}

	return res;
}

unsigned int bx(unsigned short _bx)
{
	volatile unsigned int res = 0;

	_try {
		_asm {
			mov bx, _bx	
			_emit 0x67
			_emit 0x66
			_emit 0x8b
			_emit 0x07
		}
	}
	_except(res = (GetExceptionInformation())->ExceptionRecord->ExceptionInformation[1], 1) {
	}

	return res;
}


unsigned int disp16(void)
{
	volatile unsigned int res = 0;

	_try {
		_asm {
			_emit 0x67
			_emit 0x66
			_emit 0x8b
			_emit 0x06
			_emit 0xee
			_emit 0xee
		}
	}
	_except(res = (GetExceptionInformation())->ExceptionRecord->ExceptionInformation[1], 1) {
	}

	return res;
}

unsigned int bxsidisp8(unsigned short _bx, unsigned short _si)
{
	volatile unsigned int res = 0;

	_try {
		_asm {
			mov bx, _bx
			mov si, _si
			_emit 0x67
			_emit 0x66
			_emit 0x8b
			_emit 0x40
			_emit 0x01
		}
	}
	_except(res = (GetExceptionInformation())->ExceptionRecord->ExceptionInformation[1], 1) {
	}

	return res;
}

unsigned int bxsidisp16(unsigned short _bx, unsigned short _si)
{
	volatile unsigned int res = 0;

	_try {
		_asm {
			mov bx, _bx
			mov si, _si
			_emit 0x67
			_emit 0x66
			_emit 0x8b
			_emit 0x40
			_emit 0x00
			_emit 0x00
		}
	}
	_except(res = (GetExceptionInformation())->ExceptionRecord->ExceptionInformation[1], 1) {
	}

	return res;
}

unsigned short reg(unsigned short _reg)
{
	volatile unsigned short res = 0;

	_asm {
		mov dx, _reg
		_emit 0x67
		_emit 0x66
		_emit 0x8b
		_emit 0xc2
		mov res, ax
	}

	return res;
}


int _tmain(int argc, _TCHAR* argv[])
{
	if (bxsi(0x1111, 0x2222) != 0x3333)
		return 1;

	if (bxdi(0x1212, 0x2222) != 0x3434)
		return 2;

	if (bpsi(0x1234, 0x5678) != 0x68ac)
		return 3;

	if (bpdi(0xf000, 0x1000) != 0x0000)
		return 4;

	if (si(0xcafe) != 0xcafe)
		return 5;

	if (di(0xdead) != 0xdead)
		return 5;

	if (disp16() != 0xeeee)
		return 6;

	if (bx(0x1) != 0x1)
		return 7;

	if (bxsidisp8(0x1111, 0x2222) != 0x3334)
		return 8;

	if (bxsidisp16(0x1111, 0x2222) != 0x3333)
		return 16;

	if (reg(0xf0f0) != 0xf0f0)
		return 32;

	return 0;
}

