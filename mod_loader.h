#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <unordered_set>

struct ModEntry {
    std::string name;
    std::string author;
    std::string version;
    std::string description;
    std::string modPath;
    bool enabled = false;
    bool hasConflict = false;
    bool baseMod = false;
    bool preloadCustomAssets = false;

    std::unordered_set<std::string> allFiles;

    const char* getName() const { return name.c_str(); }
    const char* getAuthor() const { return author.c_str(); }
    const char* getVersion() const { return version.c_str(); }
    const char* getDescription() const { return description.c_str(); }
};

struct ModManifest {
    std::string name;
    std::string author;
    std::string version;
    std::string description;
    std::vector<std::string> dependencies;
    std::vector<std::string> conflicts;
    bool preloadCustomAssets = false;
    bool valid = false;
};

class ModLoader {
private:
    std::string modsDirectory;
    std::vector<ModEntry> loadedMods;
    std::unordered_map<std::string, bool> modEnabledState;

    ModManifest parseManifest(const std::string& manifestPath);

    void detectConflicts();

    void loadModStates();
    void saveModStates();

    bool buildFileIndex(ModEntry& mod);

    std::string trim(const std::string& str);
    std::vector<std::string> split(const std::string& str, char delimiter);

public:
    std::unordered_map<std::string, std::string> fileOverrideCache;
    std::mutex overrideCacheMutex;

    std::unordered_map<std::string, bool> fileAvailabilityCache;
    std::mutex availabilityCacheMutex;

    std::unordered_set<std::string> preloadedCustomAssets;
    std::mutex preloadedAssetsMutex;

    ModLoader(const std::string& modsDir);

    void scanMods();

    std::string findFileOverride(const std::string& originalPath);
    void preloadCustomModAssets();
    bool checkCustomAssetAvailability(const std::string& normalizedPath);

    const std::vector<ModEntry>& getMods() const { return loadedMods; }
    std::vector<ModEntry>& getMods() { return loadedMods; }

    void setModEnabled(const std::string& modName, bool enabled);
    bool isModEnabled(const std::string& modName) const;

    std::vector<ModEntry> getEnabledMods() const;

    void saveCurrentStates();

    void refresh();

    ModEntry* getModByName(const std::string& name);
};

extern ModLoader* g_modLoader;