#pragma once
#include "TimeTurner.h"
#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <functional>
#include <random>

// Core survival statistics
struct SurvivalStats {
    float health = 100.0f;
    float hunger = 100.0f;
    float thirst = 100.0f;
    float stamina = 100.0f;
    bool bleeding = false;
    bool infected = false;
    float temperature = 36.5f;  // Body temperature
    float radiation = 0.0f;    // Radiation exposure
    float corruption = 0.0f;    // Demonic corruption (for medieval/hell modes)
    
    // Drain rates (per second)
    float hungerDrainRate = 0.1f;
    float thirstDrainRate = 0.15f;
    float staminaDrainRate = 2.0f;
    float staminaRegenRate = 1.0f;
    float bleedingDrainRate = 5.0f;
    
    void updateStats(float deltaTime);
    void applyDamage(float damage);
    void heal(float amount);
    void addHunger(float amount);
    void addThirst(float amount);
    void addStamina(float amount);
    void startBleeding();
    void stopBleeding();
    void addInfection();
    void cureInfection();
    void addRadiation(float amount);
    void addCorruption(float amount);
    
    bool isAlive() const { return health > 0.0f; }
    bool isStarving() const { return hunger < 20.0f; }
    bool isDehydrated() const { return thirst < 20.0f; }
    bool isExhausted() const { return stamina < 10.0f; }
    bool isCritical() const { return health < 25.0f; }
};

// Item system for inventory
struct Item {
    std::string id;           // "ak47", "meat", "bandage", "scrap"
    std::string name;
    int quantity = 1;
    float weight = 1.0f;
    int value = 0;           // Currency value
    std::string category;     // "weapon", "food", "medical", "material"
    std::vector<std::string> effects; // "heal", "hunger", "thirst", "stamina"
    bool stackable = true;
    bool consumable = false;
    float durability = 100.0f;
    
    Item(const std::string& itemId, const std::string& itemName, float itemWeight = 1.0f);
    void useItem();
    bool canStackWith(const Item& other) const;
    void combineWith(Item& other);
    std::string getDescription() const;
};

// Inventory system
struct Inventory {
    std::vector<Item> items;
    float capacity = 20.0f;
    float currentWeight = 0.0f;
    int maxSlots = 50;
    
    bool addItem(const Item& item);
    bool removeItem(const std::string& itemId, int quantity = 1);
    bool hasItem(const std::string& itemId, int quantity = 1) const;
    Item* findItem(const std::string& itemId);
    void useItem(const std::string& itemId);
    void dropItem(const std::string& itemId, int quantity = 1);
    void sortInventory();
    void clearInventory();
    
    float getWeightPercentage() const;
    bool isFull() const;
    bool canFit(const Item& item) const;
};

// Base building system
struct BuildPiece {
    glm::vec3 position;
    glm::vec3 size;
    int ownerId;
    std::string type;         // "wall", "door", "floor", "roof", "fence"
    std::string material;     // "wood", "metal", "stone", "concrete"
    float health = 100.0f;
    float maxHealth = 100.0f;
    bool isDestroyed = false;
    float buildTime = 0.0f;
    float maxBuildTime = 5.0f;
    
    void updateBuild(float deltaTime);
    void damage(float amount);
    void repair(float amount);
    bool isBuilt() const;
    bool canBeDestroyed() const;
};

// Resource nodes for gathering
struct ResourceNode {
    glm::vec3 position;
    std::string type;         // "wood", "metal", "stone", "food", "water"
    int quantity = 100;
    int maxQuantity = 100;
    float respawnTime = 300.0f; // 5 minutes
    float currentRespawnTime = 0.0f;
    bool isDepleted = false;
    std::string description;
    
    void updateNode(float deltaTime);
    int gatherResource(int amount);
    void respawn();
    bool canGather() const;
};

// Economy and currency system
struct Economy {
    std::map<int, int> playerCurrency; // Player ID -> Currency amount
    std::map<std::string, int> itemPrices; // Item ID -> Price
    std::map<std::string, int> itemDemand; // Item ID -> Demand level
    
    void addCurrency(int playerId, int amount);
    bool spendCurrency(int playerId, int amount);
    int getCurrency(int playerId) const;
    void setItemPrice(const std::string& itemId, int price);
    int getItemPrice(const std::string& itemId) const;
    void updateEconomy(float deltaTime);
};

