#pragma once

#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <thread>
#include <iostream>
#include <mutex>
#include <random>
#include <d3d11.h>
#include "includes/imgui/imgui.h"
#include "includes/imgui/imgui_impl_dx11.h"
#include "includes/imgui/imgui_internal.h"
#include <dxgi1_4.h>
#include "includes/imgui/imgui_impl_win32.h"
#include "includes/imgui/imgui_impl_dx12.h"
#include <sstream>
#include <cstdint>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include "xorstr.hpp"
#include "lazyimporter.h"
#include <timeapi.h>
#include <d3d11_3.h>
#include "defs.h"
#include <functional>
#include <unordered_map>
#include <map>
#include <timeapi.h>
#include <sysinfoapi.h>
#include <filesystem>
#include <kiero/kiero.h>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#include "includes/minhook/MinHook.h"
#pragma comment(lib, "minhook.lib")

#define Exit() 0

#define QWORD unsigned __int64


extern void Log_(const char* fmt, ...);
#define LOGS(fmt, ...) Log_(xorstr_(fmt), ##__VA_ARGS__)

#define LOGS_ADDR(var_name)										\
		LOGS(#var_name ": 0x%llX (0x%llx)", var_name, var_name > g_vars->baseModule - 0x10000 ? var_name - g_vars->baseModule : 0);

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
