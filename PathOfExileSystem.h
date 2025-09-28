#pragma once
#include "TimeTurner.h"
#include "BluejayArchetype.h"
#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <functional>
#include <random>

// Path of Exile-style item system
struct Item {
    std::string baseType;                  // "Pistol", "Horse", "Energy Sword", "Cowboy Hat"
    int rarity;                            // 0=normal, 1=magic, 2=rare, 3=unique, 4=legendary
    std::map<std::string, float> affixes;  // "Fire Damage": 12.5, "Life Steal": 5.0
    int itemLevel = 1;
    int sockets = 0;
    std::vector<std::string> socketedGems;
    float durability = 100.0f;
    std::string flavorText = "";
    
    // PoE-style methods
    void addAffix(const std::string& name, float value);
    void removeAffix(const std::string& name);
    bool hasAffix(const std::string& name) const;
    float getAffixValue(const std::string& name) const;
    std::string getRarityName() const;
    glm::vec3 getRarityColor() const;
    std::string getDescription() const;
    bool canSocketGem() const;
    void socketGem(const std::string& gemName);
    void unsocketGem(int socketIndex);
};

// Ore/Resource system for procedural world
struct OreNode {
    glm::vec3 position;
    std::string type;           // "Iron", "Gold", "Hellstone", "Mystic Quartz", "Cobalt"
    int richness = 100;         // 0-100, affects yield
    int remainingOre = 100;     // How much ore is left
    float respawnTime = 300.0f; // 5 minutes default
    float currentRespawnTime = 0.0f;
    bool isDepleted = false;
    std::string biome;          // Which biome this ore belongs to
    int minLevel = 1;           // Minimum level to mine
    std::vector<std::string> specialDrops; // Rare drops from this node
    
    void updateNode(float deltaTime);
    int mineOre(int amount, int playerLevel);
    void respawn();
    bool canMine(int playerLevel) const;
    std::string getDescription() const;
};

// Map zone system (PoE-style zones)
struct MapZone {
    std::string biome;                    // "city", "wasteland", "hell", "frontier"
    std::string layout;                  // Template name for map generation
    std::vector<OreNode> oreNodes;
    std::vector<HazardEvent> hazards;
    std::vector<LootChest> lootChests;
    std::vector<SpawnPoint> spawnPoints;
    std::string bossName = "";
    int zoneLevel = 1;
    float lootBonus = 1.0f;
    float experienceBonus = 1.0f;
    std::map<std::string, float> modifiers; // Zone-specific modifiers
    
    void updateZone(float deltaTime);
    void addOreNode(const OreNode& node);
    void removeOreNode(const glm::vec3& position);
    OreNode* getOreNodeAt(const glm::vec3& position);
    std::vector<OreNode> getOreNodesByType(const std::string& type) const;
};

// League/Season system
struct League {
    std::string name;
    std::string description;
    std::vector<MapZone> zones;
    std::map<std::string, float> globalModifiers; // "LavaDmg": 2.0, "LootDropRate": 1.5
    std::vector<std::string> specialMechanics;   // "MeteorRain", "ZombieHarvest"
    int minLevel = 1;
    int maxLevel = 100;
    float difficulty = 1.0f;
    bool isActive = false;
    float duration = 0.0f;              // 0 = infinite
    float timeRemaining = 0.0f;
    std::string rewardType = "loot";    // "loot", "experience", "currency"
    
    void updateLeague(float deltaTime);
    void activateLeague();
    void deactivateLeague();
    bool isExpired() const;
    void addZone(const MapZone& zone);
    void removeZone(int index);
    MapZone* getZone(int index);
    std::vector<MapZone> getZonesByBiome(const std::string& biome) const;
};

// Skill tree system (PoE-style passive tree)
struct SkillNode {
    std::string id;
    std::string name;
    std::string description;
    std::vector<std::string> connections; // Connected node IDs
    std::map<std::string, float> bonuses; // "Damage": 10.0, "Speed": 5.0
    int cost = 1;                         // Skill points to unlock
    bool isKeystone = false;              // Major passive
    bool isAscendancy = false;            // Ascendancy node
    std::string archetype = "";           // Which archetype can use this
    int levelRequirement = 1;
    
    void addConnection(const std::string& nodeId);
    void removeConnection(const std::string& nodeId);
    bool isConnectedTo(const std::string& nodeId) const;
    void addBonus(const std::string& stat, float value);
    void removeBonus(const std::string& stat);
    float getBonus(const std::string& stat) const;
    std::string getDescription() const;
};

// Player build system
struct PlayerBuild {
    int playerId;
    std::string archetype;
    std::vector<std::string> unlockedNodes; // Skill node IDs
    std::vector<Item> inventory;
    std::vector<Item> stash;               // PoE-style stash
    int skillPoints = 0;
    int experience = 0;
    int level = 1;
    std::map<std::string, float> stats;    // Current stats from build
    std::string currentZone = "";
    int currentZoneIndex = 0;
    float playTime = 0.0f;
    
