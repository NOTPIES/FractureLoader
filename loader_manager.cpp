#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <variant>
#include <functional>
#include "global.h"

namespace fs = std::filesystem;

using ConfigValue = std::variant<bool, int, float, std::string>;

struct ConfigItem {
    std::string key;
    ConfigValue defaultValue;
    std::string comment;
    std::vector<std::string> additionalComments;

    ConfigItem(const std::string& k, const ConfigValue& def, const std::string& desc = "",
        const std::vector<std::string>& extra = {})
        : key(k), defaultValue(def), comment(desc), additionalComments(extra) {
    }
};

class LoaderManager {
private:
    std::string loaderName;
    std::string baseDirectory;
    std::string configFileName;
    std::string configPath;
    std::string modsDirectory;
    std::unordered_map<std::string, ConfigValue> configValues;
    std::vector<ConfigItem> configSchema;

    std::string valueToString(const ConfigValue& value) {
        return std::visit([](const auto& v) -> std::string {
            if constexpr (std::is_same_v<std::decay_t<decltype(v)>, bool>) {
                return v ? "true" : "false";
            }
            else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, std::string>) {
                return v;
            }
            else {
                return std::to_string(v);
            }
            }, value);
    }

    ConfigValue stringToValue(const std::string& str, const ConfigValue& defaultVal) {
        return std::visit([&str](const auto& def) -> ConfigValue {
            using T = std::decay_t<decltype(def)>;

            if constexpr (std::is_same_v<T, bool>) {
                std::string lower = str;
                std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
                return (lower == "true" || lower == "1" || lower == "yes" || lower == "on");
            }
            else if constexpr (std::is_same_v<T, int>) {
                try { return std::stoi(str); }
                catch (...) { return def; }
            }
            else if constexpr (std::is_same_v<T, float>) {
                try { return std::stof(str); }
                catch (...) { return def; }
            }
            else if constexpr (std::is_same_v<T, std::string>) {
                return str;
            }
            return def;
            }, defaultVal);
    }

    std::string getExecutableDirectory() {
        char exePath[MAX_PATH];
        GetModuleFileNameA(NULL, exePath, MAX_PATH);

        std::string exeDir = exePath;
        size_t lastSlash = exeDir.find_last_of("\\/");
        if (lastSlash != std::string::npos) {
            exeDir = exeDir.substr(0, lastSlash);
        }
        return exeDir;
    }

    void createDirectoryStructure() {
        try {
            if (!fs::exists(baseDirectory)) {
                fs::create_directories(baseDirectory);
            }

            if (!fs::exists(modsDirectory)) {
                fs::create_directories(modsDirectory);
            }

            std::string readmePath = modsDirectory + "\\README.txt";
            if (!fs::exists(readmePath)) {
                std::ofstream readme(readmePath);
                if (readme.is_open()) {
                    readme << "Mods Directory\n";
                    readme << "==============\n\n";
                    readme << "Place your mods in this directory.\n";
                    readme << "Each mod should be in its own subdirectory.\n";
                    readme << "Example structure:\n";
                    readme << "  mods/\n";
                    readme << "    └── MyMod/\n";
                    readme << "        ├── camel\n";
                    readme << "        │     └── Place here files to be used on the mod\n";
                    readme << "        └── manifest.ini\n";
                    readme.close();
                }
            }

        }
        catch (const std::exception& e) {
			//FRACTURE_ERROR("Failed to create directory structure: {}", e.what());
        }
    }

    void createDefaultConfig() {
        std::ofstream configFile(configPath);
        if (!configFile.is_open()) return;

        for (const auto& item : configSchema) {
            for (const auto& comment : item.additionalComments) {
                configFile << "# " << comment << "\n";
            }

            if (!item.comment.empty()) {
                configFile << "# " << item.comment << "\n";
            }

            configFile << item.key << "=" << valueToString(item.defaultValue) << "\n\n";
        }

        configFile.close();
    }

    void loadConfig() {
        std::ifstream configFile(configPath);
        if (!configFile.is_open()) return;

        std::string line;
        while (std::getline(configFile, line)) {
            if (line.empty() || line[0] == '#') continue;

            size_t equalPos = line.find('=');
            if (equalPos == std::string::npos) continue;

            std::string key = line.substr(0, equalPos);
            std::string value = line.substr(equalPos + 1);

            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            auto schemaIt = std::find_if(configSchema.begin(), configSchema.end(),
                [&key](const ConfigItem& item) { return item.key == key; });

            if (schemaIt != configSchema.end()) {
                configValues[key] = stringToValue(value, schemaIt->defaultValue);
            }
        }
        configFile.close();
    }

public:
    LoaderManager(const std::string& name = "FractureLoader",
        const std::string& configFile = "config.ini")
        : loaderName(name), configFileName(configFile) {

        std::string exeDir = getExecutableDirectory();
        baseDirectory = exeDir + "\\" + name;
        modsDirectory = baseDirectory + "\\mods";
        configPath = baseDirectory + "\\" + configFileName;
    }

    void initialize() {
        createDirectoryStructure();

        if (!fs::exists(configPath)) {
            createDefaultConfig();
        }

        loadConfig();
    }

    void addConfigItem(const ConfigItem& item) {
        configSchema.push_back(item);
        configValues[item.key] = item.defaultValue;
    }

    std::unordered_map<std::string, std::string> getAllValuesAsStrings() {
        std::unordered_map<std::string, std::string> result;
        for (const auto& pair : configValues) {
            result[pair.first] = valueToString(pair.second);
        }
        return result;
    }
};

LoaderManager* g_loaderManager = nullptr;

void setupLoader() {
    if (!g_loaderManager) return;

    g_loaderManager->addConfigItem(ConfigItem(
        "debugLog",
        true,
        "Enable debug logging - outputs detailed information to help with troubleshooting",
        { "Set to true to enable debug messages, false to disable" }
    ));

    g_loaderManager->addConfigItem(ConfigItem(
        "menuToggleKey",
        (int)VK_INSERT, // default key is Insert key
        "Key to toggle the menu visibility"
    ));

	g_loaderManager->addConfigItem(ConfigItem(
		"menuInputPassthrough",
		false,
		"Enable input passthrough when the menu is open - allows mouse and keyboard input to pass through to the game",
		{ "Set to true to allow input passthrough, false to block input while the menu is open" }
	));

	g_loaderManager->addConfigItem(ConfigItem(
		"spoofedPlatform",
		(int)CClient_PlatformEnum::PC,
		"Sets the platform type that the game reports, can influence platform-specific behavior, assets, or UI elements."
	));

    // TODO for 1.0.1
    /*g_loaderManager->addConfigItem(ConfigItem(
        "modLoadingMethod",
        1,
        "Sets the method that FractureLoader will use to load overriden game files, default method is 1.",
        { "Method 1: Loads overriden files when they are requested by the engine at runtime (SLOWER)", "Method 2: Maps and caches all the mods file list on startup, and they are given to the engine when requested, faster but takes some more time for the game to start." }
    ));*/

    g_loaderManager->initialize();
}

LoaderManager* createLoaderManager(const std::string& name = "FractureLoader",
    const std::string& configFile = "config.ini") {
    return new LoaderManager(name, configFile);
}