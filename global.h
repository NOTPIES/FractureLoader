#pragma once
#include <string>
#include "cclient_enums.h"

namespace Global
{
	inline const std::string fractureLoaderVersion = "1.1.0";
	inline const std::string fractureLoaderRepo = "https://github.com/NOTPIES/FractureLoader";

	inline std::string engineVersion = "@Snowdrop/0";

	inline std::string normalizePath(const std::string& path)
	{
		std::string normalized = path;
		std::transform(normalized.begin(), normalized.end(), normalized.begin(), tolower);
		std::replace(normalized.begin(), normalized.end(), '\\', '/');
		return normalized;
	}

	inline CClient_PlatformEnum g_currentPlatform = PC;

	inline auto buildDate = __DATE__;
	inline auto buildTime = __TIME__;
}
