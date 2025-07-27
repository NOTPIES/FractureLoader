#pragma once
#include <string>

typedef unsigned char _BYTE;

void initializeHooks();
void cleanupHooks();
bool __fastcall snowdropOpenFileHook(__int64 a1, _BYTE* a2, unsigned int a3);
bool __fastcall snowdropIsFileAvailableHook(_BYTE* filePath, int a2);