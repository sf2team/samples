// St.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "excpt.h"

//undef for all tests
//#define TEST	17

void result(unsigned int r)
{static int n=1;
#ifdef TEST
	n = TEST;
#endif
	printf("Test%02d: %s\n",n++,r?"emulated":"native");
#ifdef TEST
	exit(r);
#endif
}

int _tmain(int argc, _TCHAR* argv[])
{

	unsigned int r;
	unsigned long long m;
	unsigned short n;

#if !defined(TEST) || TEST==1
	_asm{
		mov ecx,0x10001		;shouln't loop, cx==zero
 _emit 0x67
   loop x
   xor ecx,ecx
x:   mov [r],ecx
	}
	result(r);
#endif

#if !defined(TEST) || TEST==2
	_asm{
		mov ecx,0x10000		;should jump, cx==zero
		_emit 0x67
			jecxz y
			inc ecx
		y:	and ecx, 1
		mov [r],ecx
	}
	result(r);
#endif

#if !defined(TEST) || TEST==3
	__try{
	_asm{
		lea eax,[r]			;should fire SEH by jumping inside first 64kB, avoid all unrelated SEHs for emus that take rel16 or even rel32
		mov dword ptr [eax], 1
		_emit 0x66
		jmp dword ptr z		;emits 0e9h opcode and 4x 00h, NOT 0ffh absolute indirect - MASM sux
		z:
	}}
	__except(EXCEPTION_EXECUTE_HANDLER)
		{ if( GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ) r=0; }
	result(r);
#endif

#if !defined(TEST) || TEST==4	/* FAILS EVEN ON REAL CPU WHY ? */
	_asm{
		lea eax,[m]
		sidt [eax]
		mov ebx, [eax+2]
		inc byte ptr[eax+5]
		_emit 0x66			;should omit the 4th base byte, OR store 00h OR store 0ffh there, but for some reason the 4th byte is always properly written on my CPU
		sidt [eax]
		xor ebx, [eax+2]
		mov [r],ebx
	}
	result(!r);
	printf("    ^^--- sidt always stores 6th byte correctly?\n");
#endif

#if !defined(TEST) || TEST==5
	__try{
	_asm{
		mov ecx,2		;should fire SEH by jumping inside first 64kB, when loop is taken
		mov [r],ecx
		_emit 0x66
		loop u		;seems that all instructions that reload EIP zero upper 16 bit of target EIP
		u:
	}}
	__except(EXCEPTION_EXECUTE_HANDLER)
		{ if( GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ) r=0; }
	result(r);
#endif

#if !defined(TEST) || TEST==6
	__try{
	_asm{
		or [r],1		;should fire SEH by jumping inside first 64kB, when loop is taken
		_emit 0x66
		jnz	v	;seems that all instructions that reload EIP zero upper 16 bit of target EIP
		v:
	}}
	__except(EXCEPTION_EXECUTE_HANDLER)
		{ if( GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ) r=0; }
	result(r);
#endif

#if !defined(TEST) || TEST==7
	__try{
	_asm{
		mov [r],1		;should fire SEH by jumping inside first 64kB, when loop is taken
		_emit 0x66
		jmp	q	;seems that all instructions that reload EIP zero upper 16 bit of target EIP
		q:
	}}
	__except(EXCEPTION_EXECUTE_HANDLER)
		{ if( GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ) r=0; }
	result(r);
#endif

#if !defined(TEST) || TEST==8
	__try{
	_asm{
		xor eax,eax
		mov [r],eax
		mov eax,fs:[0x34]
		mov ebx,t
		mov fs:[0x34],ebx		;might jump using badly formed m32/m16 pointer, to badly formed target with zeroed upper EIP bits etc.
		mov ebx,0xffff0034
		_emit 0x67
		jmp	dword ptr fs:[edi]			;in 16bit mode it's [bx], not [edi]
t:
		mov fs:[0x34],eax
	}}
	__except(EXCEPTION_EXECUTE_HANDLER)
		{ if( GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ) r=1; }
	result(r);
#endif

#if !defined(TEST) || TEST==9
	__try{
	_asm{
		mov [r],1
		mov eax,fs:[0x34]
		mov ebx,w
		mov fs:[0x34],ebx		;might jump using badly formed m32/m16 pointer, to badly formed target with zeroed upper EIP bits etc.
		mov ebx,0x80000034
		_emit 0x67
		_emit 0x66
		jmp	dword ptr fs:[edi]			;in 16bit mode it's [bx], not [edi]
w:
		mov fs:[0x34],eax
	}}
	__except(EXCEPTION_EXECUTE_HANDLER)
		{ if( GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ) r=0; }
	result(r);
#endif

#if !defined(TEST) || TEST==10
	__try{
	_asm{
		xor edi,edi
		xor esi,esi
		mov [r],edi
		mov ecx,0xffff0000
		cld
		_emit 0x67
		rep movsd	;should move 0 dwords
	}}
	__except(EXCEPTION_EXECUTE_HANDLER)
		{ if( GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ) r=1; }
	result(r);
#endif

#if !defined(TEST) || TEST==11
	__try{
	_asm{
		std
		xor esi,esi
		xor edi,edi
		mov [r],esi
		mov ecx,0xffff0001
		mov [n],es
		push fs
		pop es
		_emit 0x67
		_emit 0x64
		rep movsb		;high 16 bytes of esi should stay zero
		and esi,0xffff0000
		mov [r],esi
		cld				;a must
	}}
	__except(EXCEPTION_EXECUTE_HANDLER)
		{ if( GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ) r=1; }
	_asm mov es,[n]
	result(r);
#endif

#if !defined(TEST) || TEST==12
	__try{
	_asm{
		mov [n],es
		xor eax,eax
		mov es,ax
		inc eax
		mov [r],eax
		_emit 0xa3	;just because i hate MASM, forcing yout to write/emit ds:[0]
		_emit 00
		_emit 00
		_emit 00
		_emit 00
	}}
	__except(EXCEPTION_EXECUTE_HANDLER)
		{ if( GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ) r=0; }
	_asm mov es,[n]
	result(r);
#endif

//test 0x0d was a SMC without intermediate jmp
//on older cores, the un-modified instruction was executed instead
//because of outdated pre-decoded contents in the pipeline (flushed by jmp)
//does not work on modern cores anymore, thus, next one is test 14

#if !defined(TEST) || TEST==0x0d
	result(0);
	printf("    ^^--- this test is off, modern cores handle smc with instruction granularity again.\n");
#endif

#if !defined(TEST) || TEST==14
	__try{
	_asm{
		mov eax,c
		mov [r],eax
		_emit 0x66
		call eax	;the same for jump
		jmp d
c:		ret
d:
	}}
	__except(EXCEPTION_EXECUTE_HANDLER)
		{ if( GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ) r=0; }
	result(r);
#endif

#if !defined(TEST) || TEST==15
	__try{
	_asm{
		push ebp
		xor eax,eax
		mov [r],eax
		mov eax,0x9090c348
		push eax
		mov eax,0xc3004531
		push eax
		mov eax,0x53000000
		mov ebp,esp
		call ebp
		pop ebp
		pop ebp
		pop ebp
		xor eax,0x52ffffff
		mov [r],eax
	}}
	__except(EXCEPTION_EXECUTE_HANDLER)
		{ if( GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ) r=1; }
	result(r);
#endif

#if !defined(TEST) || TEST==16
	__try{
	_asm{
		mov ebx,fs:[0x34]
		mov esi,0x80000034
		mov edi,esi
		inc edi
		mov ecx,0xffff0001
		mov eax,0x77665544
		mov fs:[0x34],eax
		mov [n],es
		push es
		push fs
		pop es
		_emit 0x67
		_emit 0x64
		_emit 0x66
		rep movsd
		mov eax,es:[0x34]
		pop es
		xor eax,0x77554444
		mov [r],eax
		mov fs:[0x34],ebx
	}}
	__except(EXCEPTION_EXECUTE_HANDLER)
		{ if( GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ) r=1; }
	_asm mov es,[n]
	result(r);
#endif

#if !defined(TEST) || TEST==17
	__try{
	_asm{
		mov [n],ds
		xor eax,eax
		mov ds,ax
		_emit 0xa3	;fire exception
		_emit 00
		_emit 00
		_emit 00
		_emit 00
		push es		;never used now
		pop ds

	}}
	__except(EXCEPTION_EXECUTE_HANDLER)
		{ 
		_asm {
				push ds
				pop eax
				push es
				pop ds
				xor [n],ax
		}
		if( GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ) r=n; }
	result(r);
#endif

#if !defined(TEST) || TEST==18	/* FAILS EVEN ON REAL CPU WHY ? */
/*	
...PLACEHOLDER
for(int i=100000; i--;)
	{
	_asm{
		lea eax,[m]
		sidt [eax]
	}
	printf("GDT base: %08X\n",*((unsigned long *)((char *)&m+2)));
	}*/
#endif

	return 0;
}

