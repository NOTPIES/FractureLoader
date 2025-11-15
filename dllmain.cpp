#include <windows.h>
#include <cstdio>
#include <iostream>
#include "includes.h"
#include <tlhelp32.h>
#include "global.h"
#include "logger.h"
#include "uplay_proxy.h"

#define USE_UBISOFT true; // For non-official/old TFBW builds, set this to false to skip the Ubisoft launcher check.

bool isFromUbisoftLauncher()
{
#if USE_UBISOFT
	DWORD currentPID = GetCurrentProcessId();
	DWORD parentPID = 0;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	bool foundCurrent = false;
	if (Process32First(hSnapshot, &pe32))
	{
		do
		{
			if (pe32.th32ProcessID == currentPID)
			{
				parentPID = pe32.th32ParentProcessID;
				foundCurrent = true;
				break;
			}
		}
		while (Process32Next(hSnapshot, &pe32));
	}

	if (!foundCurrent || parentPID == 0)
	{
		CloseHandle(hSnapshot);
		return false;
	}

	bool foundParent = false;
	bool isUbisoftLauncher = false;

	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hSnapshot, &pe32))
	{
		do
		{
			if (pe32.th32ProcessID == parentPID)
			{
				isUbisoftLauncher = (_wcsicmp(pe32.szExeFile, L"UbisoftGameLauncher.exe") == 0);
				foundParent = true;
				break;
			}
		}
		while (Process32Next(hSnapshot, &pe32));
	}

	CloseHandle(hSnapshot);
	return foundParent && isUbisoftLauncher;
#else
	return true;
#endif
}

static bool shouldInit = false;
static HANDLE hMainThread = nullptr;

DWORD mainThread(HMODULE Module)
{
	FRACTURE_ALLOC_CONSOLE();

	auto start = GetTickCount64();
	Addresses::Load();
	auto end = GetTickCount64();
	FRACTURE_DEBUG("Address loading took {} ms", end - start);

	if (!SnowdropOpenFile || !SnowdropIsFileAvailable || !SnowdropInitFileStruct)
	{
		FRACTURE_ERROR("Critical engine function not found. Aborting...");
		return 1;
	}

	initializeHooks();

	int EngineVer = GetSnowdropEngineVersion();

	if (EngineVer < 0)
	{
		FRACTURE_ERROR("Failed to get Snowdrop engine version.");
		return 1;
	}

	Global::engineVersion = "@Snowdrop/" + std::to_string(EngineVer);
	FRACTURE_MSG("Snowdrop Engine Version: {}", Global::engineVersion);

	if (g_loaderManager)
	{
		setupLoader();

		FRACTURE_MSG("FractureLoader v{} initialized successfully.", Global::fractureLoaderVersion);
		FRACTURE_MSG("Base directory: " + g_loaderManager->getBaseDirectory());
		FRACTURE_MSG("Mods directory: " + g_loaderManager->getModsDirectory());

		Global::g_currentPlatform = static_cast<CClient_PlatformEnum>(
			g_loaderManager->getValue<int>("spoofedPlatform", PC)
		);

		g_modLoader = new ModLoader(g_loaderManager->getModsDirectory());
		g_modLoader->scanMods();
	}

	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)guiHook, Module, 0, NULL);

	return 0;
}

void cleanupLoader()
{
	FRACTURE_MSG("Saving preferences...");
	if (g_modLoader)
	{
		g_modLoader->saveCurrentStates();
		delete g_modLoader;
		g_modLoader = nullptr;
		FRACTURE_DEBUG("Mod Loader cleaned up.");
	}

	if (g_loaderManager)
	{
		g_loaderManager->saveConfig();
		delete g_loaderManager;
		g_loaderManager = nullptr;
		FRACTURE_DEBUG("Config Manager cleaned up.");
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
	DisableThreadLibraryCalls(hModule);

	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		initializeUPlayProxy();

		if (isFromUbisoftLauncher())
		{
			// steam also uses the ubisoft launcher
			shouldInit = true;
			g_loaderManager = createLoaderManager("FractureLoader", "config.ini");

			hMainThread = CreateThread(nullptr, 0,
			                           (LPTHREAD_START_ROUTINE)mainThread,
			                           hModule, 0, nullptr);
		}
		break;
	case DLL_PROCESS_DETACH:
		if (hMainThread)
		{
			WaitForSingleObject(hMainThread, 2000);
			CloseHandle(hMainThread);
			hMainThread = nullptr;
		}

		if (shouldInit)
		{
			kiero::shutdown();
			cleanupHooks();
			cleanupLoader();
			FRACTURE_FREE_CONSOLE();
			FRACTURE_CLOSE_LOG_FILE();
		}

		unLoadOriginalDLL();
		break;
	}
	return TRUE;
}
