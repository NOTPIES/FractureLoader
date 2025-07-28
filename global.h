#pragma once
#include <string>
#include "cclient_enums.h"

namespace Global
{
    inline const std::string fractureLoaderVersion = "1.0.0";
    inline const std::string fractureLoaderRepo = "https://github.com/NOTPIES/FractureLoader";

    inline std::string engineVersion = "@Snowdrop/0";

    inline std::string normalizePath(const std::string& path) {
        std::string normalized = path;
        std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
        std::replace(normalized.begin(), normalized.end(), '\\', '/');
        return normalized;
    }

    inline CClient_PlatformEnum g_currentPlatform = CClient_PlatformEnum::PC;

    inline const char* buildDate = __DATE__;
    inline const char* buildTime = __TIME__;
}