#pragma once
#include <cstdint>

typedef unsigned char _BYTE;

typedef __int64* (__fastcall* LoadUI_t)(__int64* a1, char* a2);
typedef __int64(__fastcall* InitUIStruct_t)(__int64 objPtr, __int64 uiHandle);
typedef __int64(__fastcall* ReleaseResourceRef_t)(__int64 ptr);
typedef void* (__fastcall* Alloc_t)(int size);
typedef __int64(__fastcall* BuildParamBlob_t)(_BYTE* outParam, unsigned __int16 unk, _BYTE* scratch);
typedef void(__fastcall* BindUIParams_t)(__int64 objPlus16, _BYTE** paramBlobPtr, __int64 count);
typedef __int64(__fastcall* GetUISystem_t)(__int64 dummy);
typedef void(__fastcall* EnqueueUI_t)(__int64 uiList, __int64 uiHandle);
typedef void(__fastcall* QwordFunc_t)(_BYTE*);
typedef bool(__fastcall* LoadUILayout_t)(const char* layoutPath, bool forceReload, void* ownerContext);
typedef int(__fastcall* SnowdropVersion_t)(void);
typedef char(__fastcall* SnowdropIsFileAvailable_t)(_BYTE* a1, char a2);
typedef bool(__fastcall* SnowdropOpenFile_t)(__int64 a1, _BYTE* a2, unsigned int a3);
typedef void* (__fastcall* SnowdropInitFileStruct_t)(void* ctx);

typedef __int64(__fastcall* CamelClient_GetPlatformType_t)(int* rcx);

// Function pointers
extern LoadUILayout_t LoadFacemanUILayout;
extern Alloc_t Alloc;
extern LoadUI_t LoadUI;
extern ReleaseResourceRef_t ReleaseResourceRef;
extern InitUIStruct_t InitUIObj;
extern BuildParamBlob_t BuildParamBlob;
extern GetUISystem_t GetUISystem;
extern BindUIParams_t BindUIParams;
extern EnqueueUI_t EnqueueUI;

extern SnowdropVersion_t GetSnowdropEngineVersion;
extern SnowdropIsFileAvailable_t SnowdropIsFileAvailable;
extern SnowdropOpenFile_t SnowdropOpenFile;
extern SnowdropInitFileStruct_t SnowdropInitFileStruct;

extern CamelClient_GetPlatformType_t GetPlatformType;

// Global offsets
extern unsigned __int16* word_143AD9F48;  // script ID (for muigraph loader)
extern __int64* qword_143AD9E68_ptr;      // ptr to global (for muigraph loader)

void loadFunctionPointers();