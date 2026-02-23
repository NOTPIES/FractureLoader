#pragma once
#include <cstdint>
#include "MCommon/MC_Vector.h"
#include "Enums.h"
#include "Structs.h"

namespace Snowdrop
{
	class UberCamera
	{
	public:
		char pad_0000[0x128]; // +0x000
		MC_Vector3f position; // +0x128
		char pad_0134[0x238]; // +0x134
		uint8_t cameraLock; // +0x36C — 1 = locked, 0 = unlocked
		char pad_036D[0x93]; // +0x36D
		void* localPlayer; // +0x400
	};

	class CameraManager
	{
	public:
		void** vtable; // +0x000 → 0x141FEF1A8
		CClient_WorldModel* pWorldModel; // +0x008
		char pad_0010[0x10]; // +0x010
		UberCamera* pCamera; // +0x020
		char pad_0028[0x18]; // +0x028
		void* pCurrentTarget; // +0x040
		uint8_t flags; // +0x048 — bit 1 = invertY
	};
}
