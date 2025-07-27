#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>

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

public:
    LoaderManager(const std::string& name = "FractureLoader", const std::string& configFile = "config.ini");
    void initialize();
    void addConfigItem(const ConfigItem& item);

    template<typename T>
    T getValue(const std::string& key, const T& defaultValue = T{}) {
        auto it = configValues.find(key);
        if (it != configValues.end()) {
            if (std::holds_alternative<T>(it->second)) {
                return std::get<T>(it->second);
            }
        }
        return defaultValue;
    }

    template<typename T>
    void setValue(const std::string& key, const T& value) {
        configValues[key] = value;
    }

    std::string getBaseDirectory() const { return baseDirectory; }
    std::string getModsDirectory() const { return modsDirectory; }
    std::string getConfigPath() const { return configPath; }

    void saveConfig() {
        std::ofstream configFile(configPath);
        if (!configFile.is_open()) return;

        for (const auto& item : configSchema) {
            for (const auto& comment : item.additionalComments) {
                configFile << "# " << comment << "\n";
            }

            if (!item.comment.empty()) {
                configFile << "# " << item.comment << "\n";
            }

            auto it = configValues.find(item.key);
            if (it != configValues.end()) {
                configFile << item.key << "=" << valueToString(it->second) << "\n\n";
            }
            else {
                configFile << item.key << "=" << valueToString(item.defaultValue) << "\n\n";
            }
        }

        configFile.close();
    };

    std::unordered_map<std::string, std::string> getAllValuesAsStrings();

private:
    std::string loaderName;
    std::string baseDirectory;
    std::string configFileName;
    std::string configPath;
    std::string modsDirectory;
    std::unordered_map<std::string, ConfigValue> configValues;
    std::vector<ConfigItem> configSchema;

    std::string valueToString(const ConfigValue& value);
    ConfigValue stringToValue(const std::string& str, const ConfigValue& defaultVal);
    std::string getExecutableDirectory();
    void createDirectoryStructure();
    void createDefaultConfig();
    void loadConfig();
};

extern LoaderManager* g_loaderManager;

void setupLoader();
LoaderManager* createLoaderManager(const std::string& name = "FractureLoader",
    const std::string& configFile = "config.ini");
