#pragma once
#include "TimeTurner.h"
#include "MedievalFantasy.h"
#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <functional>

// Universal Battle Royale System
// Adapts to any era/hazard combination for 200-player matches

// Drop mechanics for different eras
enum class DropMethod {
    PARACHUTE,      // Modern era - classic parachute drop
    COMET,          // Hell/Diablo era - fiery comet pods
    CARAVAN,        // Frontier era - covered wagon caravans
    VAULT_DOOR,     // Fallout era - vault doors opening
    SIMULATION,     // Westworld era - simulation pods
    TELEPORT,       // Medieval era - magical teleportation
    POD_CRASH       // Wasteland era - crashed escape pods
};

// Zone types for different hazards
enum class ZoneType {
    CIRCLE_SHRINK,      // Classic BR circle
    LAVA_FLOOD,         // Hell maps - rising lava
    RADIATION_STORM,    // Fallout maps - radiation zones
    GAS_ATTACK,         // Modern maps - gas attacks
    BLIZZARD,           // Frontier maps - snow storms
    CORRUPTION_SPREAD,  // Diablo maps - demonic corruption
    SIMULATION_GLITCH   // Westworld maps - reality glitches
};

// Hazard events that can occur during BR
struct HazardEvent {
    std::string type;           // "cometImpact", "lavaFlood", "radiationBurst"
    glm::vec3 position;
    float radius;
    float damage;
    float duration;
    bool isActive = false;
    std::string description;
    
    void updateEvent(float deltaTime);
    bool isPlayerInRange(const glm::vec3& playerPos) const;
    void triggerEvent();
};

// Loot system that adapts to era
struct EraLoot {
    std::string name;
    std::string era;            // Which era this loot belongs to
    int rarity;                 // 0-5 rarity system
    std::vector<std::string> effects;
    int value;                  // Currency value
    std::string description;
    
    std::string getEraAppropriateName() const;
    glm::vec3 getRarityColor() const;
};

// Zone manager for different zone types
class ZoneManager {
private:
    ZoneType currentZoneType;
    glm::vec3 center;
    float currentRadius;
    float targetRadius;
    float shrinkRate;
    float damagePerSecond;
    bool isActive;
    float timeRemaining;
    
    // Zone-specific properties
    std::string zoneName;
    std::string zoneDescription;
    glm::vec3 zoneColor;        // Visual color for zone
    
public:
    ZoneManager();
    
    // Zone management
    void initializeZone(ZoneType type, const glm::vec3& center, float initialRadius);
    void updateZone(float deltaTime);
    void shrinkZone(float newRadius);
    void setZoneDamage(float damage);
    
    // Zone queries
    bool isPlayerInZone(const glm::vec3& playerPos) const;
    float getDistanceToZone(const glm::vec3& playerPos) const;
    float getZoneDamage() const { return damagePerSecond; }
    
    // Zone events
    void triggerZoneEvent(const std::string& eventType);
    void accelerateShrink(float multiplier);
    void pauseShrink();
    void resumeShrink();
    
    // Getters
    ZoneType getZoneType() const { return currentZoneType; }
    glm::vec3 getCenter() const { return center; }
    float getCurrentRadius() const { return currentRadius; }
    float getTimeRemaining() const { return timeRemaining; }
    std::string getZoneName() const { return zoneName; }
    std::string getZoneDescription() const { return zoneDescription; }
    glm::vec3 getZoneColor() const { return zoneColor; }
};

// Hazard manager for era-appropriate hazards
class HazardManager {
private:
    std::vector<HazardEvent> activeHazards;
    WorldSettings worldSettings;
    float hazardIntensity;
    float timeSinceLastHazard;
    float hazardInterval;
    
    // Era-specific hazard generators
    void generateModernHazards();
    void generateWastelandHazards();
    void generateFrontierHazards();
    void generateMedievalHazards();
    void generateHellHazards();
    void generateWestworldHazards();
    
public:
    HazardManager();
    
    // Hazard management
    void initializeHazards(const WorldSettings& settings);
    void updateHazards(float deltaTime);
    void spawnHazard(const std::string& type, const glm::vec3& position);
    void clearHazards();
    
    // Hazard queries
    bool isPlayerInHazard(const glm::vec3& playerPos) const;
    float getHazardDamage(const glm::vec3& playerPos) const;
    std::vector<HazardEvent> getHazardsNearPlayer(const glm::vec3& playerPos, float radius) const;
    
    // Hazard intensity
    void setHazardIntensity(float intensity);
    void triggerHazardWave();
    void triggerBossHazard();
    
    // Getters
    const std::vector<HazardEvent>& getActiveHazards() const { return activeHazards; }
    float getHazardIntensity() const { return hazardIntensity; }
};

// Loot manager for era-appropriate loot
class LootManager {
private:
    std::map<std::string, std::vector<EraLoot>> lootByEra;
    std::vector<EraLoot> currentLoot;
    WorldSettings worldSettings;
    
    // Era-specific loot generation
    void generateModernLoot();
    void generateWastelandLoot();
    void generateFrontierLoot();
    void generateMedievalLoot();
    void generateHellLoot();
    void generateWestworldLoot();
    
public:
    LootManager();
    
    // Loot management
    void initializeLoot(const WorldSettings& settings);
    void spawnLoot(const glm::vec3& position, int rarity = 0);
    void removeLoot(const glm::vec3& position);
    EraLoot generateRandomLoot(int rarity = 0);
    
