#include "sdk.h"

HWND process::hwnd;

BOOL CALLBACK process::EnumWindowCallBack(HWND hWnd, LPARAM lParam)
{
	DWORD dwPid = 0;
	GetWindowThreadProcessId(hWnd, &dwPid);
	if (dwPid == lParam)
	{
		hwnd = hWnd;
		return FALSE;
	}
	return TRUE;
}

HWND process::get_process_window()
{
	if (hwnd)
		return hwnd;

	EnumWindows(process::EnumWindowCallBack, GetCurrentProcessId());

	if (hwnd == NULL)
		Exit();

	return hwnd;
}

HWND process::get_process_window(DWORD procID)
{
	if (hwnd)
		return hwnd;

	EnumWindows(process::EnumWindowCallBack, procID);

	if (hwnd == NULL)
		Exit();

	return hwnd;
}

namespace l_menu
{
	void ui_header()
	{
	
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::Begin("A", reinterpret_cast<bool*>(true), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
			ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);
			int dwWidth = GetSystemMetrics(SM_CXSCREEN) / 4;
			int dwHeight = GetSystemMetrics(SM_CYSCREEN) / 2;
	}

	void ui_end()
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		window->DrawList->PushClipRectFullScreen();
		ImGui::End();
		ImGui::PopStyleColor();
	}

	void init(ImFont* font)
	{
		ui_header();
		ui_end();
	}
}

void unlock()
{
	uintptr_t num = (g_vars->goffsets.unlock_offsets + 0xC);
//	int num2 = mem::readMemory<int>(num);

	uintptr_t unlock_base = num + 4 - g_vars->baseModule;
	uintptr_t numP = (g_vars->baseModule + unlock_base + 0x60);

//	memcpy((BYTE*)numP, (BYTE*)xorstr_("\x48\x83\xC4\x08\x48\x8B\x5C\x24\x30\x48\x8B\x74\x24\x38\x48\x83\xC4\x20\x5F\x48\xC7\xC0\x01\x00\x00\x00\xC3"), 28);
	mem::writeMemory<uintptr_t>(g_vars->baseModule + unlock_base + 8, numP);
	mem::writeMemory<uintptr_t>(g_vars->baseModule + unlock_base, g_vars->baseModule + unlock_base);
}
