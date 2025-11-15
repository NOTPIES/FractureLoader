#pragma once
#include <string>

using _BYTE = unsigned char;

void initializeHooks();
void cleanupHooks();
bool __fastcall snowdropOpenFileHook(__int64 a1, _BYTE* a2, unsigned int a3);
bool __fastcall snowdropIsFileAvailableHook(_BYTE* filePath, int a2);
