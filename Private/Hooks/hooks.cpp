#include "hooks.h"
#include "MinHook.h"
#include <cstdio>
#include <mutex>
#include <iostream>
#include <unordered_set>
#include <string_view>
#include "../Logging/logger.h"
#include "../Memory/functions.h"
#include "../Memory/mem.h"
#include "../Loader/mod_loader.h"
#include <filesystem>
#include "../Core/global.h"

std::vector<ActorEntry> g_actorList;
std::mutex g_actorListMutex;
std::atomic<bool> g_actorListRequested{false};

DWORD WINAPI guiHook(LPVOID);

namespace fs = std::filesystem;

/*typedef __int64(__fastcall* LoadNPCFile_t)(__int64 resourceManager, const char* filePath);
LoadNPCFile_t OriginalLoadNPCFile = nullptr;

__int64 __fastcall loadNPCFileHook(__int64 resourceManager, const char* filePath) {

	return OriginalLoadNPCFile(resourceManager, filePath);
}*/

bool __fastcall snowdropIsFileAvailableHook(_BYTE* filePath, int a2)
{
	if (!filePath || !*filePath)
	{
		return SnowdropIsFileAvailable(filePath, a2);
	}

	bool result = SnowdropIsFileAvailable(filePath, a2);

	if (result || !g_modLoader)
	{
		return result;
	}

	std::string originalPath = reinterpret_cast<const char*>(filePath);
	std::string normalizedPath = Global::normalizePath(originalPath);

	{
		std::lock_guard<std::mutex> lock(g_modLoader->availabilityCacheMutex);
		auto cacheIt = g_modLoader->fileAvailabilityCache.find(normalizedPath);
		if (cacheIt != g_modLoader->fileAvailabilityCache.end())
		{
			return cacheIt->second;
		}
	}

	bool isCustomAsset = g_modLoader->checkCustomAssetAvailability(normalizedPath);
	if (isCustomAsset)
	{
		std::lock_guard<std::mutex> lock(g_modLoader->availabilityCacheMutex);
		g_modLoader->fileAvailabilityCache[normalizedPath] = true;
		return true;
	}

	return false;
}

bool EndsWithI(const std::string& str, const std::string& suffix)
{
	if (suffix.size() > str.size()) return false;
	return _stricmp(str.c_str() + str.size() - suffix.size(), suffix.c_str()) == 0;
}

bool __fastcall snowdropOpenFileHook(__int64 a1, _BYTE* a2, unsigned int a3)
{
	if (a2 && *a2 && g_modLoader)
	{
		std::string originalPath = reinterpret_cast<const char*>(a2);
		std::string overridePath = g_modLoader->findFileOverride(originalPath);

		if (!overridePath.empty())
		{
			FRACTURE_DEBUG("Using override file: {}", overridePath);

			std::vector<char> overridePathBuffer(overridePath.begin(), overridePath.end());
			overridePathBuffer.push_back('\0');

			return SnowdropOpenFile(a1, reinterpret_cast<_BYTE*>(overridePathBuffer.data()), a3 |= (1 << 0xA));
		}
	}

	return SnowdropOpenFile(a1, a2, a3);
}

bool __fastcall loadFacemanLayoutHook(const char* a1, bool forceReload, void* ownerContext)
{
	if (a1 && _stricmp(a1, "faceman/layouts/Camel Client Retail.layout") == 0) // main camel Viewport
	{
		static bool guiStarted = false;
		if (!guiStarted)
		{
			guiStarted = true;

			bool menu = g_loaderManager->getValue<bool>("noMenu", true);

			if (menu)
			{
				FRACTURE_MSG("Skipping GUI initialization as per configuration.");
				return LoadFacemanUILayout(a1, forceReload, ownerContext);
			}

			CreateThread(nullptr, 0, guiHook, nullptr, 0, nullptr);
		}
	}

	return LoadFacemanUILayout(a1, forceReload, ownerContext);
}

__int64 __fastcall getPlatformTypeHook(int* rcx)
{
	*rcx = static_cast<int>(Global::g_currentPlatform);
	return (__int64)rcx;
}

void __fastcall registerActorListHook(uint8_t* a1, uint64_t* arrayStart, uint64_t* arrayEnd, int a4, char a5, int a6)
{
	if (g_actorListRequested.load(std::memory_order_relaxed))
	{
		std::lock_guard<std::mutex> lock(g_actorListMutex);
		g_actorList.clear();

		if (arrayStart && arrayEnd && arrayEnd > arrayStart)
		{
			int actorCount = static_cast<int>(arrayEnd - arrayStart);
			g_actorList.reserve(actorCount);

			for (int i = 0; i < actorCount; i++)
			{
				uint64_t actorPtr = arrayStart[i];
				if (!actorPtr) continue;

				uint64_t vtable = *(uint64_t*)actorPtr;
				g_actorList.push_back({actorPtr, vtable});
			}

			FRACTURE_DEBUG("Actor registry updated: {} actors captured", g_actorList.size());
		}

		g_actorListRequested.store(false, std::memory_order_relaxed);
	}

	OriginalRegisterActorList(a1, arrayStart, arrayEnd, a4, a5, a6);
}

