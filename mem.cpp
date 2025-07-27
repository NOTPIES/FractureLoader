#include "mem.h"
#include "logger.h"

#include <windows.h>
#include <vector>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "functions.h"

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

uintptr_t findPattern(const char* signature, bool bRelative = false, uint32_t offset = 0) {
    uintptr_t base_address = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));
	static auto patternToByte = [](const char* pattern)
		{
			auto bytes = std::vector<int>{};
			const auto start = const_cast<char*>(pattern);
			const auto end = const_cast<char*>(pattern) + strlen(pattern);

			for (auto current = start; current < end; ++current)
			{
				if (*current == '?')
				{
					++current;
					if (*current == '?') ++current;
					bytes.push_back(-1);
				}
				else { bytes.push_back(strtoul(current, &current, 16)); }
			}
			return bytes;
		};

	const auto dosHeader = (PIMAGE_DOS_HEADER)base_address;
	const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)base_address + dosHeader->e_lfanew);

	const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
	auto patternBytes = patternToByte(signature);
	const auto scanBytes = reinterpret_cast<std::uint8_t*>(base_address);

	const auto s = patternBytes.size();
	const auto d = patternBytes.data();

	for (auto i = 0ul; i < sizeOfImage - s; ++i)
	{
		bool found = true;
		for (auto j = 0ul; j < s; ++j)
		{
			if (scanBytes[i + j] != d[j] && d[j] != -1)
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			uintptr_t address = reinterpret_cast<uintptr_t>(&scanBytes[i]);
			if (bRelative)
			{
				address = ((address + offset + 4) + *(int32_t*)(address + offset));
				FRACTURE_DEBUG("Pattern found at address: 0x{:X} (relative)", address);
				return address;
			}

			FRACTURE_DEBUG("Pattern found at address: 0x{:X}", address);
			return address;
		}
	}

	return NULL;
}

void Addresses::Load() {
	LoadUILayout = findPattern("48 89 74 24 ? 48 89 7C 24 ? 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 4C 8B 35");
	if (!LoadUILayout) FRACTURE_ERROR("Failed to find LoadUILayout pattern!");

	LoadMMBModel = findPattern("48 8B C4 44 88 40 ? 48 89 50 ? 55");
	if (!LoadMMBModel) FRACTURE_ERROR("Failed to find LoadMMBModel pattern!");
	
	LoadNPCFile = findPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 89 CE 48 8D 4C 24 ? 48 89 D7 E8 ? ? ? ? 80 78");
	if (!LoadNPCFile) FRACTURE_ERROR("Failed to find LoadNPCFile pattern!");

	InitUIStruct = findPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 89 CB 48 83 C1 ? 48 89 D6");
	if (!InitUIStruct) FRACTURE_ERROR("Failed to find InitUIStruct pattern!");

	ReleaseResourceRef = findPattern("48 89 5C 24 ? 57 48 83 EC ? 8B 41 ? 48 89 CF 83 F8");
	if (!ReleaseResourceRef) FRACTURE_ERROR("Failed to find ReleaseResourceRef pattern!");

	SnowdropOpenFile = findPattern("40 53 55 41 56 48 83 EC ? 41 8B E8");
	if (!SnowdropOpenFile) FRACTURE_ERROR("Failed to find SnowdropOpenFile pattern! (Mod loading will NOT work!)");

	SnowdropIsFileAvailable = findPattern("53 48 83 EC ? 48 89 CB 48 85 C9 74 ? 80 39 ? 74 ? 84 D2 75 ? 48 8B 0D ? ? ? ? 48 85 C9 74 ? 48 8B 01 48 89 DA");
	if (!SnowdropIsFileAvailable) FRACTURE_ERROR("Failed to find SnowdropIsFileAvailable pattern!");

	SnowdropInitFileStruct = findPattern("53 48 83 EC ? 48 89 CB 48 83 C1 ? E8 ? ? ? ? 31 C0 48 C7 43");
	if (!SnowdropInitFileStruct) FRACTURE_ERROR("Failed to find SnowdropInitFileStruct pattern!");

	GetPlatformType = findPattern("8B 05 ? ? ? ? 83 F8 ? 77 ? 89 01");
	if (!GetPlatformType) FRACTURE_ERROR("Failed to find GetPlatformType pattern!");

	loadFunctionPointers();
}