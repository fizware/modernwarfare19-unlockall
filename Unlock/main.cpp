#include "main.h"
#include <sys/stat.h>
#include "mem.h"

#define StartThread(mainThread, dllhandle) I_beginthreadex(0, 0, (_beginthreadex_proc_type)mainThread, dllhandle, 0, 0);
HANDLE hMainThread = NULL;

std::ptrdiff_t bMode = (std::ptrdiff_t)mem::modinfo(mem::info::modbase);
std::ptrdiff_t iSize = (std::ptrdiff_t)mem::modinfo(mem::info::imagesize);

void Log_(const char* fmt, ...) {
	char		text[4096];
	va_list		ap;
	va_start(ap, fmt);
	vsprintf_s(text, fmt, ap);
	va_end(ap);

	std::ofstream logfile(xorstr_("C:\\fiz\\debug\\_debuglog.logs"), std::ios::app);
	if (logfile.is_open() && text)	logfile << text << std::endl;
	logfile.close();
}

bool IsPathExist(const std::string& s)
{
	struct stat buffer;
	return (stat(s.c_str(), &buffer) == 0);
}

__declspec(dllexport)HRESULT present_hk(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!pSwapChain)
		return ori_present(pSwapChain, SyncInterval, Flags);
	if (d3d12::pCommandQueue == nullptr)
	{
		ori_present(pSwapChain, SyncInterval, Flags);
	}

	DXGI_SWAP_CHAIN_DESC sdesc;
	pSwapChain->GetDesc(&sdesc);
	if (sdesc.OutputWindow == GetForegroundWindow())
	{
		Main = imgui::start(
			reinterpret_cast<IDXGISwapChain3*>(pSwapChain),
			reinterpret_cast<ID3D12CommandQueue*>((d3d12::pCommandQueue)), nullptr, 16.0f);

		imgui::imgui_frame_header();

		if (GetAsyncKeyState(VK_END) & 1)
			kiero::shutdown();

		if (GetAsyncKeyState(VK_INSERT) & 1)
			g_vars->msettings.b_menuEnable = !g_vars->msettings.b_menuEnable;

		if (g_vars->msettings.b_menuEnable)
		{
			ImGui::GetIO().MouseDrawCursor;
			menu->DrawMenu();
		}
		imgui::imgui_no_border();

		imgui::imgui_frame_end();
	}
	return ori_present(pSwapChain, SyncInterval, Flags);
}

int GetLogonStatus(int status) {
	return reinterpret_cast<int(*)(int)>(g_vars->goffsets.logonstatus)(status);
}

void init()
{
	g_vars = new global_vars();
	g_vars->baseModule = bMode;
	
	g_vars->goffsets.logonstatus = Scanner::Scanner(g_vars->baseModule + 0x3000000, (g_vars->baseModule + 0x3000000) + (iSize - 0x3000000), xorstr_("E8 ? ? ? ? 83 F8 02 0F 84 ? ? ? ? 48 89"), xorstr_("LogonStatus"), false).resolve_call();
	LOGS_ADDR(g_vars->goffsets.logonstatus);

	while (GetLogonStatus(0) != 2)
	{
		std::this_thread::sleep_for(
			std::chrono::milliseconds(5));
	}

	g_vars->mwprocID = utility::GetProcId(L"cod.exe");
	g_vars->hProc = OpenProcess(PROCESS_ALL_ACCESS, NULL, g_vars->mwprocID);
	g_vars->hWind = process::get_process_window();

}

DWORD WINAPI  initthread(HMODULE hModule)
{
	hMainThread = GetCurrentThread();

	init();

	if (!IsPathExist(xorstr_("C:\\fiz")))
	{
		CreateDirectoryA(xorstr_("C:\\fiz"), NULL);
	}

	if (g_vars->msettings.b_debugEnable)
	{
		if (!IsPathExist(xorstr_("C:\\fiz\\Debug")))
		{
			CreateDirectoryA(xorstr_("C:\\fiz\\Debug"), NULL);
		}
	}
	else
	{
		if (IsPathExist(xorstr_("C:\\fiz\\Debug")))
		{
			std::filesystem::remove(xorstr_("C:\\fiz\\Debug\\_debuglog.log"));
		}
	}
	if (g_vars->mwprocID)
	{
		//g_vars->goffsets.unlock_offsets = Scanner::Scanner(g_vars->baseModule + 0x500000, (g_vars->baseModule + 0x500000) + (iSize - 0x500000), xorstr_("33 FF 48 8D 15 ? ? ? ? 48 89 05 ? ? ? ? 48 8D 0D"), xorstr_("LogonStatus"), false).getaddr();
	//	if (g_vars->msettings.b_debugEnable)
		//	LOGS_ADDR(g_vars->goffsets.unlock_offsets);

		if (kiero::init(kiero::RenderType::D3D12) == kiero::Status::Success)
		{
			kiero::bind(54, (void**)&oExecuteCommandListsD3D12, imgui::hookExecuteCommandListsD3D12);
			kiero::bind(140, (void**)&ori_present, present_hk);

		}
	}
	else
	{
		return 0;
	}


	return 0;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		StartThread(initthread, module);
		break;
	case DLL_PROCESS_DETACH:
		kiero::shutdown();
		break;
	}
	return TRUE;
	return TRUE;
}
