#pragma once
#include "framework.h"

#define INRANGE(x,a,b)    (x >= a && x <= b)
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

namespace utility
{
	DWORD GetProcId(const wchar_t* procName);
	HMODULE GetModuleHandleSafe(const wchar_t* pszModuleName);
	void Read(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess);
	void Write(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess);
	void Patch(BYTE* dst, BYTE* src, unsigned size);
	void Nop(BYTE* dst, unsigned int size);
	uintptr_t PatternScanEx(HANDLE hProcess, uintptr_t start, uintptr_t size, const char* sig, const char* mask);
	uintptr_t FindOffset(HANDLE hProcess, uintptr_t start, uintptr_t size, const char* sig, const char* mask, uintptr_t base_offset, uintptr_t pre_base_offset, uintptr_t rindex, bool addRip = true);
	DWORD  FindPattern(uintptr_t start, uintptr_t size, const char* pattern, const char* mask);
	void PatchBytes(BYTE* destination, BYTE* source, size_t size);
}