#include "uplay_proxy.h"
#include <iostream>
#include <string>
#include <filesystem>

#define ORIGINAL_DLL_NAME L"uplay_r1_loader64_orig.dll"

HMODULE g_originalDLL = nullptr;
bool g_dllInjected = false;

static UPLAY_ACH_GetAchievements_t orig_UPLAY_ACH_GetAchievements = nullptr;
static UPLAY_ACH_ReleaseAchievementList_t orig_UPLAY_ACH_ReleaseAchievementList = nullptr;
static UPLAY_USER_IsOwned_t orig_UPLAY_USER_IsOwned = nullptr;
static UPLAY_GetLastError_t orig_UPLAY_GetLastError = nullptr;
static UPLAY_USER_IsInOfflineMode_t orig_UPLAY_USER_IsInOfflineMode = nullptr;
static UPLAY_USER_IsConnected_t orig_UPLAY_USER_IsConnected = nullptr;
static UPLAY_HasOverlappedOperationCompleted_t orig_UPLAY_HasOverlappedOperationCompleted = nullptr;
static UPLAY_GetOverlappedOperationResult_t orig_UPLAY_GetOverlappedOperationResult = nullptr;
static UPLAY_SAVE_GetSavegames_t orig_UPLAY_SAVE_GetSavegames = nullptr;
static UPLAY_SAVE_ReleaseGameList_t orig_UPLAY_SAVE_ReleaseGameList = nullptr;
static UPLAY_SAVE_Open_t orig_UPLAY_SAVE_Open = nullptr;
static UPLAY_SAVE_Close_t orig_UPLAY_SAVE_Close = nullptr;
static UPLAY_SAVE_Read_t orig_UPLAY_SAVE_Read = nullptr;
static UPLAY_SAVE_Write_t orig_UPLAY_SAVE_Write = nullptr;
static UPLAY_SAVE_SetName_t orig_UPLAY_SAVE_SetName = nullptr;
static UPLAY_SAVE_Remove_t orig_UPLAY_SAVE_Remove = nullptr;
static UPLAY_FRIENDS_Init_t orig_UPLAY_FRIENDS_Init = nullptr;
static UPLAY_PARTY_Init_t orig_UPLAY_PARTY_Init = nullptr;
static UPLAY_Start_t orig_UPLAY_Start = nullptr;
static UPLAY_Quit_t orig_UPLAY_Quit = nullptr;
static UPLAY_AVATAR_Get_t orig_UPLAY_AVATAR_Get = nullptr;
static UPLAY_AVATAR_Release_t orig_UPLAY_AVATAR_Release = nullptr;
static UPLAY_USER_GetAccountIdUtf8_t orig_UPLAY_USER_GetAccountIdUtf8 = nullptr;
static UPLAY_USER_GetEmailUtf8_t orig_UPLAY_USER_GetEmailUtf8 = nullptr;
static UPLAY_USER_GetNameUtf8_t orig_UPLAY_USER_GetNameUtf8 = nullptr;
static UPLAY_USER_GetTicketUtf8_t orig_UPLAY_USER_GetTicketUtf8 = nullptr;
static UPLAY_GetNextEvent_t orig_UPLAY_GetNextEvent = nullptr;
static UPLAY_Update_t orig_UPLAY_Update = nullptr;
static UPLAY_ACH_EarnAchievement_t orig_UPLAY_ACH_EarnAchievement = nullptr;
static UPLAY_OVERLAY_Show_t orig_UPLAY_OVERLAY_Show = nullptr;
static UPLAY_OVERLAY_SetShopUrl_t orig_UPLAY_OVERLAY_SetShopUrl = nullptr;
static UPLAY_FRIENDS_GetFriendList_t orig_UPLAY_FRIENDS_GetFriendList = nullptr;
static UPLAY_USER_SetGameSession_t orig_UPLAY_USER_SetGameSession = nullptr;
static UPLAY_USER_ClearGameSession_t orig_UPLAY_USER_ClearGameSession = nullptr;
static UPLAY_FRIENDS_InviteToGame_t orig_UPLAY_FRIENDS_InviteToGame = nullptr;
static UPLAY_PARTY_GetFullMemberList_t orig_UPLAY_PARTY_GetFullMemberList = nullptr;
static UPLAY_PARTY_IsInParty_t orig_UPLAY_PARTY_IsInParty = nullptr;
static UPLAY_WIN_GetRewards_t orig_UPLAY_WIN_GetRewards = nullptr;
static UPLAY_WIN_ReleaseRewardList_t orig_UPLAY_WIN_ReleaseRewardList = nullptr;
static UPLAY_WIN_RefreshActions_t orig_UPLAY_WIN_RefreshActions = nullptr;
static UPLAY_OVERLAY_ShowShopUrl_t orig_UPLAY_OVERLAY_ShowShopUrl = nullptr;

