#pragma once

#include <Windows.h>
#include <dxgi.h>
#include <d3d11.h>

#include "../Core/global.h"

#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_win32.h"
#include "../../imgui/imgui_impl_dx11.h"

#include "../Core/includes.h"

#include "../../kiero/kiero.h"
#include <filesystem>

#include "../Core/git_commit.h"
#include "../../Public/Snowdrop/SDK.h"
#include "../../Public/Snowdrop/MCommon/MC_String.h"
#include "camera_loop.h"
using namespace Snowdrop;

HRESULT (WINAPI*PresentOriginal)(IDXGISwapChain* SwapChain, uint32_t Interval, uint32_t Flags);

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HWND wnd = nullptr;
WNDPROC oWndProc;
ID3D11Device* pDevice = nullptr;
ID3D11DeviceContext* pContext = nullptr;
ID3D11RenderTargetView* mainRenderTargetView;

static bool isCapturingKey = false;
static bool keyJustPressed[256] = {};
static bool keyWasDown[256] = {};

static UINT menuToggleKey = VK_INSERT;
static bool bAllowInputPassThrough = false;
static bool bNoMenu = false;
static bool bSkipStartup = false;
static bool bHasInit = false;
static bool bShow = true;

// Camera controls
bool bCameraOverrideEnabled = false;
bool bFreecamEnabled = false;
float fCustomFOV = 120.0f;
bool bFOVOverrideEnabled = false;
float fFreecamSensitivity = 1.0f;
float fScrollFOVDelta = 0.0f;

std::string getKeyDisplayName(UINT vkCode)
{
	char keyName[64] = {};

	switch (vkCode)
	{
	case VK_INSERT: return "Insert";
	case VK_DELETE: return "Delete";
	case VK_HOME: return "Home";
	case VK_END: return "End";
	case VK_PRIOR: return "Page Up";
	case VK_NEXT: return "Page Down";
	case VK_ESCAPE: return "Escape";
	case VK_RETURN: return "Enter";
	case VK_SPACE: return "Space";
	case VK_TAB: return "Tab";
	case VK_BACK: return "Backspace";
	case VK_SHIFT: return "Shift";
	case VK_CONTROL: return "Ctrl";
	case VK_MENU: return "Alt";
	case VK_LWIN: return "Left Win";
	case VK_RWIN: return "Right Win";
	case VK_APPS: return "Menu";
	case VK_NUMLOCK: return "Num Lock";
	case VK_SCROLL: return "Scroll Lock";
	case VK_CAPITAL: return "Caps Lock";
	case VK_PAUSE: return "Pause";
	case VK_SNAPSHOT: return "Print Screen";
	}

	if (vkCode >= VK_F1 && vkCode <= VK_F24)
	{
		snprintf(keyName, sizeof(keyName), "F%d", vkCode - VK_F1 + 1);
		return std::string(keyName);
	}

	if (vkCode >= VK_NUMPAD0 && vkCode <= VK_NUMPAD9)
	{
		snprintf(keyName, sizeof(keyName), "Numpad %d", vkCode - VK_NUMPAD0);
		return std::string(keyName);
	}

	UINT scanCode = MapVirtualKeyA(vkCode, MAPVK_VK_TO_VSC);
	if (scanCode != 0)
	{
		int result = GetKeyNameTextA(scanCode << 16, keyName, sizeof(keyName));
		if (result > 0)
		{
			return std::string(keyName);
		}
	}

	snprintf(keyName, sizeof(keyName), "Key %d", vkCode);
	return std::string(keyName);
}

bool isKeyJustPressed(int vkCode)
{
	bool isDown = (GetAsyncKeyState(vkCode) & 0x8000) != 0;
	bool wasDown = keyWasDown[vkCode];
	keyWasDown[vkCode] = isDown;

	return isDown && !wasDown;
}

void setupImGuiStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	style.WindowPadding = ImVec2(10.0f, 10.0f);
	style.FramePadding = ImVec2(5.0f, 5.0f);
	style.ItemSpacing = ImVec2(10.0f, 5.0f);
	style.ItemInnerSpacing = ImVec2(5.0f, 5.0f);
	style.TouchExtraPadding = ImVec2(0.0f, 0.0f);
	style.IndentSpacing = 20.0f;
	style.ScrollbarSize = 14.0f;
	style.GrabMinSize = 12.0f;

	style.WindowBorderSize = 1.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupBorderSize = 1.0f;
	style.FrameBorderSize = 1.0f;
	style.TabBorderSize = 1.0f;
	style.WindowRounding = 4.0f;
	style.ChildRounding = 4.0f;
	style.FrameRounding = 4.0f;
	style.PopupRounding = 4.0f;
	style.ScrollbarRounding = 12.0f;
	style.GrabRounding = 4.0f;
	style.TabRounding = 4.0f;

	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.13f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.10f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.21f, 0.27f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.30f, 0.31f, 0.37f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.40f, 0.41f, 0.47f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.13f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.14f, 0.17f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.71f, 0.71f, 0.71f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.20f, 0.21f, 0.27f, 0.54f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.31f, 0.37f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.40f, 0.41f, 0.47f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.20f, 0.21f, 0.27f, 0.54f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.31f, 0.37f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.40f, 0.41f, 0.47f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.30f, 0.31f, 0.37f, 1.00f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.41f, 0.47f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.21f, 0.27f, 0.54f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.30f, 0.31f, 0.37f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.40f, 0.41f, 0.47f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void textCentered(std::string text)
{
	auto windowWidth = ImGui::GetWindowSize().x;
	auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

	ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
	ImGui::Text(text.c_str());
}

