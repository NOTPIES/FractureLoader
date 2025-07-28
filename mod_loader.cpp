#include "mod_loader.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include "logger.h"
#include "global.h"
#include "functions.h"
#include "hooks.h"
#include <set>

namespace fs = std::filesystem;

static std::string to_lower(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

ModLoader::ModLoader(const std::string& modsDir) : modsDirectory(modsDir) {
    loadModStates();
}

std::string ModLoader::trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> ModLoader::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        token = trim(token);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

ModManifest ModLoader::parseManifest(const std::string& manifestPath) {
    ModManifest manifest;

    if (!fs::exists(manifestPath)) {
        return manifest;
    }

    std::ifstream file(manifestPath);
    if (!file.is_open()) {
        return manifest;
    }

    std::string line;
    while (std::getline(file, line)) {
        // skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }

        size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) {
            continue;
        }

        std::string key = trim(line.substr(0, equalPos));
        std::string value = trim(line.substr(equalPos + 1));

        // we remove quotes if present
        if (value.length() >= 2 && value[0] == '"' && value[value.length() - 1] == '"') {
            value = value.substr(1, value.length() - 2);
        }

        if (key == "name") {
            manifest.name = value;
        }
        else if (key == "author") {
            manifest.author = value;
        }
        else if (key == "version") {
            manifest.version = value;
        }
        else if (key == "description") {
            manifest.description = value;
        }
        else if (key == "dependencies") {
            manifest.dependencies = split(value, ',');
        }
        else if (key == "preloadCustomAssets") {
            manifest.preloadCustomAssets = (to_lower(value) == "true" || value == "1");
        }
        else {
            FRACTURE_WARN("Unknown key '{}' in mod manifest: {}", key, manifestPath);
        }
    }

    file.close();

    manifest.valid = true;

    if (manifest.name.empty()) {
        manifest.valid = false;
        FRACTURE_ERROR("Mod manifest is missing 'name' field in {}", manifestPath);
    }

    if (manifest.author.empty()) {
        manifest.valid = false;
        FRACTURE_ERROR("Mod manifest is missing 'author' field in {}", manifestPath);
    }

    if (manifest.version.empty()) {
        manifest.valid = false;
        FRACTURE_ERROR("Mod manifest is missing 'version' field in {}", manifestPath);
    }

    return manifest;
}

bool ModLoader::buildFileIndex(ModEntry& mod) {
    mod.allFiles.clear();

    if (!fs::exists(mod.modPath) || !fs::is_directory(mod.modPath))
        return false;

    for (const auto& entry : fs::recursive_directory_iterator(mod.modPath)) {
        if (!entry.is_regular_file())
            continue;

        if (entry.path().filename() == "manifest.ini")
            continue;

        std::string relativePath = fs::relative(entry.path(), mod.modPath).string();
        mod.allFiles.insert(Global::normalizePath(relativePath));

        FRACTURE_DEBUG("Indexed file: {} in mod: {}", relativePath, mod.name);
    }
    return true;
}

void ModLoader::scanMods() {
    loadedMods.clear();

    if (!fs::exists(modsDirectory)) {
        return;
    }

    FRACTURE_MSG("Loading mods from directory: {}", modsDirectory);

    loadedMods.push_back(ModEntry{
        "FractureLoader", "notpies", Global::fractureLoaderVersion,
            "A mod loader for South Park: The Fractured But Whole.", "", true, false, true
        });

    for (const auto& entry : fs::directory_iterator(modsDirectory)) {
        if (!entry.is_directory()) {
            continue;
        }

        std::string modDir = entry.path().string();
        std::string manifestPath = modDir + "\\manifest.ini";

        ModManifest manifest = parseManifest(manifestPath);
        if (!manifest.valid) {
            FRACTURE_ERROR("Invalid mod manifest in directory: {}, the mod will not be loaded.", modDir);
            continue;
        }

        ModEntry mod;
        mod.name = manifest.name;
        mod.author = manifest.author.empty() ? "Unknown" : manifest.author;
        mod.version = manifest.version.empty() ? "1.0.0" : manifest.version;
        mod.description = manifest.description.empty() ? "No description provided" : manifest.description;
        mod.modPath = modDir;
        mod.enabled = isModEnabled(mod.name);
        mod.hasConflict = false; // will be set by detectConflicts()
        mod.baseMod = false; // reserved for internal use
        mod.preloadCustomAssets = manifest.preloadCustomAssets;

        // TODO: dependencies

        if (mod.enabled)
            buildFileIndex(mod);

        loadedMods.push_back(mod);

        FRACTURE_SETLOGCOLOR(FractureLogger::Color::GREEN);

        FRACTURE_MSG_ARGS_WHITE("Mod loaded: {} by {} (v{}), Enabled: {}",
            mod.name,
            mod.author,
            mod.version,
            mod.enabled ? "Yes" : "No");

        FRACTURE_RESETLOGCOLOR();
    }

    detectConflicts();

    preloadCustomModAssets();
}

