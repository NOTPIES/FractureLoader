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
#include "loader_manager.h"

namespace fs = std::filesystem;

using ConfigValue = std::variant<bool, int, float, std::string>;

std::string LoaderManager::valueToString(const ConfigValue& value) const
{
	return std::visit([](const auto& v) -> std::string
	{
		using T = std::decay_t<decltype(v)>;

		if constexpr (std::is_same_v<T, bool>)
		{
			return v ? "true" : "false";
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			return v;
		}
		else
		{
			return std::to_string(v);
		}
	}, value);
}

ConfigValue LoaderManager::stringToValue(const std::string& str, const ConfigValue& defaultVal) const
{
	return std::visit([&str]<typename T0>(const T0& def) -> ConfigValue
	{
		using T = std::decay_t<T0>;

		if constexpr (std::is_same_v<T, bool>)
		{
			std::string lower = str;
			std::ranges::transform(lower, lower.begin(), tolower);
			return (lower == "true" || lower == "1" || lower == "yes" || lower == "on");
		}
		else if constexpr (std::is_same_v<T, int>)
		{
			try { return std::stoi(str); }
			catch (...) { return def; }
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			try { return std::stof(str); }
			catch (...) { return def; }
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			return str;
		}

		return def;
	}, defaultVal);
}

std::string LoaderManager::getExecutableDirectory()
{
	char exePath[MAX_PATH];
	GetModuleFileNameA(nullptr, exePath, MAX_PATH);

	std::string exeDir = exePath;
	size_t lastSlash = exeDir.find_last_of("\\/");
	if (lastSlash != std::string::npos)
		exeDir = exeDir.substr(0, lastSlash);

	return exeDir;
}

void LoaderManager::createDirectoryStructure() const
{
	try
	{
		if (!fs::exists(baseDirectory))
		{
			fs::create_directories(baseDirectory);
		}

		if (!fs::exists(modsDirectory))
		{
			fs::create_directories(modsDirectory);
		}

		std::string readmePath = modsDirectory + "\\README.txt";
		if (!fs::exists(readmePath))
		{
			std::ofstream readme(readmePath);
			if (readme.is_open())
			{
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
	catch (const std::exception& e)
	{
		//FRACTURE_ERROR("Failed to create directory structure: {}", e.what());
	}
}

void LoaderManager::createDefaultConfig()
{
	std::ofstream configFile(configPath);
	if (!configFile.is_open()) return;

	for (const auto& item : configSchema)
	{
		for (const auto& comment : item.additionalComments)
		{
			configFile << "# " << comment << "\n";
		}

		if (!item.comment.empty())
		{
			configFile << "# " << item.comment << "\n";
		}

		configFile << item.key << "=" << valueToString(item.defaultValue) << "\n\n";
	}

	configFile.close();
}

void LoaderManager::saveConfig()
{
	std::ofstream file(configPath);
	if (!file.is_open()) return;

	for (const auto& item : configSchema)
	{
		for (const auto& comment : item.additionalComments)
			file << "# " << comment << "\n";

		if (!item.comment.empty())
			file << "# " << item.comment << "\n";

		auto it = configValues.find(item.key);
		if (it != configValues.end())
			file << item.key << "=" << valueToString(it->second) << "\n\n";
		else
			file << item.key << "=" << valueToString(item.defaultValue) << "\n\n";
	}
}

void LoaderManager::addMissingKeys()
{
	bool modified = false;

	for (const auto& item : configSchema)
	{
		if (!configValues.contains(item.key))
		{
			configValues[item.key] = item.defaultValue;
			modified = true;
		}
	}

	if (modified)
		saveConfig();
}

void LoaderManager::loadConfig()
{
	std::ifstream file(configPath);
	if (!file.is_open()) return;

	std::string line;
	while (std::getline(file, line))
	{
		if (line.empty() || line[0] == '#') continue;

		size_t equalPos = line.find('=');
		if (equalPos == std::string::npos) continue;

		std::string key = line.substr(0, equalPos);
		std::string val = line.substr(equalPos + 1);

		key.erase(0, key.find_first_not_of(" \t"));
		key.erase(key.find_last_not_of(" \t") + 1);

		val.erase(0, val.find_first_not_of(" \t"));
		val.erase(val.find_last_not_of(" \t") + 1);

		auto schemaIt = std::find_if(configSchema.begin(), configSchema.end(),
		                             [&](const ConfigItem& item) { return item.key == key; });

		if (schemaIt != configSchema.end())
			configValues[key] = stringToValue(val, schemaIt->defaultValue);
	}
}

LoaderManager::LoaderManager(const std::string& name, const std::string& cfg)
	: loaderName(name), configFileName(cfg)
{
	std::string exeDir = getExecutableDirectory();
	baseDirectory = exeDir + "\\" + loaderName;
	modsDirectory = baseDirectory + "\\mods";
	configPath = baseDirectory + "\\" + configFileName;
}

void LoaderManager::initialize()
{
	createDirectoryStructure();

	if (!fs::exists(configPath))
		createDefaultConfig();

	loadConfig();
	addMissingKeys();
}

void LoaderManager::addConfigItem(const ConfigItem& item)
{
	configSchema.push_back(item);
}

std::unordered_map<std::string, std::string> LoaderManager::getAllValuesAsStrings()
{
	std::unordered_map<std::string, std::string> out;
	for (auto& p : configValues)
		out[p.first] = valueToString(p.second);
	return out;
}

LoaderManager* g_loaderManager = nullptr;

void setupLoader()
{
	if (!g_loaderManager) return;

	g_loaderManager->addConfigItem(ConfigItem(
		"debugLog",
		false,
		"Enable debug logging - outputs detailed information to help with troubleshooting",
		{"Set to true to enable debug messages, false to disable"}
	));

	g_loaderManager->addConfigItem(ConfigItem(
		"menuToggleKey",
		VK_INSERT, // default key is Insert key
		"Key to toggle the menu visibility"
	));

	g_loaderManager->addConfigItem(ConfigItem(
		"menuInputPassthrough",
		false,
		"Enable input passthrough when the menu is open - allows mouse and keyboard input to pass through to the game",
		{"Set to true to allow input passthrough, false to block input while the menu is open"}
	));

	g_loaderManager->addConfigItem(ConfigItem(
		"noMenu",
		false,
		"Disables the FractureLoader menu entirely.",
		{
			"A game restart is required after changing this, and the state can also be modified through the FractureLoader config file."
		}
	));

	g_loaderManager->addConfigItem(ConfigItem(
		"spoofedPlatform",
		PC,
		"Sets the platform type that the game reports, can influence platform-specific behavior, assets, or UI elements."
	));

	g_loaderManager->initialize();
}

LoaderManager* createLoaderManager(const std::string& name = "FractureLoader",
                                   const std::string& configFile = "config.ini")
{
	return new LoaderManager(name, configFile);
}