void drawHomeTab()
{
	ImGui::Text("Welcome to FractureLoader!");
	ImGui::Separator();
	ImGui::TextWrapped(
		"FractureLoader is a experimental and WIP mod loader for South Park: The Fractured But Whole, a game built on Massive Entertainment's Snowdrop Engine.");
	ImGui::Spacing();
	ImGui::TextWrapped(
		"Use the other tabs to load faceman layouts or UI graphs, and check the 'Mods' tab to activate or view loaded mods.");

	ImGui::Spacing();

	ImGui::TextColored(ImVec4(1, 1, 0.6f, 1), "=== FractureLoader Information ===");
	ImGui::Text("Version: %s", Global::fractureLoaderVersion);
	ImGui::Text("Git Commit: %s", GIT_COMMIT_HASH);
	ImGui::Text("Compile Time: %s %s", Global::buildDate, Global::buildTime);

	if (ImGui::Button("Open GitHub Repository"))
		ShellExecuteA(nullptr, "open", Global::fractureLoaderRepo.c_str(), nullptr, nullptr, SW_SHOWNORMAL);

	ImGui::Spacing();

	ImGui::TextColored(ImVec4(1, 1, 0.6f, 1), "=== TFBW Client Information (Camel) ===");
	ImGui::Text("Engine Version: %s", Global::engineVersion.c_str());

	ImGui::Spacing();

	ImGui::TextColored(ImVec4(1, 1, 0.6f, 1), "=== Credits ===");
	ImGui::TextWrapped("Created by notpies.\nBuilt using ImGui, Kiero and MinHook.");
}

void drawFacemanTab()
{
	static char layoutPath[256] = "faceman/layouts/Camel Town Viewer.layout";

	ImGui::InputText("##LayoutPath", layoutPath, sizeof(layoutPath));
	ImGui::SameLine();
	if (ImGui::Button("Load Layout"))
	{
		LoadFacemanUILayout(layoutPath, true, nullptr);
	}

	ImGui::NewLine();

	ImGui::Separator();
	ImGui::TextWrapped(
		"Faceman layouts are inspectors used on the Snowdrop Engine editor, most of these layouts features will not work on the Retail version of the game due to stripped code.");
	ImGui::Spacing();
	ImGui::TextWrapped(
		"Note: The Faceman layout loading feature is experimental and may not work as expected. It is intended for testing purposes only.");
}

void drawUIGraphTab()
{
	static char uiGraphPath[256] = "camel/ui/graphs/fpp_startmenu.muigraph";
	static __int64 uiTestHandle = 0;

	ImGui::InputText("##UIGraphPath", uiGraphPath, sizeof(uiGraphPath));
	ImGui::SameLine();
	if (ImGui::Button("Load UI Layout"))
	{
		__int64 newHandle = 0;
		__int64* result = LoadUI(&newHandle, uiGraphPath);

		if (result)
		{
			__int64 oldHandle = uiTestHandle;
			uiTestHandle = *result;
			*result = 0;

			if (oldHandle)
				ReleaseResourceRef(oldHandle);

			FRACTURE_DEBUG("[LoadUI] Successfully loaded UI resource with handle: 0x{:X}", uiTestHandle);

			void* obj = Alloc(0x78);
			if (!obj)
			{
				FRACTURE_DEBUG("Allocation failed");
			}
			else
			{
				FRACTURE_DEBUG("[LoadUI] Allocated object memory at: 0x{:X}", obj);

				auto res = InitUIObj((__int64)obj, uiTestHandle);
				FRACTURE_DEBUG("[LoadUI] Initialized UI object with handle: 0x{:X}", res);

				_BYTE scratch[24] = {};
				_BYTE paramBlob[28] = {};
				scratch[0] = 0;

				BuildParamBlob(paramBlob, *word_143AD9F48, scratch);
				FRACTURE_DEBUG("[LoadUI] Built param blob");

				if (qword_143AD9E68_ptr && *qword_143AD9E68_ptr)
				{
					auto func = (QwordFunc_t)(*qword_143AD9E68_ptr);
					func(scratch);
					FRACTURE_DEBUG(
						"[LoadUI] Called function pointer at qword_143AD9E68 with scratch buffer\n");
				}
				else
				{
					FRACTURE_DEBUG("[LoadUI] qword_143AD9E68 is null or invalid\n");
				}

				_BYTE* blobPtr = paramBlob;
				BindUIParams(res + 16, &blobPtr, 1);
				FRACTURE_DEBUG("[LoadUI] Parameter blob bound to UI object.");

				__int64 unkSystem = GetUISystem(0);
				FRACTURE_DEBUG("[LoadUI] Got system: 0x{:X}", unkSystem);

				__int64 renderList = *(__int64*)(unkSystem + 0xD0);
				FRACTURE_DEBUG("[LoadUI] Render list: 0x{:X}", renderList);

				EnqueueUI(renderList, static_cast<__int64>(res));
			}
		}
	}

	ImGui::NewLine();

	ImGui::Separator();
	ImGui::TextWrapped(
		"UI Graphs are widget objects used by the game to display menus, some of them require to be activated by a certain event or action, which will cause some UI Graphs to not display using this loader.");
	ImGui::Spacing();
	ImGui::TextWrapped(
		"Note: The UI Graph loading feature is experimental and may not work as expected. It is intended for testing purposes only.");
}

