#include "mem.h"
#include "logger.h"

#include <windows.h>
#include <vector>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "functions.h"

#include "memcury/memcury.h"

uintptr_t Addresses::LoadUILayout = 0;
uintptr_t Addresses::LoadMMBModel = 0;
uintptr_t Addresses::LoadNPCFile = 0;
uintptr_t Addresses::LoadUI = 0x14119EED0;
uintptr_t Addresses::InitUIStruct = 0;
uintptr_t Addresses::GetUISystem = 0x1400179B0;
uintptr_t Addresses::GetOtherSystemIDontKnowTheNameOf = 0x1400181C0;
uintptr_t Addresses::BindUIParams = 0x14019DA90;
uintptr_t Addresses::EnqueueUI = 0x14019DC00;

uintptr_t Addresses::Alloc = 0x143FA3DE0;
uintptr_t Addresses::ReleaseResourceRef = 0;
uintptr_t Addresses::SnowdropEngineVersion = 0x1419F7330;
uintptr_t Addresses::SnowdropOpenFile = 0;
uintptr_t Addresses::SnowdropInitFileStruct = 0;
uintptr_t Addresses::SnowdropIsFileAvailable = 0;

uintptr_t Addresses::BuildParamBlob = 0x1416C8A30;

uintptr_t Addresses::GetPlatformType = 0;

uintptr_t memcuryFind(const char* pattern, const char* name)
{
	auto scanner = Memcury::Scanner::FindPattern(pattern);
	uintptr_t result = scanner.Get();

	if (result)
		FRACTURE_DEBUG("Pattern '{}' found at address: {:X}", name, result);
	else
		FRACTURE_ERROR("Failed to find '{}' pattern!", name);

	return result;
}

bool readMemory(uintptr_t address, void* buffer, size_t size)
{
	ReadProcessMemory(GetCurrentProcess(), (LPCVOID)address, buffer, size, nullptr);
	return true;
}

bool writeMemory(uintptr_t address, const void* buffer, size_t size)
{
	WriteProcessMemory(GetCurrentProcess(), (LPVOID)address, buffer, size, nullptr);
	return true;
}

void Addresses::Load()
{
	LoadUILayout = memcuryFind(
		"48 89 74 24 ? 48 89 7C 24 ? 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 4C 8B 35",
		"LoadUILayout"
	);

	LoadMMBModel = memcuryFind(
		"48 8B C4 44 88 40 ? 48 89 50 ? 55",
		"LoadMMBModel"
	);

	LoadNPCFile = memcuryFind(
		"48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 89 CE 48 8D 4C 24 ? 48 89 D7 E8 ? ? ? ? 80 78",
		"LoadNPCFile"
	);

	InitUIStruct = memcuryFind(
		"48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 89 CB 48 83 C1 ? 48 89 D6",
		"InitUIStruct"
	);

	ReleaseResourceRef = memcuryFind(
		"48 89 5C 24 ? 57 48 83 EC ? 8B 41 ? 48 89 CF 83 F8",
		"ReleaseResourceRef"
	);

	SnowdropOpenFile = memcuryFind(
		"40 53 55 41 56 48 83 EC ? 41 8B E8",
		"SnowdropOpenFile"
	);

	SnowdropIsFileAvailable = memcuryFind(
		"53 48 83 EC ? 48 89 CB 48 85 C9 74 ? 80 39 ? 74 ? 84 D2 75 ? 48 8B 0D ? ? ? ? 48 85 C9 74 ? 48 8B 01 48 89 DA",
		"SnowdropIsFileAvailable"
	);

	SnowdropInitFileStruct = memcuryFind(
		"53 48 83 EC ? 48 89 CB 48 83 C1 ? E8 ? ? ? ? 31 C0 48 C7 43",
		"SnowdropInitFileStruct"
	);

	GetPlatformType = memcuryFind(
		"8B 05 ? ? ? ? 83 F8 ? 77 ? 89 01",
		"GetPlatformType"
	);

	loadFunctionPointers();
}
