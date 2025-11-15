#pragma once

#include <Windows.h>

using UPLAY_ACH_GetAchievements_t = int(*)(void*, void*, void*);
using UPLAY_ACH_ReleaseAchievementList_t = int(*)(void*);
using UPLAY_USER_IsOwned_t = int(*)(void*, void*);
using UPLAY_GetLastError_t = int(*)(void*);
using UPLAY_USER_IsInOfflineMode_t = int(*)(void*);
using UPLAY_USER_IsConnected_t = int(*)(void*);
using UPLAY_HasOverlappedOperationCompleted_t = int(*)(void*, void*);
using UPLAY_GetOverlappedOperationResult_t = int(*)(void*, void*);
using UPLAY_SAVE_GetSavegames_t = int(*)(void*, void*);
using UPLAY_SAVE_ReleaseGameList_t = int(*)(void*);
using UPLAY_SAVE_Open_t = int(*)(void*, void*, void*, void*);
using UPLAY_SAVE_Close_t = int(*)(void*);
using UPLAY_SAVE_Read_t = int(*)(void*, void*, void*, void*);
using UPLAY_SAVE_Write_t = int(*)(void*, void*, void*);
using UPLAY_SAVE_SetName_t = int(*)(void*, void*);
using UPLAY_SAVE_Remove_t = int(*)(void*);
using UPLAY_FRIENDS_Init_t = int(*)(void*);
using UPLAY_PARTY_Init_t = int(*)(void*);
using UPLAY_Start_t = int(*)(void*, void*);
using UPLAY_Quit_t = int(*)(void);
using UPLAY_AVATAR_Get_t = int(*)(void*, void*, void*);
using UPLAY_AVATAR_Release_t = int(*)(void*);
using UPLAY_USER_GetAccountIdUtf8_t = int(*)(void*, void*);
using UPLAY_USER_GetEmailUtf8_t = int(*)(void*, void*);
using UPLAY_USER_GetNameUtf8_t = int(*)(void*, void*);
using UPLAY_USER_GetTicketUtf8_t = int(*)(void*, void*);
using UPLAY_GetNextEvent_t = int(*)(void*);
using UPLAY_Update_t = int(*)(void);
using UPLAY_ACH_EarnAchievement_t = int(*)(void*, void*);
using UPLAY_OVERLAY_Show_t = int(*)(void*, void*);
using UPLAY_OVERLAY_SetShopUrl_t = int(*)(void*);
using UPLAY_FRIENDS_GetFriendList_t = int(*)(void*, void*);
using UPLAY_USER_SetGameSession_t = int(*)(void*);
using UPLAY_USER_ClearGameSession_t = int(*)(void);
using UPLAY_FRIENDS_InviteToGame_t = int(*)(void*, void*);
using UPLAY_PARTY_GetFullMemberList_t = int(*)(void*, void*);
using UPLAY_PARTY_IsInParty_t = int(*)(void*);
using UPLAY_WIN_GetRewards_t = int(*)(void*, void*);
using UPLAY_WIN_ReleaseRewardList_t = int(*)(void*);
using UPLAY_WIN_RefreshActions_t = int(*)(void*);
using UPLAY_OVERLAY_ShowShopUrl_t = int(*)(void*);

extern "C" {
__declspec(dllexport) int UPLAY_ACH_GetAchievements(void* a, void* b, void* c);
__declspec(dllexport) int UPLAY_ACH_ReleaseAchievementList(void* a);
__declspec(dllexport) int UPLAY_USER_IsOwned(void* a, void* b);
__declspec(dllexport) int UPLAY_GetLastError(void* a);
__declspec(dllexport) int UPLAY_USER_IsInOfflineMode(void* a);
__declspec(dllexport) int UPLAY_USER_IsConnected(void* a);
__declspec(dllexport) int UPLAY_HasOverlappedOperationCompleted(void* a, void* b);
__declspec(dllexport) int UPLAY_GetOverlappedOperationResult(void* a, void* b);
__declspec(dllexport) int UPLAY_SAVE_GetSavegames(void* a, void* b);
__declspec(dllexport) int UPLAY_SAVE_ReleaseGameList(void* a);
__declspec(dllexport) int UPLAY_SAVE_Open(void* a, void* b, void* c, void* d);
__declspec(dllexport) int UPLAY_SAVE_Close(void* a);
__declspec(dllexport) int UPLAY_SAVE_Read(void* a, void* b, void* c, void* d);
__declspec(dllexport) int UPLAY_SAVE_Write(void* a, void* b, void* c);
__declspec(dllexport) int UPLAY_SAVE_SetName(void* a, void* b);
__declspec(dllexport) int UPLAY_SAVE_Remove(void* a);
__declspec(dllexport) int UPLAY_FRIENDS_Init(void* a);
__declspec(dllexport) int UPLAY_PARTY_Init(void* a);
__declspec(dllexport) int UPLAY_Start(void* a, void* b);
__declspec(dllexport) int UPLAY_Quit(void);
__declspec(dllexport) int UPLAY_AVATAR_Get(void* a, void* b, void* c);
__declspec(dllexport) int UPLAY_AVATAR_Release(void* a);
__declspec(dllexport) int UPLAY_USER_GetAccountIdUtf8(void* a, void* b);
__declspec(dllexport) int UPLAY_USER_GetEmailUtf8(void* a, void* b);
__declspec(dllexport) int UPLAY_USER_GetNameUtf8(void* a, void* b);
__declspec(dllexport) int UPLAY_USER_GetTicketUtf8(void* a, void* b);
__declspec(dllexport) int UPLAY_GetNextEvent(void* a);
__declspec(dllexport) int UPLAY_Update(void);
__declspec(dllexport) int UPLAY_ACH_EarnAchievement(void* a, void* b);
__declspec(dllexport) int UPLAY_OVERLAY_Show(void* a, void* b);
__declspec(dllexport) int UPLAY_OVERLAY_SetShopUrl(void* a);
__declspec(dllexport) int UPLAY_FRIENDS_GetFriendList(void* a, void* b);
__declspec(dllexport) int UPLAY_USER_SetGameSession(void* a);
__declspec(dllexport) int UPLAY_USER_ClearGameSession(void);
__declspec(dllexport) int UPLAY_FRIENDS_InviteToGame(void* a, void* b);
__declspec(dllexport) int UPLAY_PARTY_GetFullMemberList(void* a, void* b);
__declspec(dllexport) int UPLAY_PARTY_IsInParty(void* a);
__declspec(dllexport) int UPLAY_WIN_GetRewards(void* a, void* b);
__declspec(dllexport) int UPLAY_WIN_ReleaseRewardList(void* a);
__declspec(dllexport) int UPLAY_WIN_RefreshActions(void* a);
__declspec(dllexport) int UPLAY_OVERLAY_ShowShopUrl(void* a);
}

extern HMODULE g_originalDLL;
extern bool g_dllInjected;

bool loadOriginalDll();
void unLoadOriginalDLL();
void initializeUPlayProxy();