void initializeHooks()
{
	auto InitializeResult = MH_Initialize();
	if (InitializeResult != MH_OK && InitializeResult != MH_ERROR_ALREADY_INITIALIZED)
	{
		FRACTURE_ERROR("Failed to initialize MinHook!");
		FRACTURE_ERROR("Error code: {}", InitializeResult);
		return;
	}

	if (MH_CreateHook(reinterpret_cast<void*>(Addresses::SnowdropOpenFile),
	                  reinterpret_cast<LPVOID>(&snowdropOpenFileHook),
	                  reinterpret_cast<void**>(&SnowdropOpenFile)) != MH_OK)
	{
		FRACTURE_ERROR("Failed to create SnowdropOpenFile hook.\n");
		return;
	}

	if (MH_CreateHook(reinterpret_cast<void*>(Addresses::SnowdropIsFileAvailable),
	                  reinterpret_cast<LPVOID>(&snowdropIsFileAvailableHook),
	                  reinterpret_cast<void**>(&SnowdropIsFileAvailable)) != MH_OK)
	{
		FRACTURE_ERROR("Failed to create SnowdropIsFileAvailable hook.\n");
		return;
	}

	if (MH_CreateHook(reinterpret_cast<void*>(Addresses::LoadUILayout),
	                  reinterpret_cast<LPVOID>(&loadFacemanLayoutHook),
	                  reinterpret_cast<void**>(&LoadFacemanUILayout)) != MH_OK)
	{
		FRACTURE_ERROR("Failed to create LoadFacemanUILayout hook.\n");
		return;
	}

	if (MH_CreateHook(reinterpret_cast<void*>(Addresses::GetPlatformType),
	                  reinterpret_cast<LPVOID>(&getPlatformTypeHook),
	                  reinterpret_cast<void**>(&GetPlatformType)) != MH_OK)
	{
		FRACTURE_ERROR("Failed to create GetPlatformType hook.\n");
		return;
	}

	if (MH_CreateHook(reinterpret_cast<void*>(Addresses::RegisterActorList),
	                  reinterpret_cast<LPVOID>(&registerActorListHook),
	                  reinterpret_cast<void**>(&OriginalRegisterActorList)) != MH_OK)
	{
		FRACTURE_ERROR("Failed to create RegisterActorList hook.\n");
		return;
	}

	/*if (MH_CreateHook(reinterpret_cast<void*>(0x148D301D0),
		reinterpret_cast<LPVOID>(&loadNPCFileHook),
		reinterpret_cast<void**>(&OriginalLoadNPCFile)) != MH_OK) 
	{
		FRACTURE_ERROR("Failed to create LoadNPCFile hook at 0x148D301D0.");
	}*/

	/*if (MH_CreateHook(reinterpret_cast<void*>(0x1416C1250),
		reinterpret_cast<LPVOID>(&snowdropLogFormatHook),
		reinterpret_cast<void**>(&fpSnowdropLogFormat)) != MH_OK)
	{
		FRACTURE_ERROR("Failed to create SnowdropLogFormat hook (Address: {:X})", 0x1416C1250);
	}
	else
	{
		FRACTURE_DEBUG("SnowdropLogFormat hooked successfully at {:X}", 0x1416C1250);
	}*/

	MH_EnableHook(nullptr);
}

void cleanupHooks()
{
	PatchFreecamUpdate(false); // restore freecam patches
	PatchCameraUpdate(false); // restore FOV patch
	MH_DisableHook(nullptr); // kiero may mess this up
	MH_Uninitialize();
}

static uint8_t cameraOriginalBytes[7] = {};
static bool cameraBytesBackedUp = false;

void PatchCameraUpdate(bool nop)
{
	DWORD oldProtect;
	VirtualProtect((LPVOID)0x14015D04D, 7, PAGE_EXECUTE_READWRITE, &oldProtect);
	if (nop)
	{
		if (!cameraBytesBackedUp)
		{
			memcpy(cameraOriginalBytes, (void*)0x14015D04D, 7);
			cameraBytesBackedUp = true;
		}
		memset((void*)0x14015D04D, 0x90, 7);
	}
	else if (cameraBytesBackedUp)
	{
		memcpy((void*)0x14015D04D, cameraOriginalBytes, 7);
	}
	VirtualProtect((LPVOID)0x14015D04D, 7, oldProtect, &oldProtect);
}

static uint8_t freecamOrigBytes_041[6] = {};
static uint8_t freecamOrigBytes_054[7] = {};
static bool freecamBytesBackedUp = false;

void PatchFreecamUpdate(bool nop)
{
	DWORD oldProtect;

	VirtualProtect((LPVOID)0x14015D041, 6, PAGE_EXECUTE_READWRITE, &oldProtect);
	if (nop)
	{
		if (!freecamBytesBackedUp)
		{
			memcpy(freecamOrigBytes_041, (void*)0x14015D041, 6);
			memcpy(freecamOrigBytes_054, (void*)0x14015D054, 7);
			freecamBytesBackedUp = true;
		}
		memset((void*)0x14015D041, 0x90, 6);
	}
	else if (freecamBytesBackedUp)
	{
		memcpy((void*)0x14015D041, freecamOrigBytes_041, 6);
	}
	VirtualProtect((LPVOID)0x14015D041, 6, oldProtect, &oldProtect);

	VirtualProtect((LPVOID)0x14015D054, 7, PAGE_EXECUTE_READWRITE, &oldProtect);
	if (nop)
	{
		memset((void*)0x14015D054, 0x90, 7);
	}
	else if (freecamBytesBackedUp)
	{
		memcpy((void*)0x14015D054, freecamOrigBytes_054, 7);
	}
	VirtualProtect((LPVOID)0x14015D054, 7, oldProtect, &oldProtect);
}