std::wstring GetCurrentDirectory()
{
	wchar_t buffer[MAX_PATH];
	GetModuleFileNameW(nullptr, buffer, MAX_PATH);
	std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
	return std::wstring(buffer).substr(0, pos);
}

bool loadOriginalDll()
{
	if (g_originalDLL != nullptr)
	{
		return true;
	}

	std::wstring currentDir = GetCurrentDirectory();
	std::wstring dllPath = currentDir + L"\\" + ORIGINAL_DLL_NAME;

	g_originalDLL = LoadLibraryW(dllPath.c_str());
	if (g_originalDLL == nullptr)
	{
		// try loading from system directory (very unlikely but we will still try)
		g_originalDLL = LoadLibraryW(ORIGINAL_DLL_NAME);
	}

	if (g_originalDLL == nullptr)
	{
		MessageBoxW(
			nullptr,
			L"Failed to load uplay_r1_loader64.dll proxy, make sure uplay_r1_loader64_orig.dll exists on the game folder!",
			L"FractureLoader", MB_OK | MB_ICONERROR);
		return false;
	}

	orig_UPLAY_ACH_GetAchievements = (UPLAY_ACH_GetAchievements_t)GetProcAddress(
		g_originalDLL, "UPLAY_ACH_GetAchievements");
	orig_UPLAY_ACH_ReleaseAchievementList = (UPLAY_ACH_ReleaseAchievementList_t)GetProcAddress(
		g_originalDLL, "UPLAY_ACH_ReleaseAchievementList");
	orig_UPLAY_USER_IsOwned = (UPLAY_USER_IsOwned_t)GetProcAddress(g_originalDLL, "UPLAY_USER_IsOwned");
	orig_UPLAY_GetLastError = (UPLAY_GetLastError_t)GetProcAddress(g_originalDLL, "UPLAY_GetLastError");
	orig_UPLAY_USER_IsInOfflineMode = (UPLAY_USER_IsInOfflineMode_t)GetProcAddress(
		g_originalDLL, "UPLAY_USER_IsInOfflineMode");
	orig_UPLAY_USER_IsConnected = (UPLAY_USER_IsConnected_t)GetProcAddress(g_originalDLL, "UPLAY_USER_IsConnected");
	orig_UPLAY_HasOverlappedOperationCompleted = (UPLAY_HasOverlappedOperationCompleted_t)GetProcAddress(
		g_originalDLL, "UPLAY_HasOverlappedOperationCompleted");
	orig_UPLAY_GetOverlappedOperationResult = (UPLAY_GetOverlappedOperationResult_t)GetProcAddress(
		g_originalDLL, "UPLAY_GetOverlappedOperationResult");
	orig_UPLAY_SAVE_GetSavegames = (UPLAY_SAVE_GetSavegames_t)GetProcAddress(g_originalDLL, "UPLAY_SAVE_GetSavegames");
	orig_UPLAY_SAVE_ReleaseGameList = (UPLAY_SAVE_ReleaseGameList_t)GetProcAddress(
		g_originalDLL, "UPLAY_SAVE_ReleaseGameList");
	orig_UPLAY_SAVE_Open = (UPLAY_SAVE_Open_t)GetProcAddress(g_originalDLL, "UPLAY_SAVE_Open");
	orig_UPLAY_SAVE_Close = (UPLAY_SAVE_Close_t)GetProcAddress(g_originalDLL, "UPLAY_SAVE_Close");
	orig_UPLAY_SAVE_Read = (UPLAY_SAVE_Read_t)GetProcAddress(g_originalDLL, "UPLAY_SAVE_Read");
	orig_UPLAY_SAVE_Write = (UPLAY_SAVE_Write_t)GetProcAddress(g_originalDLL, "UPLAY_SAVE_Write");
	orig_UPLAY_SAVE_SetName = (UPLAY_SAVE_SetName_t)GetProcAddress(g_originalDLL, "UPLAY_SAVE_SetName");
	orig_UPLAY_SAVE_Remove = (UPLAY_SAVE_Remove_t)GetProcAddress(g_originalDLL, "UPLAY_SAVE_Remove");
	orig_UPLAY_FRIENDS_Init = (UPLAY_FRIENDS_Init_t)GetProcAddress(g_originalDLL, "UPLAY_FRIENDS_Init");
	orig_UPLAY_PARTY_Init = (UPLAY_PARTY_Init_t)GetProcAddress(g_originalDLL, "UPLAY_PARTY_Init");
	orig_UPLAY_Start = (UPLAY_Start_t)GetProcAddress(g_originalDLL, "UPLAY_Start");
	orig_UPLAY_Quit = (UPLAY_Quit_t)GetProcAddress(g_originalDLL, "UPLAY_Quit");
	orig_UPLAY_AVATAR_Get = (UPLAY_AVATAR_Get_t)GetProcAddress(g_originalDLL, "UPLAY_AVATAR_Get");
	orig_UPLAY_AVATAR_Release = (UPLAY_AVATAR_Release_t)GetProcAddress(g_originalDLL, "UPLAY_AVATAR_Release");
	orig_UPLAY_USER_GetAccountIdUtf8 = (UPLAY_USER_GetAccountIdUtf8_t)GetProcAddress(
		g_originalDLL, "UPLAY_USER_GetAccountIdUtf8");
	orig_UPLAY_USER_GetEmailUtf8 = (UPLAY_USER_GetEmailUtf8_t)GetProcAddress(g_originalDLL, "UPLAY_USER_GetEmailUtf8");
	orig_UPLAY_USER_GetNameUtf8 = (UPLAY_USER_GetNameUtf8_t)GetProcAddress(g_originalDLL, "UPLAY_USER_GetNameUtf8");
	orig_UPLAY_USER_GetTicketUtf8 = (UPLAY_USER_GetTicketUtf8_t)GetProcAddress(
		g_originalDLL, "UPLAY_USER_GetTicketUtf8");
	orig_UPLAY_GetNextEvent = (UPLAY_GetNextEvent_t)GetProcAddress(g_originalDLL, "UPLAY_GetNextEvent");
	orig_UPLAY_Update = (UPLAY_Update_t)GetProcAddress(g_originalDLL, "UPLAY_Update");
	orig_UPLAY_ACH_EarnAchievement = (UPLAY_ACH_EarnAchievement_t)GetProcAddress(
		g_originalDLL, "UPLAY_ACH_EarnAchievement");
	orig_UPLAY_OVERLAY_Show = (UPLAY_OVERLAY_Show_t)GetProcAddress(g_originalDLL, "UPLAY_OVERLAY_Show");
	orig_UPLAY_OVERLAY_SetShopUrl = (UPLAY_OVERLAY_SetShopUrl_t)GetProcAddress(
		g_originalDLL, "UPLAY_OVERLAY_SetShopUrl");
	orig_UPLAY_FRIENDS_GetFriendList = (UPLAY_FRIENDS_GetFriendList_t)GetProcAddress(
		g_originalDLL, "UPLAY_FRIENDS_GetFriendList");
	orig_UPLAY_USER_SetGameSession = (UPLAY_USER_SetGameSession_t)GetProcAddress(
		g_originalDLL, "UPLAY_USER_SetGameSession");
	orig_UPLAY_USER_ClearGameSession = (UPLAY_USER_ClearGameSession_t)GetProcAddress(
		g_originalDLL, "UPLAY_USER_ClearGameSession");
	orig_UPLAY_FRIENDS_InviteToGame = (UPLAY_FRIENDS_InviteToGame_t)GetProcAddress(
		g_originalDLL, "UPLAY_FRIENDS_InviteToGame");
	orig_UPLAY_PARTY_GetFullMemberList = (UPLAY_PARTY_GetFullMemberList_t)GetProcAddress(
		g_originalDLL, "UPLAY_PARTY_GetFullMemberList");
	orig_UPLAY_PARTY_IsInParty = (UPLAY_PARTY_IsInParty_t)GetProcAddress(g_originalDLL, "UPLAY_PARTY_IsInParty");
	orig_UPLAY_WIN_GetRewards = (UPLAY_WIN_GetRewards_t)GetProcAddress(g_originalDLL, "UPLAY_WIN_GetRewards");
	orig_UPLAY_WIN_ReleaseRewardList = (UPLAY_WIN_ReleaseRewardList_t)GetProcAddress(
		g_originalDLL, "UPLAY_WIN_ReleaseRewardList");
	orig_UPLAY_WIN_RefreshActions = (UPLAY_WIN_RefreshActions_t)GetProcAddress(
		g_originalDLL, "UPLAY_WIN_RefreshActions");
	orig_UPLAY_OVERLAY_ShowShopUrl = (UPLAY_OVERLAY_ShowShopUrl_t)GetProcAddress(
		g_originalDLL, "UPLAY_OVERLAY_ShowShopUrl");

	return true;
}

