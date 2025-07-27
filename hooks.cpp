#include "hooks.h"
#include "MinHook.h"
#include <cstdio>
#include <mutex>
#include <iostream>
#include <unordered_set>
#include <string_view>
#include "logger.h"
#include "functions.h"
#include "mem.h"
#include "mod_loader.h"
#include <filesystem>
#include "global.h"

DWORD WINAPI guiHook(LPVOID);

namespace fs = std::filesystem;

bool __fastcall snowdropIsFileAvailableHook(_BYTE* filePath, int a2) {
    if (!filePath || !*filePath) {
        return SnowdropIsFileAvailable(filePath, a2);
    }

    bool result = SnowdropIsFileAvailable(filePath, a2);

	if (result || !g_modLoader) {
		return result;
	}

    std::string originalPath = reinterpret_cast<const char*>(filePath);
    std::string normalizedPath = Global::normalizePath(originalPath);

    {
        std::lock_guard<std::mutex> lock(g_modLoader->availabilityCacheMutex);
        auto cacheIt = g_modLoader->fileAvailabilityCache.find(normalizedPath);
        if (cacheIt != g_modLoader->fileAvailabilityCache.end()) {
            return cacheIt->second;
        }
    }

    bool isCustomAsset = g_modLoader->checkCustomAssetAvailability(normalizedPath);
    if (isCustomAsset) {
        std::lock_guard<std::mutex> lock(g_modLoader->availabilityCacheMutex);
        g_modLoader->fileAvailabilityCache[normalizedPath] = true;
        return true;
    }

    return false;
}

bool __fastcall snowdropOpenFileHook(__int64 a1, _BYTE* a2, unsigned int a3) {
    if (a2 && *a2 && g_modLoader) {
        std::string originalPath = reinterpret_cast<const char*>(a2);
        std::string overridePath = g_modLoader->findFileOverride(originalPath);

        if (!overridePath.empty()) {
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
            CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)guiHook, nullptr, 0, nullptr);
        }
    }

    return LoadFacemanUILayout(a1, forceReload, ownerContext);
}

__int64 __fastcall getPlatformTypeHook(int* rcx)
{
    *rcx = static_cast<int>(Global::g_currentPlatform);
    return (__int64)rcx;
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

    if (MH_CreateHook(reinterpret_cast<void*>(Addresses::SnowdropOpenFile), reinterpret_cast<LPVOID>(&snowdropOpenFileHook), reinterpret_cast<void**>(&SnowdropOpenFile)) != MH_OK)
    {
        FRACTURE_ERROR("Failed to create SnowdropOpenFile hook.\n");
        return;
    }

    if (MH_CreateHook(reinterpret_cast<void*>(Addresses::SnowdropIsFileAvailable), reinterpret_cast<LPVOID>(&snowdropIsFileAvailableHook), reinterpret_cast<void**>(&SnowdropIsFileAvailable)) != MH_OK)
    {
        FRACTURE_ERROR("Failed to create SnowdropIsFileAvailable hook.\n");
        return;
    }

	if (MH_CreateHook(reinterpret_cast<void*>(Addresses::LoadUILayout), reinterpret_cast<LPVOID>(&loadFacemanLayoutHook), reinterpret_cast<void**>(&LoadFacemanUILayout)) != MH_OK)
	{
		FRACTURE_ERROR("Failed to create LoadFacemanUILayout hook.\n");
		return;
	}

    if (MH_CreateHook(reinterpret_cast<void*>(Addresses::GetPlatformType), reinterpret_cast<LPVOID>(&getPlatformTypeHook), reinterpret_cast<void**>(&GetPlatformType)) != MH_OK)
    {
        FRACTURE_ERROR("Failed to create GetPlatformType hook.\n");
        return;
    }

    MH_EnableHook(MH_ALL_HOOKS);

	FRACTURE_DEBUG("Hooks Initialized Successfully.");
}

void cleanupHooks()
{
    MH_DisableHook(MH_ALL_HOOKS); // kiero may mess this up
    MH_Uninitialize();
}