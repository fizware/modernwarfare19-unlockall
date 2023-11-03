#include "utility.h"
#include "global.h"



DWORD utility::GetProcId(const wchar_t* procName)
{
	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				if (!_wcsicmp(procEntry.szExeFile, procName))
				{
					procId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));
		}
	}
	CloseHandle(hSnap);
	return procId;
}

HMODULE utility::GetModuleHandleSafe(const wchar_t* pszModuleName)
{
	HMODULE hmModuleHandle = NULL;

	do
	{
		hmModuleHandle = GetModuleHandle(pszModuleName);
		Sleep(1);
	} while (hmModuleHandle == NULL);

	return hmModuleHandle;
}

bool MemCompare(const BYTE* bData, const BYTE* bMask, const char* szMask) {
	for (; *szMask; ++szMask, ++bData, ++bMask) {
		if (*szMask == 'x' && *bData != *bMask) {
			return false;
		}
	}
	return (*szMask == NULL);
}

void utility::Read(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess)
{
	ReadProcessMemory(hProcess, dst, src, size, nullptr);
}

void utility::Write(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess)
{
	DWORD oldprotect;
	VirtualProtectEx(hProcess, dst, size, PAGE_READWRITE, &oldprotect);
	WriteProcessMemory(hProcess, dst, src, size, nullptr);
	VirtualProtectEx(hProcess, dst, size, oldprotect, &oldprotect);
}

void utility::Patch(BYTE* dst, BYTE* src, unsigned size)
{
	DWORD oldprotect;
	VirtualProtect(dst, size, PAGE_READWRITE, &oldprotect);
	memcpy(dst, src, size);
	VirtualProtect(dst, size, oldprotect, &oldprotect);
}

void utility::Nop(BYTE* dst, unsigned int size)
{
	DWORD oldprotect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memset(dst, 0x90, size);
	VirtualProtect(dst, size, oldprotect, &oldprotect);
}

uintptr_t utility::PatternScanEx(HANDLE hProcess, uintptr_t start, uintptr_t size, const char* sig, const char* mask)
{
	BYTE* data = new BYTE[size];
	SIZE_T bytesRead;

	ReadProcessMemory(hProcess, (LPVOID)start, data, size, &bytesRead);

	for (uintptr_t i = 0; i < size; i++)
	{
		if (MemCompare((const BYTE*)(data + i), (const BYTE*)sig, mask)) {
			return start + i;
		}
	}
	delete[] data;
	return NULL;
}

uintptr_t utility::FindOffset(HANDLE hProcess, uintptr_t start, uintptr_t size, const char* sig, const char* mask, uintptr_t base_offset, uintptr_t pre_base_offset, uintptr_t rindex, bool addRip)
{
	auto address = utility::PatternScanEx(hProcess, start, size, sig, mask) + rindex;
	if (!address)
		return 0;
	auto ret = pre_base_offset + *reinterpret_cast<int32_t*>(address + base_offset);

	if (addRip)
	{
		ret = ret + address;
		if (ret)
			return (ret - g_vars->baseModule);
	}

	return ret;
}

DWORD  utility::FindPattern(uintptr_t start, uintptr_t size, const char* pattern, const char* mask)
{
	DWORD patternLength = (DWORD)strlen(mask);

	for (DWORD i = 0; i < size - patternLength; i++)
	{
		bool found = true;

		for (DWORD j = 0; j < patternLength; j++)
		{
			found &= mask[j] == '?' || pattern[j] == *(char*)(start + i + j);
		}

		if (found)
			return start + i;
	}

	return 0xDEADBEEF;
}

void utility::PatchBytes(BYTE* destination, BYTE* source, size_t size)
{
	DWORD oldProtect;
	VirtualProtect((LPVOID)destination, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(destination, source, size);
	VirtualProtect(destination, size, oldProtect, &oldProtect);
}