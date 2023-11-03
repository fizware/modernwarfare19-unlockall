#include "menu.h"
#include "style.h"
#include "sdk.h"
ImFont* Main;

void UnlockMain()
{
	if (ImGui::Button("Unlock ALL", ImVec2(-1,26)))
	{
		unlock();
	}
}

void cMenu::DrawMenu()
{
	EditorColorScheme::ApplyTheme();

	ImGui::SetNextWindowSize(ImVec2(218, 90), ImGuiCond_Always);

	ImGui::Begin(xorstr_("Unlock All"), &g_vars->msettings.b_menuEnable, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize);

	int dwWidth = GetSystemMetrics(SM_CXSCREEN) / 3;
	int dwHeight = GetSystemMetrics(SM_CYSCREEN) / 2;
	ImGui::Dummy(ImVec2(0.0f, 1.0f));

	for (int i = 0; i < 5; i++)
	{
		ImGui::Spacing();
	}
	UnlockMain();
}