    // Loot queries
    std::vector<EraLoot> getLootNearPlayer(const glm::vec3& playerPos, float radius) const;
    EraLoot getLootAtPosition(const glm::vec3& position) const;
    bool isLootAtPosition(const glm::vec3& position) const;
    
    // Loot effects
    void applyLootEffect(int playerId, const EraLoot& loot);
    void removeLootEffect(int playerId, const EraLoot& loot);
    
    // Getters
    const std::vector<EraLoot>& getCurrentLoot() const { return currentLoot; }
    std::vector<EraLoot> getLootByEra(const std::string& era) const;
};

// Universal Battle Royale game mode
class UniversalBattleRoyale {
private:
    WorldSettings worldSettings;
    ZoneManager zoneManager;
    HazardManager hazardManager;
    LootManager lootManager;
    
    // BR state
    int playerCount;
    int maxPlayers;
    bool gameActive;
    float gameTime;
    float gameDuration;
    
    // Drop mechanics
    DropMethod dropMethod;
    std::vector<glm::vec3> dropPoints;
    std::vector<int> playerDropOrder;
    
    // Win conditions
    enum class WinCondition {
        LAST_PLAYER_STANDING,
        SURVIVAL_TIMER,
        BOSS_DEFEAT,
        EXTRACTION
    };
    WinCondition winCondition;
    
    // Event callbacks
    std::function<void(int, const std::string&)> onPlayerDeath;
    std::function<void(int, const std::string&)> onPlayerWin;
    std::function<void(const std::string&)> onGameEvent;
    
public:
    UniversalBattleRoyale();
    
    // Game management
    void initializeGame(const WorldSettings& settings, int maxPlayers = 200);
    void startGame();
    void updateGame(float deltaTime);
    void endGame();
    
    // Player management
    void spawnPlayer(int playerId, const glm::vec3& position);
    void removePlayer(int playerId);
    void updatePlayerPosition(int playerId, const glm::vec3& position);
    
    // Drop mechanics
    void setDropMethod(DropMethod method);
    void generateDropPoints();
    void assignDropPoints();
    glm::vec3 getPlayerDropPoint(int playerId) const;
    
    // Zone management
    void updateZone(float deltaTime);
    void triggerZoneShrink();
    void setZoneDamage(float damage);
    
    // Hazard management
    void updateHazards(float deltaTime);
    void spawnRandomHazard();
    void triggerBossHazard();
    
    // Loot management
    void updateLoot(float deltaTime);
    void spawnLootAtPosition(const glm::vec3& position);
    void removeLootAtPosition(const glm::vec3& position);
    
    // Win condition management
    void checkWinCondition();
    void setWinCondition(WinCondition condition);
    bool isGameOver() const;
    
    // Event handling
    void handlePlayerDeath(int playerId, const std::string& cause);
    void handlePlayerWin(int playerId, const std::string& reason);
    void handleGameEvent(const std::string& eventType);
    
    // Callback setters
    void setOnPlayerDeath(std::function<void(int, const std::string&)> callback);
    void setOnPlayerWin(std::function<void(int, const std::string&)> callback);
    void setOnGameEvent(std::function<void(const std::string&)> callback);
    
    // Getters
    bool isGameActive() const { return gameActive; }
    int getPlayerCount() const { return playerCount; }
    int getMaxPlayers() const { return maxPlayers; }
    float getGameTime() const { return gameTime; }
    float getGameDuration() const { return gameDuration; }
    WorldSettings getWorldSettings() const { return worldSettings; }
    
    // Zone getters
    const ZoneManager& getZoneManager() const { return zoneManager; }
    const HazardManager& getHazardManager() const { return hazardManager; }
    const LootManager& getLootManager() const { return lootManager; }
};

// BR event system
struct BREvent {
    std::string type;
    int playerId;
    glm::vec3 position;
    std::string description;
    float timestamp;
    
    BREvent(const std::string& eventType, int player, const glm::vec3& pos, const std::string& desc);
};

// BR statistics tracking
struct BRStats {
    int kills;
    int deaths;
    int wins;
    int gamesPlayed;
    float averageSurvivalTime;
    std::string favoriteEra;
    std::string favoriteHazard;
    
    void updateStats(const std::string& eventType, float survivalTime);
    void resetStats();
};

// BR lobby system
class BRLobby {
private:
    std::vector<int> queuedPlayers;
    WorldSettings selectedSettings;
    int maxPlayers;
    bool isPrivate;
    std::string lobbyName;
    
public:
    BRLobby();
    
    // Lobby management
    void createLobby(const std::string& name, const WorldSettings& settings, int maxPlayers = 200);
    void joinLobby(int playerId);
    void leaveLobby(int playerId);
    void startGame();
    
    // Settings management
    void setWorldSettings(const WorldSettings& settings);
    void setMaxPlayers(int maxPlayers);
    void setPrivate(bool isPrivate);
    
    // Getters
    const std::vector<int>& getQueuedPlayers() const { return queuedPlayers; }
    WorldSettings getWorldSettings() const { return selectedSettings; }
    int getMaxPlayers() const { return maxPlayers; }
    bool getIsPrivate() const { return isPrivate; }
    std::string getLobbyName() const { return lobbyName; }
};