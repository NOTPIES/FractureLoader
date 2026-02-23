#pragma once
#include <cstdint>

namespace Snowdrop
{
	struct NodeInnerBlock
	{
		uintptr_t outputValue; // 0x00
		uint8_t padding[0x1C - sizeof(uintptr_t)]; // pad 0x1C
		uint32_t flags; // Offset 0x1C
	};

	struct NodeGenericInputAttribute
	{
		uintptr_t SelfPointer; // 0x00
		uint8_t padding_to_1C[0x1C - sizeof(uintptr_t)];
		uint32_t FlagsOrTypeInfo; // 0x1C (set to 0x0C000001 for fast path)
		uint8_t padding_for_0x50[0x50 - (0x1C + 0x4)];
		uintptr_t ObjectPointer; // 0x50
	};

	struct NodeContainer
	{
		uint8_t padding1[0x20]; // pad 0x20
		NodeInnerBlock* pInnerBlock; // 0x20
		NodeGenericInputAttribute* pInput1; // 0x28
	};

	using NodeFunction_t = __int64(__fastcall*)(__int64 a1, __int64 a2);
}
