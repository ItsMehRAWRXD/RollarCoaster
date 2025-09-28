#pragma once
#include "TimeTurner.h"
#include "UniversalBattleRoyale.h"
#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <functional>
#include <random>

// Hunger Games Configuration
struct HGConfig {
    int playerCount = 200;
    bool duos = false;
    float scatterRadius = 1800.0f;   // meters
    float gracePeriod = 60.0f;       // no PvP grace period
    bool sponsorsEnabled = true;
    float matchDuration = 1800.0f;   // 30 minutes max
    int cornucopiaLootCount = 80;
    float cornucopiaRadius = 25.0f;
    bool enableArenaEvents = true;
    bool enableMazeShift = true;
    float eventInterval = 30.0f;     // seconds between events
};

// Tribute (player) in the Hunger Games
struct Tribute {
    int id;
    glm::vec3 position;
    bool alive = true;
    int district = -1;  // 0-11 districts
    int kills = 0;
    int sponsorCredits = 0;
    float survivalTime = 0.0f;
    std::string name;
    std::vector<std::string> inventory;
    bool inGracePeriod = true;
    
    // Stats for sponsors
    int stylePoints = 0;
    int multikills = 0;
    int audienceVotes = 0;
    
    void updateSurvivalTime(float deltaTime);
    void addKill();
    void addStylePoints(int points);
    void addSponsorCredits(int credits);
    int getTotalSponsorCredits() const;
};

// Sponsor gift system
struct SponsorGift {
    glm::vec3 dropPosition;
    std::string type;        // "medkit", "food", "knife", "random"
    int senderId;            // sponsor/spectator id
    int targetTributeId;     // intended recipient
    float dropTime;
    bool isDelivered = false;
    std::string description;
    int value;               // sponsor credit cost
    
    void updateGift(float deltaTime);
    bool isExpired() const;
};

// Arena events that create chaos
struct ArenaEvent {
    std::string type;        // "fireWall", "beastWave", "mazeShift", "acidRain"
    glm::vec3 center;
    float radius;
    float duration;
    float startTime;
    bool isActive = false;
    std::string description;
    float intensity = 1.0f;
    
    void updateEvent(float deltaTime);
    bool isExpired() const;
    void triggerEvent();
    void endEvent();
};

// Hunger Games Mode Implementation
class HungerGamesMode {
private:
    HGConfig config;
    WorldSettings worldSettings;
    
    // Game state
    std::vector<Tribute> tributes;
    std::vector<ArenaEvent> liveEvents;
    std::vector<SponsorGift> pendingGifts;
    bool gameActive = false;
    bool gameOver = false;
    float gameTime = 0.0f;
    float graceTimeLeft = 0.0f;
    
    // Systems
    ZoneManager zoneManager;
    HazardManager hazardManager;
    LootManager lootManager;
    
    // Random number generation
    std::mt19937 rng;
    std::uniform_real_distribution<float> randomFloat;
    std::uniform_int_distribution<int> randomInt;
    
    // Event probabilities (weighted by game phase)
    struct EventWeights {
        float fireWall = 0.004f;
        float beastWave = 0.003f;
        float mazeShift = 0.002f;
        float acidRain = 0.001f;
        float cometShower = 0.001f;
    };
    EventWeights eventWeights;
    
    // Callbacks
    std::function<void(int, const std::string&)> onTributeDeath;
    std::function<void(int, const std::string&)> onTributeWin;
    std::function<void(const std::string&)> onGameEvent;
    std::function<void(int, int)> onSponsorGift;
    
public:
    HungerGamesMode(const HGConfig& cfg);
    ~HungerGamesMode();
    
    // Game management
    void initializeGame(const WorldSettings& settings);
    void startGame();
    void updateGame(float deltaTime);
    void endGame();
    void resetGame();
    
    // Tribute management
    void scatterSpawn();
    void spawnTribute(int id, const glm::vec3& position);
    void removeTribute(int id);
    void updateTributePosition(int id, const glm::vec3& position);
    void handleTributeDeath(int id, const std::string& cause);
    void handleTributeKill(int killerId, int victimId);
    
    // Cornucopia system
    void spawnCornucopia();
    void updateCornucopia(float deltaTime);
    bool isCornucopiaActive() const;
    
    // Arena events
    void rollArenaEvents(float deltaTime);
    void triggerFireWall(const glm::vec3& center, float radius);
    void triggerBeastWave(const glm::vec3& center, float radius);
    void triggerMazeShift(const glm::vec3& center, float radius);
    void triggerAcidRain(const glm::vec3& center, float radius);
    void triggerCometShower(const glm::vec3& center, float radius);
    
    // Zone and hazard management
    void updateZoneAndEvents(float deltaTime);
    void applyRingAndEvents(float deltaTime);
    void updateHazards(float deltaTime);
    
