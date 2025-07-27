#pragma once

#include <Windows.h>

typedef int(*UPLAY_ACH_GetAchievements_t)(void*, void*, void*);
typedef int(*UPLAY_ACH_ReleaseAchievementList_t)(void*);
typedef int(*UPLAY_USER_IsOwned_t)(void*, void*);
typedef int(*UPLAY_GetLastError_t)(void*);
typedef int(*UPLAY_USER_IsInOfflineMode_t)(void*);
typedef int(*UPLAY_USER_IsConnected_t)(void*);
typedef int(*UPLAY_HasOverlappedOperationCompleted_t)(void*, void*);
typedef int(*UPLAY_GetOverlappedOperationResult_t)(void*, void*);
typedef int(*UPLAY_SAVE_GetSavegames_t)(void*, void*);
typedef int(*UPLAY_SAVE_ReleaseGameList_t)(void*);
typedef int(*UPLAY_SAVE_Open_t)(void*, void*, void*, void*);
typedef int(*UPLAY_SAVE_Close_t)(void*);
typedef int(*UPLAY_SAVE_Read_t)(void*, void*, void*, void*);
typedef int(*UPLAY_SAVE_Write_t)(void*, void*, void*);
typedef int(*UPLAY_SAVE_SetName_t)(void*, void*);
typedef int(*UPLAY_SAVE_Remove_t)(void*);
typedef int(*UPLAY_FRIENDS_Init_t)(void*);
typedef int(*UPLAY_PARTY_Init_t)(void*);
typedef int(*UPLAY_Start_t)(void*, void*);
typedef int(*UPLAY_Quit_t)(void);
typedef int(*UPLAY_AVATAR_Get_t)(void*, void*, void*);
typedef int(*UPLAY_AVATAR_Release_t)(void*);
typedef int(*UPLAY_USER_GetAccountIdUtf8_t)(void*, void*);
typedef int(*UPLAY_USER_GetEmailUtf8_t)(void*, void*);
typedef int(*UPLAY_USER_GetNameUtf8_t)(void*, void*);
typedef int(*UPLAY_USER_GetTicketUtf8_t)(void*, void*);
typedef int(*UPLAY_GetNextEvent_t)(void*);
typedef int(*UPLAY_Update_t)(void);
typedef int(*UPLAY_ACH_EarnAchievement_t)(void*, void*);
typedef int(*UPLAY_OVERLAY_Show_t)(void*, void*);
typedef int(*UPLAY_OVERLAY_SetShopUrl_t)(void*);
typedef int(*UPLAY_FRIENDS_GetFriendList_t)(void*, void*);
typedef int(*UPLAY_USER_SetGameSession_t)(void*);
typedef int(*UPLAY_USER_ClearGameSession_t)(void);
typedef int(*UPLAY_FRIENDS_InviteToGame_t)(void*, void*);
typedef int(*UPLAY_PARTY_GetFullMemberList_t)(void*, void*);
typedef int(*UPLAY_PARTY_IsInParty_t)(void*);
typedef int(*UPLAY_WIN_GetRewards_t)(void*, void*);
typedef int(*UPLAY_WIN_ReleaseRewardList_t)(void*);
typedef int(*UPLAY_WIN_RefreshActions_t)(void*);
typedef int(*UPLAY_OVERLAY_ShowShopUrl_t)(void*);

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