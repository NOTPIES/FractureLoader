#pragma once
#include <cstdint>

using _BYTE = unsigned char;

using LoadUI_t = __int64* (__fastcall*)(__int64* a1, char* a2);
using InitUIStruct_t = __int64(__fastcall*)(__int64 objPtr, __int64 uiHandle);
using ReleaseResourceRef_t = __int64(__fastcall*)(__int64 ptr);
using Alloc_t = void* (__fastcall*)(int size);
using BuildParamBlob_t = __int64(__fastcall*)(_BYTE* outParam, unsigned __int16 unk, _BYTE* scratch);
using BindUIParams_t = void(__fastcall*)(__int64 objPlus16, _BYTE** paramBlobPtr, __int64 count);
using GetUISystem_t = __int64(__fastcall*)(__int64 dummy);
using EnqueueUI_t = void(__fastcall*)(__int64 uiList, __int64 uiHandle);
using QwordFunc_t = void(__fastcall*)(_BYTE*);
using LoadUILayout_t = bool(__fastcall*)(const char* layoutPath, bool forceReload, void* ownerContext);
using SnowdropVersion_t = int(__fastcall*)(void);
using SnowdropIsFileAvailable_t = char(__fastcall*)(_BYTE* a1, char a2);
using SnowdropOpenFile_t = bool(__fastcall*)(__int64 a1, _BYTE* a2, unsigned int a3);
using SnowdropInitFileStruct_t = void* (__fastcall*)(void* ctx);

using CamelClient_GetPlatformType_t = __int64(__fastcall*)(int* rcx);

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
extern unsigned __int16* word_143AD9F48; // script ID (for muigraph loader)
extern __int64* qword_143AD9E68_ptr; // ptr to global (for muigraph loader)

void loadFunctionPointers();