    void unlockNode(const std::string& nodeId);
    void lockNode(const std::string& nodeId);
    bool hasNode(const std::string& nodeId) const;
    void addItem(const Item& item);
    void removeItem(const std::string& itemId);
    Item* findItem(const std::string& itemId);
    void addToStash(const Item& item);
    void removeFromStash(const std::string& itemId);
    void levelUp();
    void addExperience(int exp);
    void updateStats();
    std::string getBuildDescription() const;
};

// Crafting system (PoE-style currency)
struct CraftingCurrency {
    std::string name;
    std::string description;
    int quantity = 0;
    std::string effect;                   // What it does when used
    std::string rarity = "common";        // How rare this currency is
    int value = 1;                        // Trade value
    
    void useCurrency(Item& item);
    bool canUseOn(const Item& item) const;
    std::string getEffectDescription() const;
};

// Loot chest system
struct LootChest {
    glm::vec3 position;
    std::vector<Item> contents;
    std::string chestType = "normal";     // "normal", "rare", "unique", "boss"
    bool isLocked = false;
    std::string keyType = "";             // What key opens this chest
    float respawnTime = 0.0f;
    float currentRespawnTime = 0.0f;
    bool isLooted = false;
    
    void updateChest(float deltaTime);
    std::vector<Item> lootChest(int playerId);
    void respawnChest();
    bool canLoot(int playerId) const;
    void addItem(const Item& item);
    void removeItem(const std::string& itemId);
    std::string getDescription() const;
};

// Spawn point system
struct SpawnPoint {
    glm::vec3 position;
    std::string spawnType = "player";     // "player", "enemy", "boss", "npc"
    int level = 1;
    std::string faction = "";
    bool isActive = true;
    float respawnTime = 60.0f;
    float currentRespawnTime = 0.0f;
    std::string lastSpawned = "";
    
    void updateSpawn(float deltaTime);
    bool canSpawn() const;
    void spawnEntity(const std::string& entityType);
    void resetSpawnTimer();
    std::string getDescription() const;
};

// Path of Exile main system
class PathOfExileSystem {
private:
    std::map<std::string, League> leagues;
    std::map<std::string, SkillNode> skillTree;
    std::map<int, PlayerBuild> playerBuilds;
    std::map<std::string, Item> itemDatabase;
    std::map<std::string, CraftingCurrency> currencyDatabase;
    std::vector<MapZone> currentZones;
    std::string activeLeague = "";
    bool systemActive = false;
    
    // Internal methods
    void generateItem(Item& item, int itemLevel, const std::string& baseType);
    void applyAffixes(Item& item, int rarity);
    void updateZones(float deltaTime);
    void updateLeagues(float deltaTime);
    void updatePlayerBuilds(float deltaTime);
    
public:
    PathOfExileSystem();
    ~PathOfExileSystem();
    
    // League management
    void createLeague(const League& league);
    void activateLeague(const std::string& leagueName);
    void deactivateLeague(const std::string& leagueName);
    void updateLeague(const std::string& leagueName, const League& league);
    void deleteLeague(const std::string& leagueName);
    
    // League queries
    std::vector<League> getActiveLeagues() const;
    std::vector<League> getLeaguesByLevel(int level) const;
    League* getLeague(const std::string& leagueName);
    const std::string& getActiveLeague() const { return activeLeague; }
    
    // Skill tree management
    void addSkillNode(const SkillNode& node);
    void removeSkillNode(const std::string& nodeId);
    void connectNodes(const std::string& nodeId1, const std::string& nodeId2);
    void disconnectNodes(const std::string& nodeId1, const std::string& nodeId2);
    SkillNode* getSkillNode(const std::string& nodeId);
    std::vector<SkillNode> getNodesByArchetype(const std::string& archetype) const;
    std::vector<SkillNode> getConnectedNodes(const std::string& nodeId) const;
    
    // Player build management
    void createPlayerBuild(int playerId, const std::string& archetype);
    void updatePlayerBuild(int playerId, const PlayerBuild& build);
    void deletePlayerBuild(int playerId);
    PlayerBuild* getPlayerBuild(int playerId);
    
    // Build progression
    void unlockSkillNode(int playerId, const std::string& nodeId);
    void lockSkillNode(int playerId, const std::string& nodeId);
    bool canUnlockNode(int playerId, const std::string& nodeId) const;
    void addExperience(int playerId, int exp);
    void levelUpPlayer(int playerId);
    
    // Item management
    void addItemToDatabase(const Item& item);
    void removeItemFromDatabase(const std::string& itemId);
    Item* getItemFromDatabase(const std::string& itemId);
    Item generateRandomItem(int itemLevel, const std::string& baseType = "");
    std::vector<Item> getItemsByRarity(int rarity) const;
    std::vector<Item> getItemsByBaseType(const std::string& baseType) const;
    