void drawModsTab()
{
	if (g_modLoader == nullptr)
	{
		ImGui::Text("The mod loader is not initialized or not ready.");
		return;
	}

	auto& mods = g_modLoader->getMods();

	int enabledCount = 0;
	for (const auto& mod : mods)
	{
		if (mod.enabled) enabledCount++;
	}

	ImGui::Text("Total mods: %d | Enabled: %d", static_cast<int>(mods.size()), enabledCount);
	ImGui::Separator();

	static bool restartRequired = false;

	ImGui::BeginChild("ModsList", ImVec2(0, ImGui::GetContentRegionAvail().y - 70.0f), true);

	const float checkboxOffset = ImGui::GetContentRegionAvail().x - 100.0f;

	for (int i = 0; i < mods.size(); ++i)
	{
		auto& mod = mods[i];

		ImGui::PushID(i);

		ImGui::PushTextWrapPos(0.0f);
		ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.2f, 1.0f), "%s", mod.name.c_str());
		ImGui::PopTextWrapPos();
		ImGui::TextWrapped("Author: %s", mod.author.c_str());
		ImGui::Text("Version: %s", mod.version.c_str());

		if (mod.hasConflict)
		{
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "CONFLICT");
		}

		if (mod.baseMod)
		{
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "BUILT-IN");
		}

		ImGui::TextWrapped("Description: %s", mod.description.c_str());

		ImGui::SetCursorPosX(checkboxOffset);
		bool previousState = mod.enabled;

		if (!mod.baseMod && ImGui::Checkbox("Enabled", &mod.enabled))
		{
			g_modLoader->setModEnabled(mod.name, mod.enabled);

			if (previousState != mod.enabled)
			{
				restartRequired = true;
			}
		}

		ImGui::Separator();

		ImGui::PopID();
	}

	ImGui::EndChild();

	if (restartRequired)
	{
		ImGui::Spacing();
		ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f),
		                   "You must restart the game for changes to take effect.");
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	if (ImGui::Button("Open Mods Folder", ImVec2(-1, 0)))
	{
		ShellExecuteA(nullptr, "open", g_loaderManager->getModsDirectory().c_str(), nullptr, nullptr,
		              SW_SHOWDEFAULT);
	}
}

void drawVFSTab()
{
	static char resourcePath[256] = "camel/gameroot.juice";
	static bool testResult = false;
	static bool testPerformed = false;

	ImGui::Text("Resource Management");
	ImGui::Separator();
	ImGui::TextWrapped(
		"This tab is for testing resource loading using engine functions, useful to test if custom or overridden assets are loading correctly.");
	ImGui::Spacing();

	ImGui::InputText("Resource Path", resourcePath, sizeof(resourcePath));

	if (ImGui::Button("Test Resource Availability", ImVec2(-1, 0)))
	{
		testResult = false;
		testPerformed = true;

		if (SnowdropIsFileAvailable != nullptr)
		{
			testResult = snowdropIsFileAvailableHook((_BYTE*)resourcePath, 0);
		}
		else
		{
			testPerformed = false;
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: SnowdropIsFileAvailable function not loaded.");
		}
	}

	if (testPerformed)
	{
		if (testResult)
			ImGui::TextColored(ImVec4(0, 1, 0, 1), "The requested resource is AVAILABLE!");
		else
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "The requested resource is NOT available.");
	}

	static bool exportPerformed = false;
	static bool exportSuccess = false;

	if (ImGui::Button("Export Resource", ImVec2(-1, 0)))
	{
		exportPerformed = true;
		auto data = VFS::ExtractVFSFile(resourcePath);
		exportSuccess = !data.empty();
	}

	if (exportPerformed)
	{
		if (exportSuccess)
			ImGui::TextColored(ImVec4(0, 1, 0, 1), "Export Successful!");
		else
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Export Failed.");
	}
}

/*void drawAllySwapTab()
{
	ImGui::Text("Ally Swap");
	ImGui::Separator();
	ImGui::TextWrapped("This tab is for swapping allies in your current party.");
	ImGui::Spacing();
}*/

