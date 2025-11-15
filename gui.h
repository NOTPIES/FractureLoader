#pragma once

#include <Windows.h>
#include <dxgi.h>
#include <d3d11.h>

#include "global.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "includes.h"

#include "kiero/kiero.h"
#include <filesystem>
#include "git_commit.h"

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
static bool bHasInit = false;
static bool bShow = true;

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
	style.Alpha = 1.0f;
	style.WindowPadding = ImVec2(10, 10);
	style.WindowRounding = 6.0f;
	style.WindowBorderSize = 1.0f;
	style.WindowMinSize = ImVec2(160, 20);
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_Right;
	style.ChildRounding = 6.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 4.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(8, 4);
	style.FrameRounding = 4.0f;
	style.FrameBorderSize = 1.0f;
	style.ItemSpacing = ImVec2(8, 6);
	style.ItemInnerSpacing = ImVec2(4, 4);
	style.IndentSpacing = 20.0f;
	style.ScrollbarSize = 14.0f;
	style.ScrollbarRounding = 12.0f;
	style.GrabMinSize = 12.0f;
	style.GrabRounding = 4.0f;
	style.TabRounding = 4.0f;
	style.TabBorderSize = 1.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	auto bgColor = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
	auto highlight = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	auto midlight = ImVec4(0.20f, 0.21f, 0.25f, 1.00f);

	auto white = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	auto disabled = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

	ImVec4* colors = style.Colors;
	colors[ImGuiCol_Text] = white;
	colors[ImGuiCol_TextDisabled] = disabled;
	colors[ImGuiCol_WindowBg] = bgColor;
	colors[ImGuiCol_ChildBg] = bgColor;
	colors[ImGuiCol_PopupBg] = midlight;
	colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = midlight;
	colors[ImGuiCol_FrameBgHovered] = highlight;
	colors[ImGuiCol_FrameBgActive] = highlight;
	colors[ImGuiCol_TitleBg] = bgColor;
	colors[ImGuiCol_TitleBgActive] = highlight;
	colors[ImGuiCol_TitleBgCollapsed] = midlight;
	colors[ImGuiCol_MenuBarBg] = midlight;
	colors[ImGuiCol_ScrollbarBg] = midlight;
	colors[ImGuiCol_ScrollbarGrab] = highlight;
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.8f);
	colors[ImGuiCol_ScrollbarGrabActive] = highlight;
	colors[ImGuiCol_CheckMark] = highlight;
	colors[ImGuiCol_SliderGrab] = highlight;
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.72f, 1.00f, 1.00f);
	colors[ImGuiCol_Button] = midlight;
	colors[ImGuiCol_ButtonHovered] = highlight;
	colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	colors[ImGuiCol_Header] = midlight;
	colors[ImGuiCol_HeaderHovered] = highlight;
	colors[ImGuiCol_HeaderActive] = highlight;
	colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = highlight;
	colors[ImGuiCol_SeparatorActive] = highlight;
	colors[ImGuiCol_ResizeGrip] = highlight;
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.8f);
	colors[ImGuiCol_ResizeGripActive] = highlight;
	colors[ImGuiCol_Tab] = midlight;
	colors[ImGuiCol_TabHovered] = highlight;
	colors[ImGuiCol_TabActive] = highlight;
	colors[ImGuiCol_TabUnfocused] = bgColor;
	colors[ImGuiCol_TabUnfocusedActive] = midlight;
	colors[ImGuiCol_PlotLines] = white;
	colors[ImGuiCol_PlotLinesHovered] = highlight;
	colors[ImGuiCol_PlotHistogram] = highlight;
	colors[ImGuiCol_PlotHistogramHovered] = highlight;
	colors[ImGuiCol_TextSelectedBg] = highlight;
	colors[ImGuiCol_DragDropTarget] = highlight;
	colors[ImGuiCol_NavHighlight] = highlight;
	colors[ImGuiCol_NavWindowingHighlight] = highlight;
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.51f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.51f);
}

