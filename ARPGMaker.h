#pragma once
#include "TimeTurner.h"
#include "BluejayArchetype.h"
#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <functional>
#include <yaml-cpp/yaml.h>

// American biomes for ARPG campaigns
enum class AmericanBiome {
    URBAN_NYC,          // New York City - skyscrapers, subways, Central Park
    URBAN_LA,           // Los Angeles - sprawl, Hollywood, beaches
    URBAN_CHICAGO,      // Chicago - architecture, Lake Michigan, neighborhoods
    RURAL_FARM,         // American farmland - cornfields, barns, silos
    RURAL_SMALL_TOWN,  // Small town America - main street, diners, gas stations
    RURAL_GHOST_TOWN,  // Abandoned mining towns - saloons, general stores, mines
    WILD_GRAND_CANYON, // Grand Canyon - red rocks, canyons, desert
    WILD_YELLOWSTONE,  // Yellowstone - geysers, forests, wildlife
    WILD_ROCKIES,      // Rocky Mountains - peaks, forests, alpine
    WILD_BADLANDS,     // Badlands - eroded hills, prairies, fossils
    WILD_SWAMP,        // Southern swamps - bayous, alligators, Spanish moss
    WEIRD_VEGAS,       // Las Vegas Strip - casinos, neon, desert
    WEIRD_ROUTE_66,    // Route 66 - diners, motels, roadside attractions
    WEIRD_AREA_51,     // Area 51 - desert, UFOs, government secrets
    WEIRD_NUCLEAR,     // Nuclear test sites - radiation, bunkers, craters
    WEIRD_MINING       // Old mining towns - ghost towns, mines, gold rush
};

// Quest types with American flavor
enum class QuestType {
    MAIN_STORY,         // Main campaign storyline
    SIDE_QUEST,         // Optional side content
    DAILY_QUEST,        // Daily repeatable quests
    WEEKLY_QUEST,       // Weekly repeatable quests
    EVENT_QUEST,        // Special event quests
    FACTION_QUEST,      // Faction-specific quests
    BOSS_QUEST,         // Boss encounter quests
    COLLECTION_QUEST,   // Item collection quests
    ESCORT_QUEST,       // NPC escort missions
    DEFENSE_QUEST      // Defend location quests
};

// American cultural references
enum class CulturalReference {
    WESTERN,            // Classic westerns - cowboys, saloons, outlaws
    HORROR_80S,         // 80s horror - slashers, supernatural, camp
    CYBERPUNK,          // Cyberpunk - neon, tech, corporate dystopia
    POST_APOCALYPTIC,   // Post-apocalyptic - wasteland, survival, mutants
    URBAN_LEGEND,       // Urban legends - cryptids, ghosts, mysteries
    POLITICAL,          // Political - government, conspiracy, corruption
    SPORTS,             // Sports - football, baseball, basketball
    MUSIC,              // Music - rock, country, hip-hop, jazz
    FOOD,               // Food - diners, BBQ, regional specialties
    TRANSPORTATION      // Transportation - cars, trains, planes, ships
};

// Quest structure
struct Quest {
    std::string id;
    std::string title;
    std::string description;
    QuestType type;
    int level;
    std::vector<std::string> objectives;
    std::vector<std::string> rewards;
    std::string giver;           // NPC who gives the quest
    std::string location;        // Where quest takes place
    AmericanBiome biome;
    CulturalReference culturalRef;
    bool isCompleted = false;
    bool isActive = false;
    float timeLimit = 0.0f;      // 0 = no time limit
    std::map<std::string, int> requirements; // Skill/level requirements
    
    void updateQuest(float deltaTime);
    bool isExpired() const;
    std::string getStatus() const;
    std::string getCulturalFlavor() const;
};

// Campaign data structure
struct CampaignData {
    std::string id;
    std::string name;
    std::string description;
    std::string author;
    std::string version;
    AmericanBiome primaryBiome;
    std::vector<CulturalReference> culturalThemes;
    std::vector<Quest> quests;
    std::vector<std::string> npcs;
    std::vector<std::string> locations;
    std::vector<std::string> items;
    std::vector<std::string> enemies;
    std::vector<std::string> bosses;
    int minLevel = 1;
    int maxLevel = 100;
    float difficulty = 1.0f;
    bool isPublic = false;
    bool isRated = false;
    float rating = 0.0f;
    int playCount = 0;
    std::string thumbnail;
    std::string tags;
    
    void addQuest(const Quest& quest);
    void removeQuest(const std::string& questId);
    Quest* getQuest(const std::string& questId);
    std::vector<Quest> getQuestsByType(QuestType type) const;
    std::vector<Quest> getQuestsByBiome(AmericanBiome biome) const;
    std::vector<Quest> getQuestsByCulturalRef(CulturalReference ref) const;
};

// ARPG character progression
struct ARPGCharacter {
    int playerId;
    std::string name;
    int level = 1;
    int experience = 0;
    int experienceToNext = 100;
    std::map<std::string, int> stats;
    std::map<std::string, int> skills;
    std::vector<std::string> abilities;
    std::vector<std::string> inventory;
    std::string archetype;
    std::string faction;
    glm::vec3 position;
    bool isAlive = true;
    