void drawWorldTab()
{
	ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "World Management");
	ImGui::Separator();
	ImGui::Spacing();

	static uintptr_t lastWorldModel = 0;
	static std::vector<ActorEntry> actorSnapshot;
	static int selectedActorIdx = -1;
	static bool waitingForActors = false;
	static uintptr_t cachedPlayer = 0;

	auto* camelClient = CamelClient::GetInstance();
	auto* worldModel = (camelClient) ? camelClient->GetWorldModel() : nullptr;
	uintptr_t currentWorldModel = reinterpret_cast<uintptr_t>(worldModel);

	if (currentWorldModel != lastWorldModel)
	{
		selectedActorIdx = -1;
		waitingForActors = false;
		cachedPlayer = 0;
		lastWorldModel = currentWorldModel;
	}

	if (worldModel && (worldModel->IsLoading() || worldModel->GetGameActionState() == EGameActionState::Startup))
	{
		if (!actorSnapshot.empty())
		{
			actorSnapshot.clear();
			selectedActorIdx = -1;
			waitingForActors = false;
			cachedPlayer = 0;
		}
	}

	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Game Action State");

	if (worldModel)
	{
		auto* worldModel = camelClient->GetWorldModel();

		static std::vector<const char*> gameStateNames = []()
		{
			std::vector<const char*> result;
			constexpr auto names = magic_enum::enum_names<EGameActionState>();
			result.reserve(names.size());
			for (auto name : names)
			{
				result.push_back(name.data());
			}
			return result;
		}();

		int currentGameState = static_cast<int>(worldModel->GetGameActionState());

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.6f);
		if (ImGui::Combo("##GameActionStateCombo", &currentGameState, gameStateNames.data(),
		                 static_cast<int>(gameStateNames.size())))
		{
			worldModel->eGameActionState = static_cast<EGameActionState>(currentGameState);
		}
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "(Current: %s)",
		                   magic_enum::enum_name(worldModel->GetGameActionState()).data());
	}
	else
	{
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Engine Not Ready");
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Time of Day");

	if (worldModel && worldModel->pWorldState)
	{
		static std::vector<const char*> todNames = []()
		{
			std::vector<const char*> result;
			constexpr auto names = magic_enum::enum_names<ETimeOfDayType>();
			result.reserve(names.size());
			for (auto name : names)
			{
				result.push_back(name.data());
			}
			return result;
		}();

		int currentTOD = worldModel->pWorldState->timeOfDay;

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.6f);
		if (ImGui::Combo("##TimeOfDayCombo", &currentTOD, todNames.data(), static_cast<int>(todNames.size())))
		{
			worldModel->pWorldState->timeOfDay = static_cast<ETimeOfDayType>(currentTOD);
		}
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "(Current: %s)",
		                   magic_enum::enum_name(worldModel->pWorldState->timeOfDay).data());
	}
	else
	{
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "WorldState Not Available");
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Teleportation");

	static float targetPos[3] = {0.0f, 0.0f, 0.0f};
	ImGui::InputFloat3("Coordinates (X, Y, Z)", targetPos);

	if (ImGui::Button("Teleport to Coordinates", ImVec2(-1, 0)))
	{
		uintptr_t playerAddr = Actor::GetMainCharacter();
		if (playerAddr)
		{
			auto* player = reinterpret_cast<CClient_Actor*>(playerAddr);
			player->vPosition.myX = targetPos[0];
			player->vPosition.myY = targetPos[1];
			player->vPosition.myZ = targetPos[2];
		}
	}

	ImGui::Spacing();

	{
		if (cachedPlayer == 0 || !Actor::IsActorPlayerControlled(cachedPlayer))
			cachedPlayer = Actor::GetMainCharacter();

		if (cachedPlayer != 0)
		{
			auto pos = Actor::GetActorPosition(cachedPlayer);
			ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Current Position: (%.2f, %.2f)", pos.myX, pos.myY);
		}
		else
		{
			ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Waiting for player...");
		}
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Actor Inspector");

	if (ImGui::Button("Refresh Actor List"))
	{
		g_actorListRequested.store(true, std::memory_order_relaxed);
		waitingForActors = true;
	}

	if (waitingForActors && !g_actorListRequested.load(std::memory_order_relaxed))
	{
		std::lock_guard<std::mutex> lock(g_actorListMutex);
		actorSnapshot = g_actorList;
		if (selectedActorIdx >= static_cast<int>(actorSnapshot.size()))
			selectedActorIdx = -1;
		waitingForActors = false;
	}

	ImGui::SameLine();
	ImGui::Text("Actors: %d", static_cast<int>(actorSnapshot.size()));

	ImGui::Columns(2, "ActorListColumns", true);
	ImGui::SetColumnWidth(0, 200.0f);

	ImGui::BeginChild("ActorList", ImVec2(0, 300), true);
	for (int i = 0; i < static_cast<int>(actorSnapshot.size()); ++i)
	{
		char label[64];
		snprintf(label, sizeof(label), "Actor #%d", i);
		if (ImGui::Selectable(label, selectedActorIdx == i))
		{
			selectedActorIdx = i;
		}
	}
	ImGui::EndChild();

	ImGui::NextColumn();

	if (selectedActorIdx >= 0 && selectedActorIdx < static_cast<int>(actorSnapshot.size()))
	{
		uintptr_t actorPtr = actorSnapshot[selectedActorIdx].ptr;
		auto* actor = reinterpret_cast<CClient_Actor*>(actorPtr);

		ImGui::Text("Entity Properties: Actor #%d", selectedActorIdx);
		ImGui::Separator();

		ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "VTable: 0x%llX", actorSnapshot[selectedActorIdx].vtable);
		ImGui::Spacing();

		ImGui::Text("Position");
		float pos[3] = {actor->vPosition.myX, actor->vPosition.myY, actor->vPosition.myZ};
		if (ImGui::DragFloat3("##Pos", pos, 0.1f))
		{
			actor->vPosition.myX = pos[0];
			actor->vPosition.myY = pos[1];
			actor->vPosition.myZ = pos[2];
		}

		ImGui::Text("Rotation");
		float rot[3] = {actor->vRotation.myX, actor->vRotation.myY, actor->vRotation.myZ};
		if (ImGui::DragFloat3("##Rot", rot, 1.0f))
		{
			actor->vRotation.myX = rot[0];
			actor->vRotation.myY = rot[1];
			actor->vRotation.myZ = rot[2];
		}

		ImGui::Text("Scale");
		float scale[3] = {actor->vScale.myX, actor->vScale.myY, actor->vScale.myZ};
		if (ImGui::DragFloat3("##Scale", scale, 0.1f))
		{
			actor->vScale.myX = scale[0];
			actor->vScale.myY = scale[1];
			actor->vScale.myZ = scale[2];
		}

		ImGui::Spacing();

		if (ImGui::Button("Teleport To Actor", ImVec2(-1, 0)))
		{
			uintptr_t playerAddr = Actor::GetMainCharacter();
			if (playerAddr)
			{
				auto* player = reinterpret_cast<CClient_Actor*>(playerAddr);
				player->vPosition = actor->vPosition;
			}
		}

		if (ImGui::Button("Teleport To Me", ImVec2(-1, 0)))
		{
			uintptr_t playerAddr = Actor::GetMainCharacter();
			if (playerAddr)
			{
				auto* player = reinterpret_cast<CClient_Actor*>(playerAddr);
				actor->vPosition = player->vPosition;
			}
		}

		ImGui::Spacing();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.3f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
		if (ImGui::Button("Possess Actor", ImVec2(-1, 0)))
		{
			uintptr_t localPlayerComp = Actor::GetLocalPlayer();
			if (localPlayerComp)
			{
				*(uintptr_t*)(localPlayerComp + 0x50) = actorPtr;
				FRACTURE_MSG("Possessed actor at 0x{:X} (VTable: 0x{:X})", actorPtr,
				             actorSnapshot[selectedActorIdx].vtable);
			}
		}
		ImGui::PopStyleColor(3);
		ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "WARNING: This may crash the game!");
	}
	else
	{
		if (actorSnapshot.empty())
			ImGui::TextWrapped("Click 'Refresh Actor List' to populate the actor list.");
		else
			ImGui::TextWrapped("Select an actor from the list to view details.");
	}

	ImGui::Columns(1);
}