void unLoadOriginalDLL()
{
	if (g_originalDLL != nullptr)
	{
		FreeLibrary(g_originalDLL);
		g_originalDLL = nullptr;
	}
}

void initializeUPlayProxy()
{
	if (!loadOriginalDll())
	{
	}
}

extern "C" {
__declspec(dllexport) int UPLAY_ACH_GetAchievements(void* a, void* b, void* c)
{
	if (orig_UPLAY_ACH_GetAchievements) return orig_UPLAY_ACH_GetAchievements(a, b, c);
	return 0;
}

__declspec(dllexport) int UPLAY_ACH_ReleaseAchievementList(void* a)
{
	if (orig_UPLAY_ACH_ReleaseAchievementList) return orig_UPLAY_ACH_ReleaseAchievementList(a);
	return 0;
}

__declspec(dllexport) int UPLAY_USER_IsOwned(void* a, void* b)
{
	if (orig_UPLAY_USER_IsOwned) return orig_UPLAY_USER_IsOwned(a, b);
	return 0;
}

__declspec(dllexport) int UPLAY_GetLastError(void* a)
{
	if (orig_UPLAY_GetLastError) return orig_UPLAY_GetLastError(a);
	return 0;
}

__declspec(dllexport) int UPLAY_USER_IsInOfflineMode(void* a)
{
	if (orig_UPLAY_USER_IsInOfflineMode) return orig_UPLAY_USER_IsInOfflineMode(a);
	return 0;
}

__declspec(dllexport) int UPLAY_USER_IsConnected(void* a)
{
	if (orig_UPLAY_USER_IsConnected) return orig_UPLAY_USER_IsConnected(a);
	return 0;
}

__declspec(dllexport) int UPLAY_HasOverlappedOperationCompleted(void* a, void* b)
{
	if (orig_UPLAY_HasOverlappedOperationCompleted) return orig_UPLAY_HasOverlappedOperationCompleted(a, b);
	return 0;
}

__declspec(dllexport) int UPLAY_GetOverlappedOperationResult(void* a, void* b)
{
	if (orig_UPLAY_GetOverlappedOperationResult) return orig_UPLAY_GetOverlappedOperationResult(a, b);
	return 0;
}

__declspec(dllexport) int UPLAY_SAVE_GetSavegames(void* a, void* b)
{
	if (orig_UPLAY_SAVE_GetSavegames) return orig_UPLAY_SAVE_GetSavegames(a, b);
	return 0;
}

__declspec(dllexport) int UPLAY_SAVE_ReleaseGameList(void* a)
{
	if (orig_UPLAY_SAVE_ReleaseGameList) return orig_UPLAY_SAVE_ReleaseGameList(a);
	return 0;
}

__declspec(dllexport) int UPLAY_SAVE_Open(void* a, void* b, void* c, void* d)
{
	if (orig_UPLAY_SAVE_Open) return orig_UPLAY_SAVE_Open(a, b, c, d);
	return 0;
}

__declspec(dllexport) int UPLAY_SAVE_Close(void* a)
{
	if (orig_UPLAY_SAVE_Close) return orig_UPLAY_SAVE_Close(a);
	return 0;
}

__declspec(dllexport) int UPLAY_SAVE_Read(void* a, void* b, void* c, void* d)
{
	if (orig_UPLAY_SAVE_Read) return orig_UPLAY_SAVE_Read(a, b, c, d);
	return 0;
}

__declspec(dllexport) int UPLAY_SAVE_Write(void* a, void* b, void* c)
{
	if (orig_UPLAY_SAVE_Write) return orig_UPLAY_SAVE_Write(a, b, c);
	return 0;
}

__declspec(dllexport) int UPLAY_SAVE_SetName(void* a, void* b)
{
	if (orig_UPLAY_SAVE_SetName) return orig_UPLAY_SAVE_SetName(a, b);
	return 0;
}

__declspec(dllexport) int UPLAY_SAVE_Remove(void* a)
{
	if (orig_UPLAY_SAVE_Remove) return orig_UPLAY_SAVE_Remove(a);
	return 0;
}

__declspec(dllexport) int UPLAY_FRIENDS_Init(void* a)
{
	if (orig_UPLAY_FRIENDS_Init) return orig_UPLAY_FRIENDS_Init(a);
	return 0;
}

__declspec(dllexport) int UPLAY_PARTY_Init(void* a)
{
	if (orig_UPLAY_PARTY_Init) return orig_UPLAY_PARTY_Init(a);
	return 0;
}

__declspec(dllexport) int UPLAY_Start(void* a, void* b)
{
	if (orig_UPLAY_Start) return orig_UPLAY_Start(a, b);
	return 0;
}

__declspec(dllexport) int UPLAY_Quit(void)
{
	if (orig_UPLAY_Quit) return orig_UPLAY_Quit();
	return 0;
}

__declspec(dllexport) int UPLAY_AVATAR_Get(void* a, void* b, void* c)
{
	if (orig_UPLAY_AVATAR_Get) return orig_UPLAY_AVATAR_Get(a, b, c);
	return 0;
}

__declspec(dllexport) int UPLAY_AVATAR_Release(void* a)
{
	if (orig_UPLAY_AVATAR_Release) return orig_UPLAY_AVATAR_Release(a);
	return 0;
}

__declspec(dllexport) int UPLAY_USER_GetAccountIdUtf8(void* a, void* b)
{
	if (orig_UPLAY_USER_GetAccountIdUtf8) return orig_UPLAY_USER_GetAccountIdUtf8(a, b);
	return 0;
}

__declspec(dllexport) int UPLAY_USER_GetEmailUtf8(void* a, void* b)
{
	if (orig_UPLAY_USER_GetEmailUtf8) return orig_UPLAY_USER_GetEmailUtf8(a, b);
	return 0;
}

__declspec(dllexport) int UPLAY_USER_GetNameUtf8(void* a, void* b)
{
	if (orig_UPLAY_USER_GetNameUtf8) return orig_UPLAY_USER_GetNameUtf8(a, b);
	return 0;
}

__declspec(dllexport) int UPLAY_USER_GetTicketUtf8(void* a, void* b)
{
	if (orig_UPLAY_USER_GetTicketUtf8) return orig_UPLAY_USER_GetTicketUtf8(a, b);
	return 0;
}

__declspec(dllexport) int UPLAY_GetNextEvent(void* a)
{
	if (orig_UPLAY_GetNextEvent) return orig_UPLAY_GetNextEvent(a);
	return 0;
}

__declspec(dllexport) int UPLAY_Update(void)
{
	if (orig_UPLAY_Update) return orig_UPLAY_Update();
	return 0;
}

__declspec(dllexport) int UPLAY_ACH_EarnAchievement(void* a, void* b)
{
	if (orig_UPLAY_ACH_EarnAchievement) return orig_UPLAY_ACH_EarnAchievement(a, b);
	return 0;
}

__declspec(dllexport) int UPLAY_OVERLAY_Show(void* a, void* b)
{
	if (orig_UPLAY_OVERLAY_Show) return orig_UPLAY_OVERLAY_Show(a, b);
	return 0;
}

__declspec(dllexport) int UPLAY_OVERLAY_SetShopUrl(void* a)
{
	if (orig_UPLAY_OVERLAY_SetShopUrl) return orig_UPLAY_OVERLAY_SetShopUrl(a);
	return 0;
}

__declspec(dllexport) int UPLAY_FRIENDS_GetFriendList(void* a, void* b)
{
	if (orig_UPLAY_FRIENDS_GetFriendList) return orig_UPLAY_FRIENDS_GetFriendList(a, b);
	return 0;
}

__declspec(dllexport) int UPLAY_USER_SetGameSession(void* a)
{
	if (orig_UPLAY_USER_SetGameSession) return orig_UPLAY_USER_SetGameSession(a);
	return 0;
}

__declspec(dllexport) int UPLAY_USER_ClearGameSession(void)
{
	if (orig_UPLAY_USER_ClearGameSession) return orig_UPLAY_USER_ClearGameSession();
	return 0;
}

__declspec(dllexport) int UPLAY_FRIENDS_InviteToGame(void* a, void* b)
{
	if (orig_UPLAY_FRIENDS_InviteToGame) return orig_UPLAY_FRIENDS_InviteToGame(a, b);
	return 0;
}

__declspec(dllexport) int UPLAY_PARTY_GetFullMemberList(void* a, void* b)
{
	if (orig_UPLAY_PARTY_GetFullMemberList) return orig_UPLAY_PARTY_GetFullMemberList(a, b);
	return 0;
}

__declspec(dllexport) int UPLAY_PARTY_IsInParty(void* a)
{
	if (orig_UPLAY_PARTY_IsInParty) return orig_UPLAY_PARTY_IsInParty(a);
	return 0;
}

__declspec(dllexport) int UPLAY_WIN_GetRewards(void* a, void* b)
{
	if (orig_UPLAY_WIN_GetRewards) return orig_UPLAY_WIN_GetRewards(a, b);
	return 0;
}

__declspec(dllexport) int UPLAY_WIN_ReleaseRewardList(void* a)
{
	if (orig_UPLAY_WIN_ReleaseRewardList) return orig_UPLAY_WIN_ReleaseRewardList(a);
	return 0;
}

__declspec(dllexport) int UPLAY_WIN_RefreshActions(void* a)
{
	if (orig_UPLAY_WIN_RefreshActions) return orig_UPLAY_WIN_RefreshActions(a);
	return 0;
}

__declspec(dllexport) int UPLAY_OVERLAY_ShowShopUrl(void* a)
{
	if (orig_UPLAY_OVERLAY_ShowShopUrl) return orig_UPLAY_OVERLAY_ShowShopUrl(a);
	return 0;
}
}
