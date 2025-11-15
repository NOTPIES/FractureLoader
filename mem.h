#pragma once
#include <cstdint>
#include <vector>
#include <algorithm>

namespace Addresses
{
	extern uintptr_t LoadUILayout;
	extern uintptr_t LoadUI;
	extern uintptr_t InitUIStruct;
	extern uintptr_t BuildParamBlob;
	extern uintptr_t GetUISystem;
	extern uintptr_t GetOtherSystemIDontKnowTheNameOf;
	extern uintptr_t BindUIParams;
	extern uintptr_t EnqueueUI;

	extern uintptr_t LoadMMBModel;
	extern uintptr_t LoadNPCFile;

	// Engine
	extern uintptr_t Alloc;
	extern uintptr_t ReleaseResourceRef;
	extern uintptr_t SnowdropEngineVersion;
	extern uintptr_t SnowdropOpenFile;
	extern uintptr_t SnowdropIsFileAvailable;
	extern uintptr_t SnowdropInitFileStruct;

	// Camel Client
	extern uintptr_t GetPlatformType;

	void Load();
}