void drawCameraTab()
{
	ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Camera Controls");
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Camera Override");
	ImGui::Spacing();

	if (ImGui::Checkbox("Enable Camera Override", &bCameraOverrideEnabled))
	{
		if (!bCameraOverrideEnabled)
		{
			if (bFreecamEnabled)
				PatchFreecamUpdate(false);
			if (bFOVOverrideEnabled || bFreecamEnabled)
				PatchCameraUpdate(false);
			bFOVOverrideEnabled = false;
			bFreecamEnabled = false;
		}
	}

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.6f, 0.2f, 1.0f));
	ImGui::TextWrapped(
		"WARNING: Enabling camera override hooks the game's camera update function to prevent it from "
		"overwriting your changes every frame. This may cause visual glitches, broken cutscenes, or "
		"unstable camera behavior. Disable this if you experience issues.");
	ImGui::PopStyleColor();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Field of View");
	ImGui::Spacing();

	if (!bCameraOverrideEnabled)
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);

	{
		bool fovCheckbox = bFOVOverrideEnabled;
		if (ImGui::Checkbox("Override FOV", &fovCheckbox))
		{
			if (bCameraOverrideEnabled)
			{
				bFOVOverrideEnabled = fovCheckbox;
				PatchCameraUpdate(bFOVOverrideEnabled);
			}
		}
	}

	if (!bFOVOverrideEnabled || !bCameraOverrideEnabled)
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);

	{
		float fovVal = fCustomFOV;
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.7f);
		if (ImGui::SliderFloat("##FOVSlider", &fovVal, -20.0f, 300.0f, "%.1f"))
		{
			if (bCameraOverrideEnabled && bFOVOverrideEnabled)
				fCustomFOV = fovVal;
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##FOV"))
		{
			if (bCameraOverrideEnabled && bFOVOverrideEnabled)
				fCustomFOV = 120.0f;
		}
	}

	ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Current: %.1f", fCustomFOV);

	if (!bFOVOverrideEnabled || !bCameraOverrideEnabled)
		ImGui::PopStyleVar();

	if (!bCameraOverrideEnabled)
		ImGui::PopStyleVar();

	ImGui::TextWrapped("Adjusts the camera FOV.");

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Free Camera");
	ImGui::Spacing();

	if (!bCameraOverrideEnabled)
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);

	{
		bool freecamCheckbox = bFreecamEnabled;
		if (ImGui::Checkbox("Enable Freecam", &freecamCheckbox))
		{
			if (bCameraOverrideEnabled)
			{
				bFreecamEnabled = freecamCheckbox;
				PatchFreecamUpdate(bFreecamEnabled);
				PatchCameraUpdate(bFreecamEnabled || bFOVOverrideEnabled);
			}
		}
	}

	if (!bFreecamEnabled || !bCameraOverrideEnabled)
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);

	{
		float sensVal = fFreecamSensitivity;
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.7f);
		if (ImGui::SliderFloat("##SensSlider", &sensVal, 0.1f, 10.0f, "%.1f"))
		{
			if (bCameraOverrideEnabled && bFreecamEnabled)
				fFreecamSensitivity = sensVal;
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset##Sens"))
		{
			if (bCameraOverrideEnabled && bFreecamEnabled)
				fFreecamSensitivity = 1.0f;
		}
		ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Sensitivity: %.1f", fFreecamSensitivity);
	}

	ImGui::Spacing();
	ImGui::TextWrapped(
		"Detaches the camera from the player and allows free movement.\n"
		"Controls:\n"
		"  Arrow Left / Right - Move horizontally\n"
		"  Arrow Up / Down    - Move vertically\n"
		"  Scroll Wheel       - Adjust FOV");

	if (!bFreecamEnabled || !bCameraOverrideEnabled)
		ImGui::PopStyleVar();

	if (!bCameraOverrideEnabled)
		ImGui::PopStyleVar();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Camera Info");
	ImGui::Spacing();

	auto* camelClientCam = CamelClient::GetInstance();
	if (camelClientCam && camelClientCam->GetWorldModel() && camelClientCam->GetWorldModel()->pCameraManager)
	{
		auto* camMgr = camelClientCam->GetWorldModel()->pCameraManager;
		if (camMgr->pCamera)
		{
			auto* cam = camMgr->pCamera;

			ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", cam->position.myX, cam->position.myY, cam->position.myZ);
			ImGui::Text("Camera Lock: %s", cam->cameraLock ? "Locked" : "Unlocked");
		}
		else
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No active camera");
		}
	}
	else
	{
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Engine Not Ready");
	}
}

