#pragma once
#include <cstdint>
#include "../../Public/FunctionTypes.h"

extern LoadUILayout_t LoadFacemanUILayout;
extern Alloc_t Alloc;
extern Free_t Free;
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
extern ReadData_t ReadData;

extern CamelClient_GetPlatformType_t GetPlatformType;
extern RegisterActorList_t OriginalRegisterActorList;

// Global offsets
extern unsigned __int16* word_143AD9F48; // script ID (for muigraph loader)
extern __int64* qword_143AD9E68_ptr; // ptr to global (for muigraph loader)

void loadFunctionPointers();
