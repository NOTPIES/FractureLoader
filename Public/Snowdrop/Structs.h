#pragma once
#include <cstdint>
#include "MCommon/MC_Vector.h"

namespace Snowdrop
{
	// Forward declarations
	class CClient_Actor;
	class ActorContainer;
	class CClient_WorldModel;
	class Client;
	class CamelClient;
	class CameraManager;
	class WorldState;
	class UberCamera;

	class CClient_Actor
	{
	public:
		void* pVTable; // +0x000
		char pad_0008[0x78]; // +0x008
		MC_Vector3f vPosition; // +0x080
		MC_Vector3f vRotation; // +0x08C
		MC_Vector3f vScale; // +0x098
		char pad_00A4[0x144]; // +0x0A4
		uint8_t bIsActive; // +0x1E8
		char pad_01E9[0x7]; // +0x1E9
		uint8_t bFlag_1EA; // +0x1EA
		char pad_01EB[0x5]; // +0x1EB
		uintptr_t uTimestamp; // +0x1F0 (496)
		uintptr_t uField_1F8; // +0x1F8 (504)
		char pad_0200[0x80]; // +0x200
		uintptr_t pComponent_280; // +0x280 (640)
		char pad_0288[0x1A0]; // +0x288
		uintptr_t pComponent_428; // +0x428 (1064)
		uintptr_t pComponent_438; // +0x438 (1080)
		char pad_0440[0x20]; // +0x440
		uintptr_t pComponent_460; // +0x460 (1120)
		uintptr_t pComponent_470; // +0x470 (1136)
		uintptr_t pComponent_478; // +0x478 (1144)
		uintptr_t pComponent_480; // +0x480 (1152)

		uintptr_t GetVTable() const
		{
			return (uintptr_t)pVTable;
		}

		template<typename Ret = void, typename... Args>
		Ret CallVFunc(size_t index, Args... args)
		{
			using Fn = Ret(*)(void*, Args...);

			auto vtable = *reinterpret_cast<void***>(this);
			auto fn = reinterpret_cast<Fn>(vtable[index]);

			return fn(this, args...);
		}

		// vtable[36] (+0x120)
		void Tick()
		{
			CallVFunc(36);
		}

		// vtable[18] (+0x90)
		bool IsValid()
		{
			return CallVFunc<bool>(18);
		}

		float DistanceTo(CClient_Actor* other) const
		{
			if (!other) return -1.0f;
			MC_Vector3f delta = vPosition - other->vPosition;
			return delta.Length();
		}

		float DistanceTo(const MC_Vector3f& point) const
		{
			MC_Vector3f delta = vPosition - point;
			return delta.Length();
		}
	};

	struct ActorQuery
	{
		uint8_t v5[8]; // +0x00: internal state
		uintptr_t v6; // +0x08: result container after query
	};
}