    void addExperience(int exp);
    void levelUp();
    void addStat(const std::string& stat, int value);
    void addSkill(const std::string& skill, int value);
    void addAbility(const std::string& ability);
    void addItem(const std::string& item);
    void removeItem(const std::string& item);
    bool hasItem(const std::string& item) const;
    int getStat(const std::string& stat) const;
    int getSkill(const std::string& skill) const;
    bool hasAbility(const std::string& ability) const;
    std::string getLevelDescription() const;
};

// American-themed loot system
struct AmericanLoot {
    std::string id;
    std::string name;
    std::string description;
    std::string category;        // "weapon", "armor", "consumable", "relic"
    int rarity;                 // 0-5 (common to mythic)
    int value;
    std::vector<std::string> effects;
    AmericanBiome biome;
    CulturalReference culturalRef;
    std::string flavorText;
    
    std::string getRarityName() const;
    glm::vec3 getRarityColor() const;
    std::string getCulturalDescription() const;
    bool isBiomeAppropriate(AmericanBiome targetBiome) const;
};

// Boss system with American flavor
struct AmericanBoss {
    std::string id;
    std::string name;
    std::string description;
    int level;
    float health;
    float maxHealth;
    std::vector<std::string> abilities;
    std::vector<std::string> phases;
    AmericanBiome biome;
    CulturalReference culturalRef;
    std::vector<std::string> loot;
    std::string flavorText;
    bool isActive = false;
    glm::vec3 position;
    
    void updateBoss(float deltaTime);
    void triggerPhase(int phase);
    void addAbility(const std::string& ability);
    void removeAbility(const std::string& ability);
    bool hasAbility(const std::string& ability) const;
    std::string getCulturalDescription() const;
    std::string getBiomeDescription() const;
};

// ARPG Maker main class
class ARPGMaker {
private:
    std::map<std::string, CampaignData> campaigns;
    std::map<int, ARPGCharacter> characters;
    std::map<std::string, AmericanLoot> lootTable;
    std::map<std::string, AmericanBoss> bosses;
    std::vector<Quest> activeQuests;
    std::vector<AmericanBoss> activeBosses;
    
    // Campaign management
    void loadCampaign(const std::string& campaignId);
    void saveCampaign(const std::string& campaignId);
    void validateCampaign(const CampaignData& campaign);
    
    // Quest management
    void updateQuests(float deltaTime);
    void completeQuest(const std::string& questId, int playerId);
    void failQuest(const std::string& questId, int playerId);
    
    // Character management
    void updateCharacters(float deltaTime);
    void levelUpCharacter(int playerId);
    void distributeRewards(int playerId, const std::vector<std::string>& rewards);
    
    // Boss management
    void updateBosses(float deltaTime);
    void spawnBoss(const std::string& bossId, const glm::vec3& position);
    void defeatBoss(const std::string& bossId, int playerId);
    
public:
    ARPGMaker();
    ~ARPGMaker();
    
    // Campaign management
    void createCampaign(const CampaignData& campaign);
    void editCampaign(const std::string& campaignId, const CampaignData& campaign);
    void deleteCampaign(const std::string& campaignId);
    void publishCampaign(const std::string& campaignId);
    void rateCampaign(const std::string& campaignId, float rating);
    
    // Campaign queries
    std::vector<CampaignData> getPublicCampaigns() const;
    std::vector<CampaignData> getCampaignsByBiome(AmericanBiome biome) const;
    std::vector<CampaignData> getCampaignsByCulturalRef(CulturalReference ref) const;
    std::vector<CampaignData> getCampaignsByLevel(int level) const;
    CampaignData* getCampaign(const std::string& campaignId);
    
    // Quest management
    void startQuest(const std::string& questId, int playerId);
    void completeQuest(const std::string& questId, int playerId);
    void abandonQuest(const std::string& questId, int playerId);
    void updateQuestProgress(const std::string& questId, int playerId, const std::string& objective);
    
    // Character management
    void createCharacter(int playerId, const std::string& name, const std::string& archetype);
    void updateCharacter(int playerId, const ARPGCharacter& character);
    void deleteCharacter(int playerId);
    ARPGCharacter* getCharacter(int playerId);
    
    // Loot management
    void addLoot(const AmericanLoot& loot);
    void removeLoot(const std::string& lootId);
    AmericanLoot* getLoot(const std::string& lootId);
    std::vector<AmericanLoot> getLootByBiome(AmericanBiome biome) const;
    std::vector<AmericanLoot> getLootByRarity(int rarity) const;
    
    // Boss management
    void addBoss(const AmericanBoss& boss);
    void removeBoss(const std::string& bossId);
    AmericanBoss* getBoss(const std::string& bossId);
    void spawnBoss(const std::string& bossId, const glm::vec3& position);
    void defeatBoss(const std::string& bossId, int playerId);
    
    // Game loop
    void update(float deltaTime);
    void startCampaign(const std::string& campaignId);
    void endCampaign();
    