void drawOptionsTab()
{
	if (!g_loaderManager)
	{
		ImGui::Text("The loader manager is not initialized or not ready.");
		return;
	}

	ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Mod Loader Options");
	ImGui::Separator();

	ImGui::Text("Debug Logging:");
	ImGui::Spacing();
	static bool debugLogEnabled = g_loaderManager->getValue("debugLog", true);

	if (ImGui::Checkbox("Enable Debug Logging", &debugLogEnabled))
	{
		g_loaderManager->setValue("debugLog", debugLogEnabled);
		g_loaderManager->saveConfig();
	}

	ImGui::TextWrapped(
		"Enabling debug logging will output detailed information to help with troubleshooting and debugging mod loading issues. "
		"Disable it for better performance if you don't need debug information.");

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Menu Input Passthrough:");
	ImGui::Spacing();

	if (ImGui::Checkbox("Enable Input Passthrough", &bAllowInputPassThrough))
	{
		g_loaderManager->setValue("menuInputPassthrough", bAllowInputPassThrough);
		g_loaderManager->saveConfig();
	}

	ImGui::TextWrapped(
		"Enabling input passthrough allows mouse and keyboard input to pass through to the game while the menu is open. "
		"Disable it to block input while the menu is open, which can be useful for preventing accidental input.");

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Disable Menu:");
	ImGui::Spacing();

	if (ImGui::Checkbox("Disable FractureLoader Menu", &bNoMenu))
	{
		g_loaderManager->setValue("noMenu", bNoMenu);
		g_loaderManager->saveConfig();
	}

	ImGui::TextWrapped(
		"Disables the FractureLoader menu from rendering on the game, a restart is required after enabling."
		"To re-enable the FractureLoader menu, change the setting using the config.ini file on the FractureLoader folder.");

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	std::string keyDisplayName = getKeyDisplayName(menuToggleKey);

	ImGui::Text("Menu Toggle Key:");
	ImGui::SameLine();

	if (isCapturingKey)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.6f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.7f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));

		if (ImGui::Button("Press a key... (ESC to cancel)"))
		{
			isCapturingKey = false;
		}

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Waiting for key input...");
	}
	else
	{
		if (ImGui::Button(keyDisplayName.c_str()))
		{
			isCapturingKey = true;
		}
	}

	if (!isCapturingKey)
	{
		ImGui::TextWrapped(
			"Click the button above, then press the key you want to use to toggle the menu. Default is Insert.");
	}
	else
	{
		ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f),
		                   "Press any key to set as toggle key, or ESC to cancel.");
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f),
		                   "Modifier keys (Shift, Ctrl, Alt, Windows) are not allowed.");
	}

	ImGui::Separator();
	ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Game Options");
	ImGui::Separator();

	ImGui::Text("Skip Startup:");
	ImGui::Spacing();

	if (ImGui::Checkbox("Skip Startup Intro", &bSkipStartup))
	{
		g_loaderManager->setValue("skipStartup", bSkipStartup);
		g_loaderManager->saveConfig();
	}

	ImGui::TextWrapped(
		"Skips the startup intro sequence (logos/videos) and jumps straight to the press-start screen. "
		"Takes effect on the next game launch.");

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	static std::vector<const char*> platformCStr = []()
	{
		std::vector<const char*> result;
		constexpr auto names = magic_enum::enum_names<CClient_PlatformEnum>();
		result.reserve(names.size());
		for (auto name : names)
		{
			result.push_back(name.data());
		}
		return result;
	}();

	static int currentPlatformIndex = Global::g_currentPlatform;

	if (ImGui::Combo("Platform Type", &currentPlatformIndex, platformCStr.data(),
	                 static_cast<int>(platformCStr.size())))
	{
		Global::g_currentPlatform = static_cast<CClient_PlatformEnum>(currentPlatformIndex);

		g_loaderManager->setValue("spoofedPlatform", static_cast<int>(Global::g_currentPlatform));
		g_loaderManager->saveConfig();
	}

	ImGui::TextWrapped(
		"Selects which platform type the engine reports. "
		"This can influence platform-specific behavior, assets, or UI elements "
		"that the game loads (e.g., PC, console, or mobile)."
	);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Demo Type:");
	ImGui::Spacing();

	auto* camelOpt = CamelClient::GetInstance();
	auto* wmOpt = camelOpt ? camelOpt->GetWorldModel() : nullptr;

	if (wmOpt)
	{
		static std::vector<const char*> demoTypeCStr = []()
		{
			std::vector<const char*> result;
			constexpr auto names = magic_enum::enum_names<CClient_DemoType>();
			result.reserve(names.size());
			for (auto name : names)
			{
				result.push_back(name.data());
			}
			return result;
		}();

		int currentDemoType = wmOpt->eDemoType;

		if (ImGui::Combo("##DemoType", &currentDemoType, demoTypeCStr.data(),
		                 static_cast<int>(demoTypeCStr.size())))
		{
			wmOpt->eDemoType = static_cast<CClient_DemoType>(currentDemoType);
		}
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "(Current: %s)",
		                   magic_enum::enum_name(wmOpt->eDemoType).data());
	}
	else
	{
		ImGui::TextDisabled("WorldModel not available.");
	}

	ImGui::TextWrapped(
		"Changes the demo type reported by the engine's GetDemoType function. "
		"This controls demo-related restrictions and behavior. "
		"None is the default (full game).");

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	if (wmOpt)
	{
		auto pIsVIPDemo = reinterpret_cast<bool*>(0x1439DC0F4);
		bool bIsVIPDemo = *pIsVIPDemo;
		if (ImGui::Checkbox("Is VIP Demo", &bIsVIPDemo))
		{
			*pIsVIPDemo = bIsVIPDemo;
		}
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "(Current: %s)", bIsVIPDemo ? "true" : "false");

		ImGui::TextWrapped(
			"Toggles the VIP demo flag. Controls access to VIP demo-exclusive content and restrictions.");

		ImGui::Spacing();

		bool bIsTrial = false;
		bool* pTrialByte = nullptr;
		{
			auto* client = camelOpt->pClient;
			if (client)
			{
				uintptr_t obj1A8 = *reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(client) + 0x1A8);
				if (obj1A8)
				{
					uintptr_t obj28 = *reinterpret_cast<uintptr_t*>(obj1A8 + 0x28);
					if (obj28)
					{
						pTrialByte = reinterpret_cast<bool*>(obj28 + 0x6C9);
						bIsTrial = *pTrialByte;
					}
				}
			}
		}

		if (pTrialByte)
		{
			if (ImGui::Checkbox("Is Trial", &bIsTrial))
			{
				*pTrialByte = bIsTrial;
			}
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "(Current: %s)", bIsTrial ? "true" : "false");
		}
		else
		{
			ImGui::Text("Is Trial:");
			ImGui::SameLine();
			ImGui::TextDisabled("unavailable");
		}

		ImGui::TextWrapped(
			"Toggles the trial mode flag. Controls whether the game considers itself running as a trial version.");
	}
}