// Gambling system
struct GambleResult {
    int playerId;
    std::string gameType;    // "slots", "roulette", "blackjack", "dice"
    int stake;
    int payout;
    bool won;
    std::string description;
    
    GambleResult(int player, const std::string& game, int bet, int result, bool victory);
};

// Gambling games
class GamblingSystem {
private:
    std::mt19937 rng;
    std::uniform_real_distribution<float> randomFloat;
    
public:
    GamblingSystem();
    
    // Slot machine
    GambleResult playSlots(int playerId, int stake);
    
    // Roulette
    GambleResult playRoulette(int playerId, int stake, const std::string& bet);
    
    // Blackjack
    GambleResult playBlackjack(int playerId, int stake);
    
    // Dice
    GambleResult playDice(int playerId, int stake, int target);
    
    // Card games
    GambleResult playPoker(int playerId, int stake);
    
    // Random events
    GambleResult playRandomEvent(int playerId, int stake);
    
    // Utility
    bool canAfford(int playerId, int stake, const Economy& economy) const;
    void processGambleResult(const GambleResult& result, Economy& economy);
};

// Survival Mode Implementation
class SurvivalMode {
private:
    // Game state
    std::map<int, SurvivalStats> playerStats;
    std::map<int, Inventory> playerInventories;
    std::vector<BuildPiece> buildPieces;
    std::vector<ResourceNode> resourceNodes;
    Economy economy;
    GamblingSystem gambling;
    
    // World settings
    WorldSettings worldSettings;
    bool gameActive = false;
    float gameTime = 0.0f;
    float dayNightCycle = 0.0f;
    
    // Survival settings
    struct SurvivalConfig {
        bool enableHunger = true;
        bool enableThirst = true;
        bool enableBleeding = true;
        bool enableInfection = true;
        bool enableRadiation = false;
        bool enableCorruption = false;
        bool enableTemperature = false;
        bool enableBaseBuilding = true;
        bool enableResourceGathering = true;
        bool enableEconomy = true;
        bool enableGambling = true;
        bool enablePvP = true;
        float dayLength = 1200.0f; // 20 minutes
        float nightLength = 600.0f; // 10 minutes
    };
    SurvivalConfig config;
    
    // Events
    std::vector<std::string> activeEvents;
    float eventTimer = 0.0f;
    float eventInterval = 300.0f; // 5 minutes
    
    // Callbacks
    std::function<void(int, const std::string&)> onPlayerDeath;
    std::function<void(int, const std::string&)> onPlayerRevive;
    std::function<void(int, const GambleResult&)> onGambleResult;
    std::function<void(int, const std::string&)> onItemUse;
    std::function<void(int, const std::string&)> onBuildComplete;
    
public:
    SurvivalMode();
    ~SurvivalMode();
    
    // Game management
    void initializeGame(const WorldSettings& settings);
    void startGame();
    void updateGame(float deltaTime);
    void endGame();
    void resetGame();
    
    // Player management
    void addPlayer(int playerId);
    void removePlayer(int playerId);
    void updatePlayerPosition(int playerId, const glm::vec3& position);
    void handlePlayerDeath(int playerId, const std::string& cause);
    void revivePlayer(int playerId);
    
    // Survival stats
    void updatePlayerStats(int playerId, float deltaTime);
    void applyDamage(int playerId, float damage);
    void healPlayer(int playerId, float amount);
    void addHunger(int playerId, float amount);
    void addThirst(int playerId, float amount);
    void addStamina(int playerId, float amount);
    
    // Inventory management
    void addItemToPlayer(int playerId, const Item& item);
    void removeItemFromPlayer(int playerId, const std::string& itemId, int quantity = 1);
    void useItem(int playerId, const std::string& itemId);
    void dropItem(int playerId, const std::string& itemId, int quantity = 1);
    
    // Base building
    void placeBuildPiece(int playerId, const glm::vec3& position, const std::string& type);
    void removeBuildPiece(int playerId, const glm::vec3& position);
    void updateBuildPieces(float deltaTime);
    void damageBuildPiece(const glm::vec3& position, float damage);
    
    // Resource gathering
    void addResourceNode(const glm::vec3& position, const std::string& type);
    void updateResourceNodes(float deltaTime);
    int gatherResource(int playerId, const glm::vec3& position, int amount);
    