std::string ModLoader::findFileOverride(const std::string& originalPath) {
    std::lock_guard<std::mutex> lock(overrideCacheMutex);

    std::string normalizedPath = Global::normalizePath(originalPath);

    auto cacheIt = fileOverrideCache.find(normalizedPath);
    if (cacheIt != fileOverrideCache.end()) {
        return cacheIt->second;
    }

    std::vector<ModEntry> enabledMods = getEnabledMods();
    for (auto it = enabledMods.rbegin(); it != enabledMods.rend(); ++it) {
        const ModEntry& mod = *it;

        if (mod.baseMod) continue;
        if (mod.hasConflict) continue;

        if (mod.allFiles.count(normalizedPath) > 0) {
            std::string overridePath = mod.modPath + "\\" + normalizedPath;

            FRACTURE_DEBUG("File override found: {} -> {} (from mod: {})",
                originalPath, overridePath, mod.name);

            fileOverrideCache[normalizedPath] = overridePath;
            return overridePath;
        }
    }

    fileOverrideCache[normalizedPath] = "";
    return "";
}

bool ModLoader::checkCustomAssetAvailability(const std::string& normalizedPath) {
    {
        std::lock_guard<std::mutex> lock(preloadedAssetsMutex);
        if (preloadedCustomAssets.find(normalizedPath) != preloadedCustomAssets.end()) {
            return true;
        }
    }

    std::vector<ModEntry> enabledMods = getEnabledMods();
    for (auto it = enabledMods.rbegin(); it != enabledMods.rend(); ++it) {
        const ModEntry& mod = *it;
        if (mod.baseMod || mod.hasConflict) continue;

        if (mod.allFiles.count(normalizedPath) > 0) {
            return true;
        }
    }

    return false;
}


void ModLoader::preloadCustomModAssets()
{
    for (const auto& mod : loadedMods) {
        if (!mod.preloadCustomAssets)
            continue;

        if (mod.baseMod)
            continue;

        FRACTURE_DEBUG("Preloading all assets for mod: {}", mod.name);

        for (const auto& relativePath : mod.allFiles) {
            if (relativePath == "manifest.ini")
                continue;

            _BYTE buffer[256] = {};
            strncpy_s(reinterpret_cast<char*>(buffer), sizeof(buffer), relativePath.c_str(), _TRUNCATE);

            if (!SnowdropIsFileAvailable(buffer, 0)) {
                FRACTURE_DEBUG("Loading custom asset into Snowdrop Engine: {}", relativePath);

                if (SnowdropOpenFile) {
                    char ctxBuffer[0x58] = {}; // temporary buffer for context
                    void* ctx = SnowdropInitFileStruct(ctxBuffer);

                    if (ctx && snowdropOpenFileHook((__int64)ctx, (_BYTE*)relativePath.c_str(), 1)) {
                        FRACTURE_DEBUG("Successfully preloaded custom asset: {}", relativePath);

                        typedef __int64(__fastcall* FuncType2)(__int64);
                        FuncType2 sub_14B13CD50 = (FuncType2)0x14B13CD50;
                        sub_14B13CD50(reinterpret_cast<__int64>(ctx));
                    }
                    else {
                        FRACTURE_ERROR("Failed to preload custom asset: {}", relativePath);
                    }
                }
                else {
                    FRACTURE_ERROR("SnowdropOpenFile function is not available, cannot preload custom assets, mods may not work correctly!");
                }
            }
        }
    }
}