    // Currency management
    void addCurrency(const CraftingCurrency& currency);
    void removeCurrency(const std::string& currencyName);
    CraftingCurrency* getCurrency(const std::string& currencyName);
    void useCurrency(int playerId, const std::string& currencyName, const std::string& itemId);
    
    // Zone management
    void addZone(const MapZone& zone);
    void removeZone(int index);
    MapZone* getZone(int index);
    void updateZones(float deltaTime);
    std::vector<MapZone> getZonesByBiome(const std::string& biome) const;
    
    // Ore management
    void addOreNode(const std::string& zoneName, const OreNode& node);
    void removeOreNode(const std::string& zoneName, const glm::vec3& position);
    OreNode* getOreNode(const std::string& zoneName, const glm::vec3& position);
    int mineOre(int playerId, const std::string& zoneName, const glm::vec3& position, int amount);
    
    // Loot management
    void addLootChest(const std::string& zoneName, const LootChest& chest);
    void removeLootChest(const std::string& zoneName, const glm::vec3& position);
    LootChest* getLootChest(const std::string& zoneName, const glm::vec3& position);
    std::vector<Item> lootChest(int playerId, const std::string& zoneName, const glm::vec3& position);
    
    // System management
    void startSystem();
    void stopSystem();
    void updateSystem(float deltaTime);
    void resetSystem();
    
    // Callbacks
    std::function<void(int, const std::string&)> onPlayerLevelUp;
    std::function<void(int, const std::string&)> onSkillNodeUnlocked;
    std::function<void(int, const Item&)> onItemFound;
    std::function<void(int, const std::string&)> onLeagueJoined;
    std::function<void(int, const std::string&)> onLeagueLeft;
    std::function<void(const std::string&)> onLeagueStarted;
    std::function<void(const std::string&)> onLeagueEnded;
    
    // Getters
    const std::map<std::string, League>& getLeagues() const { return leagues; }
    const std::map<std::string, SkillNode>& getSkillTree() const { return skillTree; }
    const std::map<int, PlayerBuild>& getPlayerBuilds() const { return playerBuilds; }
    const std::map<std::string, Item>& getItemDatabase() const { return itemDatabase; }
    const std::map<std::string, CraftingCurrency>& getCurrencyDatabase() const { return currencyDatabase; }
    const std::vector<MapZone>& getCurrentZones() const { return currentZones; }
    
    // Statistics
    struct PoEStats {
        int totalLeagues = 0;
        int activeLeagues = 0;
        int totalPlayers = 0;
        int totalSkillNodes = 0;
        int totalItems = 0;
        int totalCurrency = 0;
        float averagePlayerLevel = 0.0f;
        int totalPlayTime = 0;
    };
    PoEStats getStats() const;
    
    // Utility functions
    bool isSystemActive() const { return systemActive; }
    std::string getSystemStatus() const;
    void exportSystemData(const std::string& filename) const;
    void importSystemData(const std::string& filename);
};

// Ore table system for procedural generation
class OreTable {
private:
    std::map<std::string, std::vector<std::string>> biomeOres; // Biome -> Ore types
    std::map<std::string, OreNode> oreTemplates;               // Ore type -> Template
    std::map<std::string, float> oreRarity;                    // Ore type -> Rarity (0-1)
    
public:
    OreTable();
    
    // Ore management
    void addOreType(const std::string& oreType, const OreNode& template);
    void removeOreType(const std::string& oreType);
    void addOreToBiome(const std::string& biome, const std::string& oreType);
    void removeOreFromBiome(const std::string& biome, const std::string& oreType);
    
    // Generation
    std::vector<OreNode> generateOreForBiome(const std::string& biome, int count) const;
    OreNode generateRandomOre(const std::string& biome) const;
    std::vector<std::string> getOresForBiome(const std::string& biome) const;
    
    // Queries
    OreNode* getOreTemplate(const std::string& oreType);
    float getOreRarity(const std::string& oreType) const;
    void setOreRarity(const std::string& oreType, float rarity);
    
    // Getters
    const std::map<std::string, std::vector<std::string>>& getBiomeOres() const { return biomeOres; }
    const std::map<std::string, OreNode>& getOreTemplates() const { return oreTemplates; }
    const std::map<std::string, float>& getOreRarity() const { return oreRarity; }
};

// Utility functions
namespace PoEUtils {
    std::string getRarityName(int rarity);
    glm::vec3 getRarityColor(int rarity);
    std::string getBiomeName(const std::string& biome);
    std::string getArchetypeName(const std::string& archetype);
    float calculateItemValue(const Item& item);
    bool canUseItem(const Item& item, int playerLevel);
    std::string generateItemName(const Item& item);
    std::string generateFlavorText(const Item& item);
}