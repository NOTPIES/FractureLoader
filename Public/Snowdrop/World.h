#pragma once
#include <cstdint>
#include "Enums.h"
#include "Camera.h"

namespace Snowdrop
{
	class CClient_WorldModel
	{
	public:
		void* pVTable; // +0x000
		char pad_0008[0x68]; // +0x008
		uintptr_t field_0070; // +0x070
		uintptr_t field_0078; // +0x078
		char pad_0080[0x08]; // +0x080
		uintptr_t field_0088; // +0x088
		uintptr_t field_0090; // +0x090
		int32_t nField_0098; // +0x098
		char pad_009C[0x04]; // +0x09C
		uintptr_t pComponent_A0; // +0x0A0
		uintptr_t pComponent_A8; // +0x0A8
		uintptr_t pComponent_B0; // +0x0B0
		uintptr_t pComponent_B8; // +0x0B8
		uintptr_t pComponent_C0; // +0x0C0
		char pad_00C8[0x10]; // +0x0C8
		int32_t* pGameStateStack; // +0xD8
		int32_t nGameStateStackSize; // +0xE0
		char pad_00E4[0x8]; // +0xE4
		EGameActionState eGameActionState; // +0x0EC
		char pad_00F0[0x88]; // +0x0F0
		uintptr_t pWorldModelList; // +0x178
		uintptr_t pActorContainer; // +0x180
		uintptr_t pComponent_188; // +0x188
		uintptr_t pComponent_190; // +0x190
		uintptr_t pComponent_198; // +0x198
		char pad_01A0[0x2068]; // +0x1A0
		WorldState* pWorldState; // +0x2208
		char pad_2210[0x18]; // +0x2210
		CameraManager* pCameraManager; // +0x2228
		char pad_2230[0x188]; // +0x2230
		CClient_DemoType eDemoType; // +0x23B8

		EGameActionState GetGameActionState() const
		{
			return eGameActionState;
		}

		bool IsInGame() const
		{
			return eGameActionState == EGameActionState::Normal ||
				eGameActionState == EGameActionState::Combat;
		}

		bool IsLoading() const
		{
			return eGameActionState == EGameActionState::Loading;
		}

		bool IsPaused() const
		{
			return eGameActionState == EGameActionState::PauseMenu ||
				eGameActionState == EGameActionState::SystemPauseMenu ||
				eGameActionState == EGameActionState::CinematicPauseMenu ||
				eGameActionState == EGameActionState::InGameUIPause;
		}

		bool IsInMenu() const
		{
			return eGameActionState == EGameActionState::MainMenu ||
				eGameActionState == EGameActionState::SetupGame;
		}

		int32_t GetStackGameState() const
		{
			// v2 = *(_DWORD *)(result + 4LL * *(int *)(v5 + 0xE0) - 4);

			if (pGameStateStack && nGameStateStackSize > 0)
			{
				return pGameStateStack[nGameStateStackSize - 1];
			}
			return -1;
		}
	};

	class WorldState
	{
	public:
		char pad_0000[0x110]; // +0x0000
		CClient_WorldModel* pWorldModel; // +0x0110
		char pad_0118[0x11A0]; // +0x0118
		ETimeOfDayType timeOfDay; // +0x12B8
	};

	class Client
	{
	public:
		char pad_0000[0x128]; // +0x000
		CClient_WorldModel* pWorldModel; // +0x128
		uintptr_t pSubObject_130; // +0x130
		uintptr_t pSubObject_138; // +0x138
	};

	class CamelClient
	{
	public:
		char pad_0000[0x128]; // +0x000
		Client* pClient; // +0x128

		static CamelClient* GetInstance()
		{
			uintptr_t ptr = *(uintptr_t*)(0x1437805E0); // singleton instance address
			if (!ptr) return nullptr;
			return (CamelClient*)ptr;
		}

		CClient_WorldModel* GetWorldModel() const
		{
			if (!pClient) return nullptr;
			return pClient->pWorldModel;
		}
	};
}
