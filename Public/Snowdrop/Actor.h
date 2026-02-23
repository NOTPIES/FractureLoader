#pragma once
#include <cstdint>
#include <windows.h>
#include <vector>
#include <cstdio>
#include "Node.h"
#include "Structs.h"
#include "Utils.h"
#include "MCommon/MC_Vector.h"

namespace Snowdrop::Actor
{
	// Client_Node_Actor_GetLocalPlayer
	inline uintptr_t GetLocalPlayer()
	{
		auto getLocalPlayerFunc = reinterpret_cast<NodeFunction_t>(0x145A46710);

		NodeInnerBlock innerBlock = {};
		innerBlock.outputValue = 0;
		innerBlock.flags = 0x1;

		NodeContainer nodeContainer = {};
		nodeContainer.pInnerBlock = &innerBlock;

		getLocalPlayerFunc(0, reinterpret_cast<__int64>(&nodeContainer));

		uintptr_t result = innerBlock.outputValue;

		return result;
	}

	// works same as CClient_Node_Actor_GetMainCharacter
	inline uintptr_t GetMainCharacter()
	{
		uintptr_t localPlayerComp = GetLocalPlayer();

		// MainCharacter is localPlayerComp + 0x50

		if (!localPlayerComp)
		{
			return 0;
		}

		return SafeRead<uintptr_t>(localPlayerComp + 0x50);
	}


	inline bool IsActorPlayerControlled(uintptr_t actor)
	{
		NodeGenericInputAttribute inputAttr = {};
		inputAttr.SelfPointer = reinterpret_cast<uintptr_t>(&inputAttr);
		inputAttr.FlagsOrTypeInfo = 0x0C000001;
		inputAttr.ObjectPointer = actor;

		NodeInnerBlock innerBlock = {};
		innerBlock.outputValue = 0;
		innerBlock.flags = 0x1;

		NodeContainer nodeContainer = {};
		nodeContainer.pInnerBlock = &innerBlock;
		nodeContainer.pInput1 = &inputAttr;

		auto isPlayerControlledFunc = reinterpret_cast<NodeFunction_t>(0x145A4A1C0);

		isPlayerControlledFunc(0, reinterpret_cast<__int64>(&nodeContainer));

		return *reinterpret_cast<bool*>(&innerBlock.outputValue);
	}

	inline MC_Vector3f GetActorPosition(uintptr_t actor)
	{
		return SafeRead<MC_Vector3f>(actor + 0x80);
	}

	inline MC_Vector3f GetActorRotation(uintptr_t actor)
	{
		return SafeRead<MC_Vector3f>(actor + 0x8C);
	}

	inline MC_Vector3f GetActorScale(uintptr_t actor)
	{
		return SafeRead<MC_Vector3f>(actor + 0x98);
	}
}
