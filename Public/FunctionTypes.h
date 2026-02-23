#pragma once
#include <cstdint>

using _BYTE = unsigned char;

using LoadUI_t = __int64* (__fastcall*)(__int64* a1, char* a2);
using InitUIStruct_t = __int64(__fastcall*)(__int64 objPtr, __int64 uiHandle);
using ReleaseResourceRef_t = __int64(__fastcall*)(__int64 ptr);

using Alloc_t = void* (__fastcall*)(int size);
using Free_t = void(__fastcall*)(void* ptr);

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

using ReadData_t = char(__fastcall*)(void* readerContext, void* buffer, int size);

using CamelClient_GetPlatformType_t = __int64(__fastcall*)(int* rcx);
using RegisterActorList_t = void(__fastcall*)(uint8_t* a1, uint64_t* arrayStart, uint64_t* arrayEnd, int a4, char a5,
                                              int a6);