    // Economy
    void addCurrency(int playerId, int amount);
    bool spendCurrency(int playerId, int amount);
    void setItemPrice(const std::string& itemId, int price);
    int getItemPrice(const std::string& itemId) const;
    
    // Gambling
    GambleResult playSlots(int playerId, int stake);
    GambleResult playRoulette(int playerId, int stake, const std::string& bet);
    GambleResult playBlackjack(int playerId, int stake);
    GambleResult playDice(int playerId, int stake, int target);
    
    // Events
    void updateEvents(float deltaTime);
    void triggerRandomEvent();
    void triggerResourceEvent();
    void triggerWeatherEvent();
    void triggerHazardEvent();
    
    // World events
    void triggerDayNightCycle();
    void triggerWeatherChange();
    void triggerResourceRespawn();
    void triggerHazardZone();
    
    // Callback setters
    void setOnPlayerDeath(std::function<void(int, const std::string&)> callback);
    void setOnPlayerRevive(std::function<void(int, const std::string&)> callback);
    void setOnGambleResult(std::function<void(int, const GambleResult&)> callback);
    void setOnItemUse(std::function<void(int, const std::string&)> callback);
    void setOnBuildComplete(std::function<void(int, const std::string&)> callback);
    
    // Getters
    const SurvivalStats& getPlayerStats(int playerId) const;
    const Inventory& getPlayerInventory(int playerId) const;
    const std::vector<BuildPiece>& getBuildPieces() const { return buildPieces; }
    const std::vector<ResourceNode>& getResourceNodes() const { return resourceNodes; }
    const Economy& getEconomy() const { return economy; }
    
    // Game state
    bool isGameActive() const { return gameActive; }
    float getGameTime() const { return gameTime; }
    float getDayNightCycle() const { return dayNightCycle; }
    bool isDay() const;
    bool isNight() const;
    
    // Configuration
    void setConfig(const SurvivalConfig& config);
    const SurvivalConfig& getConfig() const { return config; }
    
    // Statistics
    struct SurvivalStats {
        int totalPlayers = 0;
        int alivePlayers = 0;
        int totalDeaths = 0;
        int totalBuildPieces = 0;
        int totalResourceNodes = 0;
        int totalGambles = 0;
        int totalCurrency = 0;
    };
    SurvivalStats getGameStats() const;
    
    // Utility functions
    glm::vec3 getRandomSpawnPosition() const;
    bool isPositionSafe(const glm::vec3& position) const;
    bool canPlayerBuild(int playerId, const glm::vec3& position) const;
    bool canPlayerGather(int playerId, const glm::vec3& position) const;
    
    // Item definitions
    void loadItemDefinitions();
    Item createItem(const std::string& itemId) const;
    std::vector<Item> getAvailableItems() const;
    
    // Safe zones
    void addSafeZone(const glm::vec3& position, float radius);
    bool isPositionInSafeZone(const glm::vec3& position) const;
    void disableWeaponsInSafeZone(int playerId);
    void enableWeaponsInSafeZone(int playerId);
};

// Survival Mode Lobby
class SurvivalLobby {
private:
    std::vector<int> queuedPlayers;
    WorldSettings worldSettings;
    SurvivalMode::SurvivalConfig config;
    bool isPrivate = false;
    std::string lobbyName;
    int maxPlayers = 100;
    
public:
    SurvivalLobby();
    
    // Lobby management
    void createLobby(const std::string& name, const WorldSettings& settings, const SurvivalMode::SurvivalConfig& config);
    void joinLobby(int playerId);
    void leaveLobby(int playerId);
    void startGame();
    
    // Configuration
    void setWorldSettings(const WorldSettings& settings);
    void setConfig(const SurvivalMode::SurvivalConfig& config);
    void setPrivate(bool isPrivate);
    void setMaxPlayers(int maxPlayers);
    
    // Getters
    const std::vector<int>& getQueuedPlayers() const { return queuedPlayers; }
    const WorldSettings& getWorldSettings() const { return worldSettings; }
    const SurvivalMode::SurvivalConfig& getConfig() const { return config; }
    bool getIsPrivate() const { return isPrivate; }
    std::string getLobbyName() const { return lobbyName; }
    int getMaxPlayers() const { return maxPlayers; }
    int getPlayerCount() const { return queuedPlayers.size(); }
};