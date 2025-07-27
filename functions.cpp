#include "functions.h"
#include "mem.h"

// Engine Memory
Alloc_t Alloc = nullptr;
ReleaseResourceRef_t ReleaseResourceRef = nullptr;

// Resource Loading Related
LoadUI_t LoadUI = nullptr;
InitUIStruct_t InitUIObj = nullptr;
BuildParamBlob_t BuildParamBlob = nullptr;
GetUISystem_t GetUISystem = nullptr;
BindUIParams_t BindUIParams = nullptr;
EnqueueUI_t EnqueueUI = nullptr;
LoadUILayout_t LoadFacemanUILayout = nullptr;

// Engine
SnowdropVersion_t GetSnowdropEngineVersion = nullptr;
SnowdropIsFileAvailable_t SnowdropIsFileAvailable = nullptr;
SnowdropOpenFile_t SnowdropOpenFile = nullptr;
SnowdropInitFileStruct_t SnowdropInitFileStruct = nullptr;

// Camel Client
CamelClient_GetPlatformType_t GetPlatformType = nullptr;

// Global offsets
unsigned __int16* word_143AD9F48 = nullptr;
__int64* qword_143AD9E68_ptr = nullptr;


void loadFunctionPointers()
{
	LoadFacemanUILayout = reinterpret_cast<LoadUILayout_t>(Addresses::LoadUILayout);

	LoadUI = reinterpret_cast<LoadUI_t>(Addresses::LoadUI);

	InitUIObj = reinterpret_cast<InitUIStruct_t>(Addresses::InitUIStruct);
	BuildParamBlob = reinterpret_cast<BuildParamBlob_t>(Addresses::BuildParamBlob);
	GetUISystem = reinterpret_cast<GetUISystem_t>(Addresses::GetUISystem);
	BindUIParams = reinterpret_cast<BindUIParams_t>(Addresses::BindUIParams);
	EnqueueUI = reinterpret_cast<EnqueueUI_t>(Addresses::EnqueueUI);

	Alloc = reinterpret_cast<Alloc_t>(Addresses::Alloc);
	ReleaseResourceRef = reinterpret_cast<ReleaseResourceRef_t>(Addresses::ReleaseResourceRef);
	GetSnowdropEngineVersion = reinterpret_cast<SnowdropVersion_t>(Addresses::SnowdropEngineVersion);
	SnowdropIsFileAvailable = reinterpret_cast<SnowdropIsFileAvailable_t>(Addresses::SnowdropIsFileAvailable);
	SnowdropOpenFile = reinterpret_cast<SnowdropOpenFile_t>(Addresses::SnowdropOpenFile);
	SnowdropInitFileStruct = reinterpret_cast<SnowdropInitFileStruct_t>(Addresses::SnowdropInitFileStruct);

	GetPlatformType = reinterpret_cast<CamelClient_GetPlatformType_t>(Addresses::GetPlatformType);

	word_143AD9F48 = (unsigned __int16*)(0x143AD9F48);
	qword_143AD9E68_ptr = (__int64*)(0x143AD9E68);
}