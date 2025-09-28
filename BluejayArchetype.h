#pragma once
#include "TimeTurner.h"
#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <functional>

// Player archetype system
enum class ArchetypeType {
    BLUEJAY,        // Scout: fast, perceptive, territorial
    BEAR,           // Tank: slow, strong, defensive
    WOLF,           // Hunter: balanced, pack tactics
    EAGLE,          // Sniper: long-range, high ground
    FOX,            // Stealth: sneaky, cunning, traps
    RAVEN,          // Mystic: magic, corruption resistance
    LION,           // Leader: team buffs, charisma
    SHARK,          // Predator: water combat, bloodlust
    COYOTE,         // Survivor: resourceful, adaptable
    HAWK            // Striker: fast attack, precision
};

// Base archetype structure
struct Archetype {
    std::string name;
    ArchetypeType type;
    float speed = 1.0f;           // Movement speed multiplier
    float health = 100.0f;        // Base health
    float vision = 1.0f;          // Vision range multiplier
    float stealth = 0.0f;          // Stealth bonus
    float damage = 1.0f;          // Damage multiplier
    float defense = 1.0f;         // Defense multiplier
    std::vector<std::string> abilities;
    std::string description;
    std::string culturalFlavor;   // American cultural reference
    
    // Era-specific bonuses
    std::map<Era, float> eraBonuses;
    std::map<Hazard, float> hazardResistances;
    
    void applyEraBonus(Era era, float bonus);
    void applyHazardResistance(Hazard hazard, float resistance);
    float getEraBonus(Era era) const;
    float getHazardResistance(Hazard hazard) const;
};

// Bluejay archetype - the scout class
class BluejayArchetype {
private:
    Archetype bluejayData;
    float echoPingCooldown = 0.0f;
    float echoPingRadius = 100.0f;
    float echoPingDuration = 5.0f;
    bool echoPingActive = false;
    float echoPingTimeLeft = 0.0f;
    
    // Bluejay-specific abilities
    struct BluejayAbilities {
        bool wallRun = true;
        bool rooftopPerch = true;
        bool earlyWarning = true;
        bool lootDetection = true;
        bool hazardSpotting = true;
        bool echoPing = true;
    };
    BluejayAbilities abilities;
    
public:
    BluejayArchetype();
    
    // Core abilities
    void updateArchetype(float deltaTime);
    void activateEchoPing();
    void deactivateEchoPing();
    bool canEchoPing() const;
    float getEchoPingCooldown() const { return echoPingCooldown; }
    float getEchoPingRadius() const { return echoPingRadius; }
    bool isEchoPingActive() const { return echoPingActive; }
    float getEchoPingTimeLeft() const { return echoPingTimeLeft; }
    
    // Movement abilities
    bool canWallRun() const { return abilities.wallRun; }
    bool canRooftopPerch() const { return abilities.rooftopPerch; }
    glm::vec3 getWallRunDirection(const glm::vec3& position, const glm::vec3& direction) const;
    glm::vec3 getRooftopPerchPosition(const glm::vec3& position) const;
    
    // Detection abilities
    bool canDetectLoot() const { return abilities.lootDetection; }
    bool canSpotHazards() const { return abilities.hazardSpotting; }
    bool canEarlyWarning() const { return abilities.earlyWarning; }
    std::vector<glm::vec3> detectLoot(const glm::vec3& position, float radius) const;
    std::vector<glm::vec3> spotHazards(const glm::vec3& position, float radius) const;
    std::vector<glm::vec3> getEarlyWarning(const glm::vec3& position, float radius) const;
    
    // Era-specific adaptations
    void adaptToEra(Era era);
    void adaptToHazard(Hazard hazard);
    void adaptToWorldSettings(const WorldSettings& settings);
    
    // Getters
    const Archetype& getArchetypeData() const { return bluejayData; }
    const BluejayAbilities& getAbilities() const { return abilities; }
    
    // Callbacks
    std::function<void(const glm::vec3&, float)> onEchoPing;
    std::function<void(const std::vector<glm::vec3>&)> onLootDetected;
    std::function<void(const std::vector<glm::vec3>&)> onHazardsSpotted;
    std::function<void(const std::vector<glm::vec3>&)> onEarlyWarning;
};

// Archetype manager for all player classes
class ArchetypeManager {
private:
    std::map<ArchetypeType, Archetype> archetypes;
    std::map<int, ArchetypeType> playerArchetypes; // Player ID -> Archetype
    std::map<int, BluejayArchetype> bluejayPlayers; // Player ID -> Bluejay instance
    
public:
    ArchetypeManager();
    
    // Archetype management
    void registerArchetype(const Archetype& archetype);
    void setPlayerArchetype(int playerId, ArchetypeType type);
    ArchetypeType getPlayerArchetype(int playerId) const;
    const Archetype& getArchetype(ArchetypeType type) const;
    
    // Bluejay-specific management
    void createBluejayPlayer(int playerId);
    void removeBluejayPlayer(int playerId);
    BluejayArchetype* getBluejayPlayer(int playerId);
    
    // Update all archetypes
    void updateArchetypes(float deltaTime);
    