void textCentered(std::string text)
{
	auto windowWidth = ImGui::GetWindowSize().x;
	auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

	ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
	ImGui::Text(text.c_str());
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

			ImGui_ImplWin32_Init(wnd);
			ImGui_ImplDX11_Init(pDevice, pContext);

			setupImGuiStyle();

			menuToggleKey = g_loaderManager->getValue<int>("menuToggleKey", VK_INSERT);
			bAllowInputPassThrough = g_loaderManager->getValue<bool>("menuInputPassthrough", false);
			bHasInit = true;
		}

		else return PresentOriginal(SwapChain, Interval, Flags);
	}

	if (bShow)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();
		ImGui::SetNextWindowBgAlpha(0.8f);
		ImGui::SetNextWindowSize(ImVec2(560, 345));

		static std::string windowTitle = "FractureLoader v" + Global::fractureLoaderVersion;
		ImGui::Begin(windowTitle.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

		static int Tab = 0;
		static char g_layoutPath[256] = "faceman/layouts/Camel Town Viewer.layout";
		static char g_UIGraphPath[256] = "camel/ui/graphs/fpp_startmenu.muigraph";
		static __int64 g_uiTestHandle = 0;

		if (ImGui::BeginTabBar(""))
		{
			if (ImGui::BeginTabItem(("Home")))
			{
				Tab = 0;
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

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(("Faceman Layout")))
			{
				Tab = 1;
				ImGui::InputText("##LayoutPath", g_layoutPath, sizeof(g_layoutPath));
				ImGui::SameLine();
				if (ImGui::Button("Load Layout"))
				{
					LoadFacemanUILayout(g_layoutPath, true, nullptr);
				}

				ImGui::NewLine();

				ImGui::Separator();
				ImGui::TextWrapped(
					"Faceman layouts are inspectors used on the Snowdrop Engine editor, most of these layouts features will not work on the Retail version of the game due to stripped code.");
				ImGui::Spacing();
				ImGui::TextWrapped(
					"Note: The Faceman layout loading feature is experimental and may not work as expected. It is intended for testing purposes only.");

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(("UI Graph")))
			{
				Tab = 2;
				ImGui::InputText("##UIGraphPath", g_UIGraphPath, sizeof(g_UIGraphPath));
				ImGui::SameLine();
				if (ImGui::Button("Load UI Layout"))
				{
					__int64 newHandle = 0;
					__int64* result = LoadUI(&newHandle, g_UIGraphPath);

					if (result)
					{
						__int64 oldHandle = g_uiTestHandle;
						g_uiTestHandle = *result;
						*result = 0;

						if (oldHandle)
							ReleaseResourceRef(oldHandle);

						FRACTURE_DEBUG("[LoadUI] Successfully loaded UI resource with handle: 0x{:X}", g_uiTestHandle);

						void* obj = Alloc(0x78);
						if (!obj)
						{
							FRACTURE_DEBUG("Allocation failed");
						}
						else
						{
							FRACTURE_DEBUG("[LoadUI] Allocated object memory at: 0x{:X}", obj);

							auto res = InitUIObj((__int64)obj, g_uiTestHandle);
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
								printf("[LoadUI] qword_143AD9E68 is null or invalid\n");
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

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(("Mods")))
			{
				Tab = 3;

				if (g_modLoader == nullptr)
				{
					ImGui::Text("The mod loader is not initialized or not ready.");
					ImGui::EndTabItem();
					goto endMods;
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

					ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.2f, 1.0f), "%s", mod.name);
					ImGui::Text("Author: %s", mod.author);
					ImGui::Text("Version: %s", mod.version);

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

				ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 150.0f) * 0.5f);
				if (ImGui::Button("Open Mods Folder", ImVec2(150.0f, 0)))
				{
					ShellExecuteA(nullptr, "open", g_loaderManager->getModsDirectory().c_str(), nullptr, nullptr,
					              SW_SHOWDEFAULT);
				}

				ImGui::EndTabItem();
			}

		endMods:

			static char resourcePath[256] = "camel/gameroot.juice";
			static bool testResult = false;
			static bool testPerformed = false;

			if (ImGui::BeginTabItem("Resources"))
			{
				Tab = 4;

				ImGui::Text("Resource Management");
				ImGui::Separator();
				ImGui::TextWrapped(
					"This tab is for testing resource loading using engine functions, useful to test if custom or overridden assets are loading correctly.");
				ImGui::Spacing();

				ImGui::InputText("Resource Path", resourcePath, sizeof(resourcePath));

				if (ImGui::Button("Test Resource Availability"))
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

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Ally Swap"))
			{
				Tab = 5;

				ImGui::Text("Ally Swap");
				ImGui::Separator();
				ImGui::TextWrapped("This tab is for swapping allies in your current party.");
				ImGui::Spacing();

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(("Options")))
			{
				Tab = 6;

				if (!g_loaderManager)
				{
					ImGui::Text("The loader manager is not initialized or not ready.");
					ImGui::EndTabItem();
					goto endOptions;
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

				// TODO for 1.0.1
				/*

				ImGui::Text("Mod Loading Method:");
				ImGui::Spacing();

				static int selectedMethod = g_loaderManager->getValue("modLoadingMethod", 1);

				if (ImGui::RadioButton("Method 1: File Override Hook (Optimized)", selectedMethod == 0))
					selectedMethod = 1;

				if (ImGui::RadioButton("Method 2: File Override Hook", selectedMethod == 0))
					selectedMethod = 2;

				ImGui::Spacing();

				ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Method Info:");
				ImGui::Separator();

				if (selectedMethod == 1)
				{
					ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Method 1: File Override Hook (Optimized)");
					ImGui::TextWrapped("This method maps all the mod folders and stores them to give them to the engine by intercepting file loading operations,"
						"This is faster than Method 2 because it finds the modified game files at startup and just gives the engine the path of where these files are, unlike method 2 which finds the files when the engine requests them (at runtime).");
				}
				else if (selectedMethod == 2)
				{
					ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Method 2: File Override Hook");
					ImGui::TextWrapped("This method intercepts file loading operations and redirects them to mod files when available. "
						"It provides real-time file replacement, finding the mod folders at RUNTIME (slower) and then cached. "
						"Files are loaded with priority based on mod load order (last loaded mod has highest priority).");

					ImGui::Spacing();
				}

				ImGui::Spacing();
				ImGui::Spacing();

				if (ImGui::Button("Apply Method Settings"))
				{
					g_loaderManager->setValue("modLoadingMethod", selectedMethod);
					g_loaderManager->saveConfig();
				}*/

				ImGui::Separator();
				ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Game Options");
				ImGui::Separator();

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

				ImGui::EndTabItem();
			}

		endOptions:

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