    // Callbacks
    std::function<void(int, const std::string&)> onQuestCompleted;
    std::function<void(int, const std::string&)> onQuestFailed;
    std::function<void(int, int)> onCharacterLevelUp;
    std::function<void(int, const std::string&)> onBossDefeated;
    std::function<void(const std::string&)> onCampaignStarted;
    std::function<void(const std::string&)> onCampaignEnded;
    
    // Getters
    const std::map<std::string, CampaignData>& getCampaigns() const { return campaigns; }
    const std::map<int, ARPGCharacter>& getCharacters() const { return characters; }
    const std::map<std::string, AmericanLoot>& getLootTable() const { return lootTable; }
    const std::map<std::string, AmericanBoss>& getBosses() const { return bosses; }
    const std::vector<Quest>& getActiveQuests() const { return activeQuests; }
    const std::vector<AmericanBoss>& getActiveBosses() const { return activeBosses; }
    
    // Statistics
    struct ARPGStats {
        int totalCampaigns = 0;
        int totalCharacters = 0;
        int totalQuests = 0;
        int totalBosses = 0;
        int totalLoot = 0;
        float averageRating = 0.0f;
        int totalPlayTime = 0;
    };
    ARPGStats getStats() const;
};

// Campaign Editor
class CampaignEditor {
private:
    CampaignData currentCampaign;
    bool isEditing = false;
    std::string currentCampaignId;
    
    // Editor tools
    void addQuest(const Quest& quest);
    void editQuest(const std::string& questId, const Quest& quest);
    void removeQuest(const std::string& questId);
    void addNPC(const std::string& npcId);
    void addLocation(const std::string& locationId);
    void addItem(const std::string& itemId);
    void addEnemy(const std::string& enemyId);
    void addBoss(const std::string& bossId);
    
public:
    CampaignEditor();
    
    // Campaign editing
    void startEditing(const std::string& campaignId);
    void createNewCampaign(const std::string& name, const std::string& author);
    void saveCampaign();
    void publishCampaign();
    void cancelEditing();
    
    // Quest editing
    void addQuest(const Quest& quest);
    void editQuest(const std::string& questId, const Quest& quest);
    void removeQuest(const std::string& questId);
    void duplicateQuest(const std::string& questId);
    
    // Content editing
    void addNPC(const std::string& npcId, const std::string& name, const std::string& description);
    void addLocation(const std::string& locationId, const std::string& name, AmericanBiome biome);
    void addItem(const std::string& itemId, const AmericanLoot& loot);
    void addEnemy(const std::string& enemyId, const std::string& name, int level);
    void addBoss(const std::string& bossId, const AmericanBoss& boss);
    
    // Campaign queries
    CampaignData* getCurrentCampaign();
    std::vector<Quest> getQuests() const;
    std::vector<std::string> getNPCs() const;
    std::vector<std::string> getLocations() const;
    std::vector<std::string> getItems() const;
    std::vector<std::string> getEnemies() const;
    std::vector<std::string> getBosses() const;
    
    // Validation
    bool validateCampaign() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Export/Import
    void exportCampaign(const std::string& filename);
    void importCampaign(const std::string& filename);
    void exportToYAML(const std::string& filename);
    void importFromYAML(const std::string& filename);
    
    // Getters
    bool isEditingCampaign() const { return isEditing; }
    std::string getCurrentCampaignId() const { return currentCampaignId; }
};

// American cultural reference system
class CulturalReferenceSystem {
private:
    std::map<CulturalReference, std::vector<std::string>> referenceData;
    std::map<AmericanBiome, std::vector<CulturalReference>> biomeReferences;
    
public:
    CulturalReferenceSystem();
    
    // Reference management
    void addReference(CulturalReference ref, const std::string& reference);
    void removeReference(CulturalReference ref, const std::string& reference);
    std::vector<std::string> getReferences(CulturalReference ref) const;
    
    // Biome associations
    void associateBiomeWithReference(AmericanBiome biome, CulturalReference ref);
    std::vector<CulturalReference> getReferencesForBiome(AmericanBiome biome) const;
    
    // Flavor text generation
    std::string generateFlavorText(CulturalReference ref, AmericanBiome biome) const;
    std::string generateQuestDescription(QuestType type, CulturalReference ref, AmericanBiome biome) const;
    std::string generateBossDescription(const std::string& bossName, CulturalReference ref, AmericanBiome biome) const;
    
    // Random generation
    CulturalReference getRandomReference() const;
    CulturalReference getRandomReferenceForBiome(AmericanBiome biome) const;
    std::string getRandomReferenceText(CulturalReference ref) const;
};

// Utility functions
namespace ARPGUtils {
    std::string getBiomeName(AmericanBiome biome);
    std::string getCulturalReferenceName(CulturalReference ref);
    std::string getQuestTypeName(QuestType type);
    glm::vec3 getBiomeColor(AmericanBiome biome);
    std::string getBiomeDescription(AmericanBiome biome);
    std::string getCulturalFlavor(CulturalReference ref, AmericanBiome biome);
}