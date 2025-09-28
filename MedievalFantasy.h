#pragma once
#include "TimeTurner.h"
#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>

// Medieval gear and equipment
struct MedievalGear {
    std::string type;        // "sword", "bow", "shield", "armor"
    std::string name;
    int damage = 0;
    int defense = 0;
    float staminaCost = 1.0f;
    int durability = 100;
    std::string material;    // "iron", "steel", "enchanted", "demonic"
    std::vector<std::string> effects; // "fire_damage", "corruption_resistance"
    
    // Rarity system (Diablo-style)
    int rarity = 0;          // 0=common, 1=uncommon, 2=rare, 3=epic, 4=legendary, 5=mythic
    std::string getRarityName() const;
    glm::vec3 getRarityColor() const;
};

// Corruption system for demonic influence
struct Corruption {
    float value = 0.0f;      // 0-100 corruption level
    bool possessed = false;   // Player is fully possessed
    float resistance = 50.0f;  // Natural resistance to corruption
    std::vector<std::string> activeEffects; // Current corruption effects
    
    // Corruption thresholds
    static constexpr float MILD_THRESHOLD = 25.0f;
    static constexpr float MODERATE_THRESHOLD = 50.0f;
    static constexpr float SEVERE_THRESHOLD = 75.0f;
    static constexpr float POSSESSION_THRESHOLD = 90.0f;
    
    void updateCorruption(float deltaTime, bool inCorruptedZone);
    std::string getCorruptionStatus() const;
    bool isCorrupted() const { return value > MILD_THRESHOLD; }
    bool isPossessed() const { return possessed; }
};

// Medieval factions and guilds
struct MedievalFaction {
    std::string name;
    std::string type;        // "knightly_order", "guild", "noble_house", "cult"
    int influence = 0;       // Political influence
    int territory = 0;       // Controlled territory
    std::vector<int> members; // Player IDs
    glm::vec3 headquarters;
    std::string allegiance;  // "holy", "neutral", "demonic"
    bool isCorrupted = false;
    
    // Faction relationships
    std::map<std::string, int> relationships; // Faction name -> relationship (-100 to 100)
    
    void addMember(int playerId);
    void removeMember(int playerId);
    bool isMember(int playerId) const;
    void updateInfluence(int change);
    void corruptFaction();
};

// Corruption zones (like Lilith's influence)
struct CorruptionZone {
    glm::vec3 position;
    float radius;
    float corruptionLevel;   // 0-100, affects spawn rates and player corruption
    std::string type;        // "lilith_altar", "demonic_portal", "cursed_ruins"
    bool isActive = true;
    float spreadRate = 1.0f; // How fast corruption spreads
    std::vector<int> affectedPlayers; // Players currently in zone
    
    void updateZone(float deltaTime);
    void spreadCorruption();
    bool isPlayerInZone(const glm::vec3& playerPos) const;
    void addPlayer(int playerId);
    void removePlayer(int playerId);
};

// Demonic boss system (Lilith and others)
struct DemonBoss {
    std::string name;
    glm::vec3 position;
    float health;
    float maxHealth;
    bool isActive = false;
    bool isBoss = false;    // True for major bosses like Lilith
    std::string type;        // "lilith", "demon_lord", "corrupted_knight"
    std::vector<std::string> abilities;
    float corruptionAura = 0.0f; // Corruption effect radius
    
    // Boss phases
    int currentPhase = 1;
    int maxPhases = 3;
    std::vector<float> phaseHealthThresholds; // Health % for phase transitions
    
    void updateBoss(float deltaTime);
    void triggerPhaseTransition();
    void spawnMinions();
    void castCorruptionWave();
};

// Loot system with Diablo-style rarity
struct Loot {
    std::string name;
    int rarity;              // 0=common, 1=uncommon, 2=rare, 3=epic, 4=legendary, 5=mythic
    std::vector<std::string> effects;
    std::string itemType;    // "weapon", "armor", "trinket", "consumable"
    int value = 0;           // Gold value
    bool isCursed = false;    // Cursed items have negative effects
    bool isBlessed = false;  // Blessed items have positive effects
    
    std::string getRarityName() const;
    glm::vec3 getRarityColor() const;
    std::string getDescription() const;
};

