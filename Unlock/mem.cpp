#include "mem.h"
#include "utility.h"
#define DEBUG_ERROR(msg, ...) MessageBoxA(0, std::format(msg, __VA_ARGS__).c_str(), "Error", MB_OK) 
void mem::memcopy(BYTE* dst, BYTE* src, size_t size) {
    DWORD oldprotect; //variable to hold protection
    VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect); //make memory read/writeable and save the old protection
    memcpy(dst, src, size); //typical memcopy
    VirtualProtect(dst, size, oldprotect, &oldprotect); //restore our old protection
}

uintptr_t mem::modinfo(info param) {
    MODULEINFO moduleInfo{};
    if (!GetModuleInformation((HANDLE)-1, GetModuleHandle(NULL), &moduleInfo, sizeof(MODULEINFO)) || !moduleInfo.lpBaseOfDll) {
        std::cout << "Couldnt GetModuleInformation" << std::endl;
        return 0;
    }
    switch (param)
    {
    case 1:
        return (uintptr_t)moduleInfo.lpBaseOfDll; //returns module base
        break;
    case 2:
        return (uintptr_t)moduleInfo.SizeOfImage; //return image size
        break;
    }
}

uintptr_t mem::find_pattern(uintptr_t range_start, uintptr_t range_end, const char* pattern)
{
    const char* pat = pattern;
    __int64 firstMatch = NULL;
    __int64 pCur = range_start;
    __int64 region_end{};
    MEMORY_BASIC_INFORMATION mbi{};
    SIZE_T size = VirtualQuery((LPCVOID)pCur, &mbi, sizeof(mbi));

    while (sizeof(mbi) == size) {
        if (pCur >= range_end - strlen(pattern))
        {
            break;
        }
        if (!(mbi.Protect & (PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_READWRITE))) {
            pCur += mbi.RegionSize;
            continue;
        }
        region_end = pCur + mbi.RegionSize;

        if (mbi.State == MEM_COMMIT && mbi.Protect != PAGE_NOACCESS && !(mbi.Protect & PAGE_GUARD))
        {
            region_end = pCur + mbi.RegionSize;
            while (pCur < region_end)
            {
                if (!*pat)
                    return firstMatch;
                if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat)) {
                    if (!firstMatch)
                        firstMatch = pCur;
                    if (!pat[1] || !pat[2])
                        return firstMatch;
                    if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
                        pat += 3;
                    else
                        pat += 2;
                }
                else {
                    if (firstMatch)
                        pCur = firstMatch;
                    pat = pattern;
                    firstMatch = 0;
                }
                pCur++;
            }
        }
    }
    return NULL;
}

Scanner::Scanner(std::ptrdiff_t startAddr, std::ptrdiff_t endAddr, const char* pattern, const char* name, bool debug, std::ptrdiff_t offset) : name{ name }, debugflag{ debug }
{
    addr = (mem::find_pattern(startAddr, endAddr, pattern) + (offset));
}

auto Scanner::validaddr() -> bool
{
    if (is_bad_ptr(addr))
    {
        if (debugflag)
        {
            DEBUG_ERROR("FAILED!! {} @ : {:#x}", name, addr);
        }
        return false;
    }
    if (is_valid_ptr(addr))
    {
        if (debugflag)
        {
            DEBUG_ERROR("found {} @: {:#x}. offset: {:#x}", name, addr, addr - g_vars->baseModule);
        }
        return true;
    }
};

auto Scanner::getaddr() -> std::ptrdiff_t {
    if (validaddr()) {
        return addr;
    }
    return NULL;
};

auto Scanner::resolve_call() -> std::ptrdiff_t {
    if (validaddr()) {
        return *(int*)(addr + 1) + addr + 5;
    }
    return NULL;
};

auto Scanner::resolve_lea() -> std::ptrdiff_t {
    if (validaddr()) {
        return *(int*)(addr + 3) + addr + 7;
    }
    return NULL;
};

//works with XOR
auto Scanner::resolve_cmp() -> std::ptrdiff_t {
    if (validaddr()) {
        return *(int*)(addr + 2) + addr + 6;
    }
    return NULL;
};