LRESULT __stdcall wndProc(const HWND hWnd, uint32_t message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		if (isCapturingKey && wParam != VK_ESCAPE)
		{
			if (wParam != VK_SHIFT && wParam != VK_CONTROL && wParam != VK_MENU &&
				wParam != VK_LWIN && wParam != VK_RWIN && wParam != VK_APPS &&
				wParam >= 8 && wParam < 256)
			{
				menuToggleKey = static_cast<UINT>(wParam);
				g_loaderManager->setValue("menuToggleKey", static_cast<int>(menuToggleKey));
				g_loaderManager->saveConfig();
				isCapturingKey = false;
			}
			return 0;
		}
		break;
	case WM_KEYUP:
		if (!isCapturingKey)
		{
			if (wParam == menuToggleKey)
			{
				bShow = !bShow;
				ImGui::GetIO().MouseDrawCursor = bShow;
			}
		}
		else if (wParam == VK_ESCAPE)
		{
			isCapturingKey = false;
		}
		break;
	case WM_SIZE:
		if (pDevice && wParam != SIZE_MINIMIZED)
		{
		}
		break;
	case WM_QUIT:
		if (bShow)
			ExitProcess(0);

		break;
	case WM_DESTROY:
		PostQuitMessage(0);
	}

	if (message == WM_MOUSEWHEEL && bCameraOverrideEnabled && bFreecamEnabled)
	{
		short wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		fScrollFOVDelta += (wheelDelta > 0) ? 5.0f : -5.0f;
	}

	if (bShow)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);

		if (!bAllowInputPassThrough)
			return TRUE;
	}

	return CallWindowProc(oWndProc, hWnd, message, wParam, lParam);
}

HRESULT (__stdcall*ResizeBuffersOriginal)(
	IDXGISwapChain* pSwapChain,
	UINT BufferCount,
	UINT Width,
	UINT Height,
	DXGI_FORMAT NewFormat,
	UINT SwapChainFlags
);

void cleanupRenderTarget()
{
	if (mainRenderTargetView)
	{
		mainRenderTargetView->Release();
		mainRenderTargetView = nullptr;
	}
}

void createRenderTarget(IDXGISwapChain* pSwapChain)
{
	ID3D11Texture2D* pBackBuffer = nullptr;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &mainRenderTargetView);
	pBackBuffer->Release();
}