// Medieval structures and buildings
struct MedievalStructure {
    glm::vec3 position;
    std::string type;        // "castle", "village", "cathedral", "tavern", "guild_hall"
    std::string faction;     // Owning faction
    int level = 1;           // Structure level/upgrade level
    bool isCorrupted = false;
    std::vector<std::string> features; // "walls", "moat", "tower", "chapel"
    
    // Siege mechanics
    int siegeHealth = 100;
    int maxSiegeHealth = 100;
    bool underSiege = false;
    std::string besiegingFaction;
    
    void updateStructure(float deltaTime);
    void startSiege(const std::string& attackingFaction);
    void endSiege();
    void corruptStructure();
};

// Corruption system manager
class CorruptionSystem {
private:
    std::vector<CorruptionZone> corruptionZones;
    std::vector<DemonBoss> activeBosses;
    float globalCorruptionLevel = 0.0f;
    bool lilithActive = false;
    
public:
    CorruptionSystem();
    
    // Zone management
    void addCorruptionZone(const CorruptionZone& zone);
    void removeCorruptionZone(const glm::vec3& position);
    void updateZones(float deltaTime);
    
    // Boss management
    void spawnBoss(const std::string& bossType, const glm::vec3& position);
    void updateBosses(float deltaTime);
    void spawnLilith(const glm::vec3& position);
    
    // Corruption effects
    void applyCorruptionToPlayer(int playerId, float amount);
    void spreadCorruption();
    void updateGlobalCorruption(float deltaTime);
    
    // Getters
    const std::vector<CorruptionZone>& getCorruptionZones() const { return corruptionZones; }
    const std::vector<DemonBoss>& getActiveBosses() const { return activeBosses; }
    float getGlobalCorruptionLevel() const { return globalCorruptionLevel; }
    bool isLilithActive() const { return lilithActive; }
    
    // Events
    void triggerLilithEvent();
    void triggerCorruptionWave();
    void triggerDemonInvasion();
};

// Medieval combat system
class MedievalCombat {
private:
    std::vector<MedievalGear> availableGear;
    std::map<std::string, std::vector<MedievalGear>> gearByType;
    
public:
    MedievalCombat();
    
    // Gear management
    void addGear(const MedievalGear& gear);
    MedievalGear generateRandomGear(int rarity = 0);
    MedievalGear generateLootDrop(const DemonBoss& boss);
    
    // Combat calculations
    int calculateDamage(const MedievalGear& weapon, const MedievalGear& armor);
    float calculateStaminaCost(const MedievalGear& weapon);
    bool canUseGear(const MedievalGear& gear, float playerStamina);
    
    // Special abilities
    void triggerCorruptionAttack(const glm::vec3& position, float radius);
    void triggerHolySmite(const glm::vec3& position, float radius);
    void triggerDemonicSummon(const glm::vec3& position);
    
    // Getters
    const std::vector<MedievalGear>& getAvailableGear() const { return availableGear; }
    std::vector<MedievalGear> getGearByType(const std::string& type) const;
};

// Guild system manager
class GuildSystem {
private:
    std::vector<MedievalFaction> factions;
    std::map<int, std::string> playerFactions; // Player ID -> Faction name
    
public:
    GuildSystem();
    
    // Faction management
    void createFaction(const MedievalFaction& faction);
    void joinFaction(int playerId, const std::string& factionName);
    void leaveFaction(int playerId);
    void updateFactionRelationships();
    
    // Territory management
    void claimTerritory(const std::string& factionName, const glm::vec3& position);
    void loseTerritory(const std::string& factionName, const glm::vec3& position);
    void startSiege(const std::string& attackingFaction, const std::string& defendingFaction);
    
    // Corruption effects
    void corruptFaction(const std::string& factionName);
    void purifyFaction(const std::string& factionName);
    
    // Getters
    const std::vector<MedievalFaction>& getFactions() const { return factions; }
    std::string getPlayerFaction(int playerId) const;
    MedievalFaction* getFaction(const std::string& name);
    
    // Events
    void triggerFactionWar(const std::string& faction1, const std::string& faction2);
    void triggerGuildTournament();
    void triggerCorruptionSpread();
};