#pragma once
#include <cstdint>

namespace Snowdrop
{
	template <typename T>
	T SafeRead(uintptr_t address)
	{
		if (!address) return T{};
		return *reinterpret_cast<T*>(address);
	}
}