void ModLoader::detectConflicts() {
    for (auto& mod : loadedMods) {
        mod.hasConflict = false;
    }

    // name conflicts
    for (size_t i = 0; i < loadedMods.size(); ++i) {
        for (size_t j = i + 1; j < loadedMods.size(); ++j) {
            if (loadedMods[i].name == loadedMods[j].name) {
                loadedMods[i].hasConflict = true;
                loadedMods[j].hasConflict = true;
                FRACTURE_WARN("Conflict: Duplicate mod name '{}' between '{}' and '{}'",
                    loadedMods[i].name, loadedMods[i].modPath, loadedMods[j].modPath);
            }
        }
    }

    // file conflicts
    for (size_t i = 0; i < loadedMods.size(); ++i) {
        if (loadedMods[i].baseMod || !loadedMods[i].enabled)
            continue;

        for (size_t j = i + 1; j < loadedMods.size(); ++j) {
            if (loadedMods[j].baseMod || !loadedMods[j].enabled)
                continue;

            for (const auto& file : loadedMods[i].allFiles) {
                if (file == "manifest.ini")
                    continue;

                if (loadedMods[j].allFiles.count(file) > 0) {
                    loadedMods[i].hasConflict = true;
                    loadedMods[j].hasConflict = true;
                    FRACTURE_WARN("Conflict: File '{}' exists in both '{}' and '{}'",
                        file, loadedMods[i].name, loadedMods[j].name);
                }
            }
        }
    }
}

void ModLoader::loadModStates() {
    std::string statePath = modsDirectory + "\\..\\mod_states.ini";

    if (!fs::exists(statePath)) {
        return;
    }

    std::ifstream file(statePath);
    if (!file.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) {
            continue;
        }

        std::string modName = trim(line.substr(0, equalPos));
        std::string enabledStr = trim(line.substr(equalPos + 1));

        bool enabled = (enabledStr == "true" || enabledStr == "1");
        modEnabledState[modName] = enabled;
    }

    file.close();
}

void ModLoader::saveModStates() {
    std::string statePath = modsDirectory + "\\..\\mod_states.ini";

    std::ofstream file(statePath);
    if (!file.is_open()) {
        return;
    }

    file << "# Mod enabled states\n";
    file << "# Format: ModName=true/false\n\n";

    for (const auto& pair : modEnabledState) {
        const ModEntry* mod = getModByName(pair.first);
        if (mod && mod->baseMod) {
            continue;
        }

        file << pair.first << "=" << (pair.second ? "true" : "false") << "\n";
    }

    file.close();
}

void ModLoader::setModEnabled(const std::string& modName, bool enabled) {
    ModEntry* mod = getModByName(modName);
    if (!mod) {
        return;
    }

    bool wasEnabled = mod->enabled;
    modEnabledState[modName] = enabled;

    for (auto& m : loadedMods) {
        if (m.baseMod) {
            modEnabledState.erase(m.name);
            break;
        }

        if (m.name == modName) {
            m.enabled = enabled;
            break;
        }
    }

    detectConflicts();
}

bool ModLoader::isModEnabled(const std::string& modName) const {
    auto it = modEnabledState.find(modName);
    return it != modEnabledState.end() ? it->second : true; // enabled by default
}

std::vector<ModEntry> ModLoader::getEnabledMods() const {
    std::vector<ModEntry> enabledMods;
    for (const auto& mod : loadedMods) {
        if (mod.enabled && !mod.baseMod) {
            enabledMods.push_back(mod);
        }
    }
    return enabledMods;
}

void ModLoader::saveCurrentStates() {
    for (const auto& mod : loadedMods) {
        modEnabledState[mod.name] = mod.enabled;
    }

    saveModStates();
}

void ModLoader::refresh() {
    scanMods();
}

ModEntry* ModLoader::getModByName(const std::string& name) {
    for (auto& mod : loadedMods) {
        if (mod.name == name) {
            return &mod;
        }
    }
    return nullptr;
}

ModLoader* g_modLoader = nullptr;