    // Sponsor system
    void enableSponsorMode(int tributeId);
    void sendSponsorGift(int sponsorId, int targetTributeId, const std::string& giftType);
    void updateSponsorGifts(float deltaTime);
    void deliverGift(const SponsorGift& gift);
    void addSponsorCredits(int tributeId, int credits);
    
    // Win condition
    void checkWinCondition();
    bool isGameOver() const { return gameOver; }
    int getWinnerId() const;
    std::vector<int> getAliveTributes() const;
    
    // Event handling
    void handleGameEvent(const std::string& eventType, const std::string& data = "");
    void broadcastEvent(const std::string& eventType, const std::string& data = "");
    
    // Callback setters
    void setOnTributeDeath(std::function<void(int, const std::string&)> callback);
    void setOnTributeWin(std::function<void(int, const std::string&)> callback);
    void setOnGameEvent(std::function<void(const std::string&)> callback);
    void setOnSponsorGift(std::function<void(int, int)> callback);
    
    // Getters
    const HGConfig& getConfig() const { return config; }
    const std::vector<Tribute>& getTributes() const { return tributes; }
    const std::vector<ArenaEvent>& getLiveEvents() const { return liveEvents; }
    const std::vector<SponsorGift>& getPendingGifts() const { return pendingGifts; }
    
    // Game state
    bool isGameActive() const { return gameActive; }
    float getGameTime() const { return gameTime; }
    float getGraceTimeLeft() const { return graceTimeLeft; }
    int getAliveCount() const;
    int getTotalKills() const;
    
    // Statistics
    struct GameStats {
        int totalKills;
        int totalDeaths;
        float averageSurvivalTime;
        std::string mostActiveEvent;
        int totalSponsorGifts;
    };
    GameStats getGameStats() const;
    
    // Utility functions
    glm::vec3 getRandomSpawnPosition() const;
    glm::vec3 getCornucopiaPosition() const;
    bool isPositionInGraceZone(const glm::vec3& position) const;
    float getDistanceToZone(const glm::vec3& position) const;
    
    // Event weights (adjustable for different phases)
    void setEventWeights(const EventWeights& weights);
    void adjustEventWeightsForPhase(float gamePhase); // 0.0 = start, 1.0 = end
};

// Hunger Games Lobby System
class HGLobby {
private:
    std::vector<int> queuedPlayers;
    HGConfig lobbyConfig;
    WorldSettings worldSettings;
    bool isPrivate = false;
    std::string lobbyName;
    int maxPlayers = 200;
    
public:
    HGLobby();
    
    // Lobby management
    void createLobby(const std::string& name, const HGConfig& config, const WorldSettings& settings);
    void joinLobby(int playerId);
    void leaveLobby(int playerId);
    void startGame();
    
    // Configuration
    void setConfig(const HGConfig& config);
    void setWorldSettings(const WorldSettings& settings);
    void setPrivate(bool isPrivate);
    void setMaxPlayers(int maxPlayers);
    
    // Getters
    const std::vector<int>& getQueuedPlayers() const { return queuedPlayers; }
    const HGConfig& getConfig() const { return lobbyConfig; }
    const WorldSettings& getWorldSettings() const { return worldSettings; }
    bool getIsPrivate() const { return isPrivate; }
    std::string getLobbyName() const { return lobbyName; }
    int getMaxPlayers() const { return maxPlayers; }
    int getPlayerCount() const { return queuedPlayers.size(); }
};

// Hunger Games Statistics
struct HGStats {
    int gamesPlayed = 0;
    int wins = 0;
    int kills = 0;
    int deaths = 0;
    float totalSurvivalTime = 0.0f;
    int sponsorGiftsSent = 0;
    int sponsorGiftsReceived = 0;
    std::string favoriteDistrict;
    std::string favoriteEvent;
    
    void updateStats(const std::string& eventType, float survivalTime);
    void resetStats();
    float getWinRate() const;
    float getAverageSurvivalTime() const;
    float getKillDeathRatio() const;
};

// Hunger Games Event Types
namespace HGEvents {
    const std::string TRIBUTE_DEATH = "tribute_death";
    const std::string TRIBUTE_WIN = "tribute_win";
    const std::string GAME_START = "game_start";
    const std::string GAME_END = "game_end";
    const std::string ARENA_EVENT = "arena_event";
    const std::string SPONSOR_GIFT = "sponsor_gift";
    const std::string CORNUCOPIA_OPEN = "cornucopia_open";
    const std::string GRACE_PERIOD_END = "grace_period_end";
    const std::string ZONE_SHRINK = "zone_shrink";
    const std::string BEAST_WAVE = "beast_wave";
    const std::string FIRE_WALL = "fire_wall";
    const std::string MAZE_SHIFT = "maze_shift";
    const std::string ACID_RAIN = "acid_rain";
    const std::string COMET_SHOWER = "comet_shower";
}