HRESULT __stdcall hookResizeBuffers(
	IDXGISwapChain* pSwapChain,
	UINT BufferCount,
	UINT Width,
	UINT Height,
	DXGI_FORMAT NewFormat,
	UINT SwapChainFlags)
{
	if (pDevice != nullptr)
	{
		ImGui_ImplDX11_InvalidateDeviceObjects();
		cleanupRenderTarget();
	}

	HRESULT hr = ResizeBuffersOriginal(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

	if (pDevice != nullptr)
	{
		createRenderTarget(pSwapChain);
		ImGui_ImplDX11_CreateDeviceObjects();
	}

	return hr;
}


HRESULT WINAPI hookPresent(IDXGISwapChain* SwapChain, uint32_t Interval, uint32_t Flags)
{
	if (!bHasInit)
	{
		auto stat = SUCCEEDED(SwapChain->GetDevice(__uuidof(ID3D11Device), (PVOID*)&pDevice));
		if (stat)
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			SwapChain->GetDesc(&sd);
			wnd = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(wnd, GWLP_WNDPROC, (LONG_PTR)wndProc);
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

			ImFontConfig fontConfig;
			fontConfig.OversampleH = 3;
			fontConfig.OversampleV = 2;
			fontConfig.PixelSnapH = false;
			fontConfig.RasterizerMultiply = 1.0f;

			bool fontLoaded = false;
			const char* fontNames[] = {"Segoe UI", "Calibri", "Arial"};
			for (const char* fontName : fontNames)
			{
				HDC hdc = CreateCompatibleDC(nullptr);
				HFONT hFont = CreateFontA(-16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
				                          DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS,
				                          CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, fontName);
				if (hFont)
				{
					auto hOldFont = static_cast<HFONT>(SelectObject(hdc, hFont));
					DWORD dataSize = GetFontData(hdc, 0, 0, nullptr, 0);
					if (dataSize != GDI_ERROR && dataSize > 0)
					{
						void* fontData = IM_ALLOC(dataSize);
						GetFontData(hdc, 0, 0, fontData, dataSize);
						io.Fonts->AddFontFromMemoryTTF(fontData, dataSize, 16.0f, &fontConfig);
						fontLoaded = true;
					}
					SelectObject(hdc, hOldFont);
					DeleteObject(hFont);
				}
				DeleteDC(hdc);
				if (fontLoaded) break;
			}

			if (!fontLoaded)
			{
				fontConfig.SizePixels = 15.0f;
				io.Fonts->AddFontDefault(&fontConfig);
			}

			io.Fonts->Build();

			ImGui_ImplWin32_Init(wnd);
			ImGui_ImplDX11_Init(pDevice, pContext);

			setupImGuiStyle();

			menuToggleKey = g_loaderManager->getValue<int>("menuToggleKey", VK_INSERT);
			bNoMenu = g_loaderManager->getValue<bool>("noMenu", false); // useless because this wouldnt even get called if its true but anyways
			bAllowInputPassThrough = g_loaderManager->getValue<bool>("menuInputPassthrough", false);
			bSkipStartup = g_loaderManager->getValue<bool>("skipStartup", false);
			bHasInit = true;
		}

		else return PresentOriginal(SwapChain, Interval, Flags);
	}

	static bool bStartupSkipped = false;
	if (bSkipStartup && !bStartupSkipped)
	{
		auto* camel = CamelClient::GetInstance();
		auto* wm = camel ? camel->GetWorldModel() : nullptr;
		if (wm)
		{
			auto state = wm->GetGameActionState();
			if (state == EGameActionState::Startup ||
				state == EGameActionState::Legal3rdParty)
			{
				wm->eGameActionState = EGameActionState::PressStart;
				bStartupSkipped = true;
			}
		}
	}

	UpdateCameraOverrides();

	if (bShow)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();
		ImGui::SetNextWindowBgAlpha(0.8f);
		ImGui::SetNextWindowSize(ImVec2(750, 500));

		static std::string windowTitle = "FractureLoader v" + Global::fractureLoaderVersion;
		ImGui::Begin(windowTitle.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

		if (ImGui::BeginTabBar(""))
		{
			if (ImGui::BeginTabItem("Home"))
			{
				drawHomeTab();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Faceman Layout"))
			{
				drawFacemanTab();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("UI Graph"))
			{
				drawUIGraphTab();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Mods"))
			{
				drawModsTab();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("VFS"))
			{
				drawVFSTab();
				ImGui::EndTabItem();
			}
			//if (ImGui::BeginTabItem("Ally Swap")) { drawAllySwapTab(); ImGui::EndTabItem(); }
			if (ImGui::BeginTabItem("World"))
			{
				drawWorldTab();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Camera"))
			{
				drawCameraTab();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Options"))
			{
				drawOptionsTab();
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}

		ImGui::End();
		ImGui::Render();

		pContext->OMSetRenderTargets(1, &mainRenderTargetView, nullptr);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
	return PresentOriginal(SwapChain, Interval, Flags);
}

DWORD WINAPI guiHook(LPVOID)
{
	bool bHooked = false;
	while (!bHooked)
	{
		auto status = kiero::init(kiero::RenderType::D3D11);
		if (status == kiero::Status::Success)
		{
			kiero::bind(8, (PVOID*)&PresentOriginal, hookPresent);
			kiero::bind(13, (void**)&ResizeBuffersOriginal, hookResizeBuffers);
			bHooked = true;
		}

		Sleep(100);
	}

	FRACTURE_DEBUG("GUI Initialized Successfully.");

	return 0;
}
