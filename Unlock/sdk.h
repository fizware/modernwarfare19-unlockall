#pragma once
#include "framework.h"
#include "global.h"
#include "mem.h"

extern HMODULE m_hModule;

#define _PTR_MAX_VALUE ((PVOID)0x000F000000000000)
#define BYTEn(x, n)   (*((BYTE*)&(x)+n))
#define BYTE1(x)   BYTEn(x,  1)

//auto padding
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

#define is_valid_ptr(p) ((uintptr_t)(p) <= 0x7FFFFFFEFFFF && (uintptr_t)(p) >= 0x1000) 
#define is_bad_ptr(p)	(!is_valid_ptr(p))

template <typename T>
bool IsValidPtr(PVOID ptr)
{
	if (is_bad_ptr(ptr))return false;
	else
		return true;

}

namespace process
{
	extern HWND hwnd;
	BOOL CALLBACK EnumWindowCallBack(HWND hWnd, LPARAM lParam);
	HWND get_process_window();
	HWND get_process_window(DWORD procID);
}

namespace l_menu
{
	void init(ImFont* font);
}
void unlock();