    // Era/hazard adaptations
    void adaptAllToEra(Era era);
    void adaptAllToHazard(Hazard hazard);
    void adaptAllToWorldSettings(const WorldSettings& settings);
    
    // Getters
    const std::map<ArchetypeType, Archetype>& getAllArchetypes() const { return archetypes; }
    const std::map<int, ArchetypeType>& getPlayerArchetypes() const { return playerArchetypes; }
    
    // Statistics
    int getArchetypeCount(ArchetypeType type) const;
    std::map<ArchetypeType, int> getArchetypeDistribution() const;
};

// Echo ping system for Bluejay
struct EchoPing {
    glm::vec3 position;
    float radius;
    float duration;
    float timeLeft;
    int playerId;
    bool isActive;
    std::vector<int> revealedPlayers;
    
    void updatePing(float deltaTime);
    bool isExpired() const;
    void addRevealedPlayer(int playerId);
    bool hasRevealedPlayer(int playerId) const;
};

// Echo ping manager
class EchoPingManager {
private:
    std::vector<EchoPing> activePings;
    float globalCooldown = 0.0f;
    float globalCooldownDuration = 30.0f; // 30 second global cooldown
    
public:
    EchoPingManager();
    
    // Ping management
    void createEchoPing(int playerId, const glm::vec3& position, float radius, float duration);
    void updatePings(float deltaTime);
    void removeExpiredPings();
    
    // Ping queries
    std::vector<EchoPing> getPingsNearPosition(const glm::vec3& position, float radius) const;
    std::vector<int> getRevealedPlayers(const glm::vec3& position, float radius) const;
    bool isPlayerRevealed(int playerId, const glm::vec3& position, float radius) const;
    
    // Cooldown management
    bool canCreatePing() const;
    float getGlobalCooldown() const { return globalCooldown; }
    void setGlobalCooldown(float duration) { globalCooldownDuration = duration; }
    
    // Getters
    const std::vector<EchoPing>& getActivePings() const { return activePings; }
    int getActivePingCount() const { return activePings.size(); }
    
    // Callbacks
    std::function<void(const EchoPing&)> onPingCreated;
    std::function<void(const EchoPing&)> onPingExpired;
    std::function<void(int, const std::vector<int>&)> onPlayersRevealed;
};

// Bluejay visual/audio effects
class BluejayEffects {
public:
    // Visual effects
    static void renderEchoPing(const glm::vec3& position, float radius, float intensity);
    static void renderWallRun(const glm::vec3& start, const glm::vec3& end);
    static void renderRooftopPerch(const glm::vec3& position);
    static void renderLootDetection(const std::vector<glm::vec3>& lootPositions);
    static void renderHazardSpotting(const std::vector<glm::vec3>& hazardPositions);
    static void renderEarlyWarning(const std::vector<glm::vec3>& warningPositions);
    
    // Audio effects
    static void playEchoPingSound(const glm::vec3& position);
    static void playWallRunSound(const glm::vec3& position);
    static void playRooftopPerchSound(const glm::vec3& position);
    static void playLootDetectionSound(const glm::vec3& position);
    static void playHazardSpottingSound(const glm::vec3& position);
    static void playEarlyWarningSound(const glm::vec3& position);
    
    // Particle effects
    static void createEchoPingParticles(const glm::vec3& position, float radius);
    static void createWallRunParticles(const glm::vec3& position);
    static void createRooftopPerchParticles(const glm::vec3& position);
    static void createLootDetectionParticles(const glm::vec3& position);
    static void createHazardSpottingParticles(const glm::vec3& position);
    static void createEarlyWarningParticles(const glm::vec3& position);
};

// Bluejay integration with existing systems
class BluejayIntegration {
private:
    ArchetypeManager* archetypeManager;
    EchoPingManager* echoPingManager;
    TimeTurner* timeTurner;
    
public:
    BluejayIntegration(ArchetypeManager* archetypeMgr, EchoPingManager* echoMgr, TimeTurner* turner);
    
    // Integration with TimeTurner
    void onEraChanged(Era newEra);
    void onHazardChanged(Hazard newHazard);
    void onWorldSettingsChanged(const WorldSettings& settings);
    
    // Integration with game modes
    void onPlayerSpawn(int playerId, const glm::vec3& position);
    void onPlayerDeath(int playerId, const std::string& cause);
    void onPlayerKill(int killerId, int victimId);
    void onGameEvent(const std::string& eventType, const std::string& data);
    
    // Bluejay-specific events
    void onEchoPingUsed(int playerId, const glm::vec3& position);
    void onLootDetected(int playerId, const std::vector<glm::vec3>& lootPositions);
    void onHazardsSpotted(int playerId, const std::vector<glm::vec3>& hazardPositions);
    void onEarlyWarning(int playerId, const std::vector<glm::vec3>& warningPositions);
    
    // Callback setters
    void setOnEchoPingUsed(std::function<void(int, const glm::vec3&)> callback);
    void setOnLootDetected(std::function<void(int, const std::vector<glm::vec3>&)> callback);
    void setOnHazardsSpotted(std::function<void(int, const std::vector<glm::vec3>&)> callback);
    void setOnEarlyWarning(std::function<void(int, const std::vector<glm::vec3>&)> callback);
};