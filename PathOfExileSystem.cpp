#include "PathOfExileSystem.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <fstream>
#include <yaml-cpp/yaml.h>

// Item implementation
void Item::addAffix(const std::string& name, float value) {
    affixes[name] = value;
}

void Item::removeAffix(const std::string& name) {
    affixes.erase(name);
}

bool Item::hasAffix(const std::string& name) const {
    return affixes.find(name) != affixes.end();
}

float Item::getAffixValue(const std::string& name) const {
    auto it = affixes.find(name);
    return (it != affixes.end()) ? it->second : 0.0f;
}

std::string Item::getRarityName() const {
    switch (rarity) {
        case 0: return "Normal";
        case 1: return "Magic";
        case 2: return "Rare";
        case 3: return "Unique";
        case 4: return "Legendary";
        default: return "Unknown";
    }
}

glm::vec3 Item::getRarityColor() const {
    switch (rarity) {
        case 0: return glm::vec3(1.0f, 1.0f, 1.0f);      // White
        case 1: return glm::vec3(0.0f, 0.5f, 1.0f);      // Blue
        case 2: return glm::vec3(1.0f, 1.0f, 0.0f);      // Yellow
        case 3: return glm::vec3(0.8f, 0.0f, 0.8f);      // Purple
        case 4: return glm::vec3(1.0f, 0.5f, 0.0f);      // Orange
        default: return glm::vec3(0.5f, 0.5f, 0.5f);     // Gray
    }
}

std::string Item::getDescription() const {
    std::string desc = baseType;
    if (rarity > 0) {
        desc = getRarityName() + " " + desc;
    }
    return desc;
}

bool Item::canSocketGem() const {
    return sockets > 0 && socketedGems.size() < sockets;
}

void Item::socketGem(const std::string& gemName) {
    if (canSocketGem()) {
        socketedGems.push_back(gemName);
    }
}

void Item::unsocketGem(int socketIndex) {
    if (socketIndex >= 0 && socketIndex < socketedGems.size()) {
        socketedGems.erase(socketedGems.begin() + socketIndex);
    }
}

// OreNode implementation
void OreNode::updateNode(float deltaTime) {
    if (isDepleted) {
        currentRespawnTime -= deltaTime;
        if (currentRespawnTime <= 0.0f) {
            respawn();
        }
    }
}

int OreNode::mineOre(int amount, int playerLevel) {
    if (isDepleted || !canMine(playerLevel)) {
        return 0;
    }
    
    int minedAmount = std::min(amount, remainingOre);
    remainingOre -= minedAmount;
    
    if (remainingOre <= 0) {
        isDepleted = true;
        currentRespawnTime = respawnTime;
    }
    
    return minedAmount;
}

void OreNode::respawn() {
    isDepleted = false;
    remainingOre = richness;
    currentRespawnTime = 0.0f;
}

bool OreNode::canMine(int playerLevel) const {
    return playerLevel >= minLevel;
}

std::string OreNode::getDescription() const {
    return type + " Ore (" + std::to_string(remainingOre) + "/" + std::to_string(richness) + ")";
}

// MapZone implementation
void MapZone::updateZone(float deltaTime) {
    for (auto& node : oreNodes) {
        node.updateNode(deltaTime);
    }
    
    for (auto& chest : lootChests) {
        chest.updateChest(deltaTime);
    }
    
    for (auto& spawn : spawnPoints) {
        spawn.updateSpawn(deltaTime);
    }
}

void MapZone::addOreNode(const OreNode& node) {
    oreNodes.push_back(node);
}

void MapZone::removeOreNode(const glm::vec3& position) {
    oreNodes.erase(std::remove_if(oreNodes.begin(), oreNodes.end(),
        [&position](const OreNode& node) {
            return glm::length(node.position - position) < 1.0f;
        }), oreNodes.end());
}

OreNode* MapZone::getOreNodeAt(const glm::vec3& position) {
    for (auto& node : oreNodes) {
        if (glm::length(node.position - position) < 1.0f) {
            return &node;
        }
    }
    return nullptr;
}

std::vector<OreNode> MapZone::getOreNodesByType(const std::string& type) const {
    std::vector<OreNode> result;
    for (const auto& node : oreNodes) {
        if (node.type == type) {
            result.push_back(node);
        }
    }
    return result;
}

// League implementation
void League::updateLeague(float deltaTime) {
    if (isActive && duration > 0.0f) {
        timeRemaining -= deltaTime;
        if (timeRemaining <= 0.0f) {
            deactivateLeague();
        }
    }
}

void League::activateLeague() {
    isActive = true;
    timeRemaining = duration;
}

void League::deactivateLeague() {
    isActive = false;
    timeRemaining = 0.0f;
}

bool League::isExpired() const {
    return duration > 0.0f && timeRemaining <= 0.0f;
}

void League::addZone(const MapZone& zone) {
    zones.push_back(zone);
}

void League::removeZone(int index) {
    if (index >= 0 && index < zones.size()) {
        zones.erase(zones.begin() + index);
    }
}

MapZone* League::getZone(int index) {
    if (index >= 0 && index < zones.size()) {
        return &zones[index];
    }
    return nullptr;
}

std::vector<MapZone> League::getZonesByBiome(const std::string& biome) const {
    std::vector<MapZone> result;
    for (const auto& zone : zones) {
        if (zone.biome == biome) {
            result.push_back(zone);
        }
    }
    return result;
}

// SkillNode implementation
void SkillNode::addConnection(const std::string& nodeId) {
    if (std::find(connections.begin(), connections.end(), nodeId) == connections.end()) {
        connections.push_back(nodeId);
    }
}

void SkillNode::removeConnection(const std::string& nodeId) {
    connections.erase(std::remove(connections.begin(), connections.end(), nodeId), connections.end());
}

bool SkillNode::isConnectedTo(const std::string& nodeId) const {
    return std::find(connections.begin(), connections.end(), nodeId) != connections.end();
}

void SkillNode::addBonus(const std::string& stat, float value) {
    bonuses[stat] = value;
}

void SkillNode::removeBonus(const std::string& stat) {
    bonuses.erase(stat);
}

float SkillNode::getBonus(const std::string& stat) const {
    auto it = bonuses.find(stat);
    return (it != bonuses.end()) ? it->second : 0.0f;
}

std::string SkillNode::getDescription() const {
    return name + ": " + description;
}

// PlayerBuild implementation
void PlayerBuild::unlockNode(const std::string& nodeId) {
    if (std::find(unlockedNodes.begin(), unlockedNodes.end(), nodeId) == unlockedNodes.end()) {
        unlockedNodes.push_back(nodeId);
        updateStats();
    }
}

void PlayerBuild::lockNode(const std::string& nodeId) {
    unlockedNodes.erase(std::remove(unlockedNodes.begin(), unlockedNodes.end(), nodeId), unlockedNodes.end());
    updateStats();
}

bool PlayerBuild::hasNode(const std::string& nodeId) const {
    return std::find(unlockedNodes.begin(), unlockedNodes.end(), nodeId) != unlockedNodes.end();
}

void PlayerBuild::addItem(const Item& item) {
    inventory.push_back(item);
}

void PlayerBuild::removeItem(const std::string& itemId) {
    inventory.erase(std::remove_if(inventory.begin(), inventory.end(),
        [&itemId](const Item& item) {
            return item.baseType == itemId;
        }), inventory.end());
}

Item* PlayerBuild::findItem(const std::string& itemId) {
    for (auto& item : inventory) {
        if (item.baseType == itemId) {
            return &item;
        }
    }
    return nullptr;
}

void PlayerBuild::addToStash(const Item& item) {
    stash.push_back(item);
}

void PlayerBuild::removeFromStash(const std::string& itemId) {
    stash.erase(std::remove_if(stash.begin(), stash.end(),
        [&itemId](const Item& item) {
            return item.baseType == itemId;
        }), stash.end());
}

void PlayerBuild::levelUp() {
    level++;
    skillPoints += 2; // 2 skill points per level
    experience = 0;
    experienceToNext = level * 100; // Experience required increases with level
    updateStats();
}

void PlayerBuild::addExperience(int exp) {
    experience += exp;
    if (experience >= experienceToNext) {
        levelUp();
    }
}

void PlayerBuild::updateStats() {
    stats.clear();
    
    // Base stats from archetype
    if (archetype == "Bluejay") {
        stats["Speed"] = 1.3f;
        stats["Vision"] = 1.5f;
        stats["Health"] = 80.0f;
    } else if (archetype == "Bear") {
        stats["Health"] = 150.0f;
        stats["Defense"] = 1.4f;
        stats["Speed"] = 0.7f;
    }
    
    // Add bonuses from skill nodes
    // This would integrate with the skill tree system
    // for (const auto& nodeId : unlockedNodes) {
    //     SkillNode* node = getSkillNode(nodeId);
    //     if (node) {
    //         for (const auto& bonus : node->bonuses) {
    //             stats[bonus.first] += bonus.second;
    //         }
    //     }
    // }
}

std::string PlayerBuild::getBuildDescription() const {
    return "Level " + std::to_string(level) + " " + archetype + " with " + std::to_string(unlockedNodes.size()) + " skill nodes";
}

// CraftingCurrency implementation
void CraftingCurrency::useCurrency(Item& item) {
    // This would implement the specific currency effect
    // For example, "Orb of Transmutation" would change a normal item to magic
    if (effect == "transmute") {
        if (item.rarity == 0) {
            item.rarity = 1;
        }
    } else if (effect == "augment") {
        // Add a random affix
        // This would be implemented based on the specific currency
    }
}

bool CraftingCurrency::canUseOn(const Item& item) const {
    // Check if this currency can be used on this item
    if (effect == "transmute") {
        return item.rarity == 0;
    } else if (effect == "augment") {
        return item.rarity >= 1;
    }
    return false;
}

std::string CraftingCurrency::getEffectDescription() const {
    return name + ": " + effect;
}

// LootChest implementation
void LootChest::updateChest(float deltaTime) {
    if (isLooted && respawnTime > 0.0f) {
        currentRespawnTime -= deltaTime;
        if (currentRespawnTime <= 0.0f) {
            respawnChest();
        }
    }
}

std::vector<Item> LootChest::lootChest(int playerId) {
    if (isLooted || isLocked) {
        return {};
    }
    
    isLooted = true;
    currentRespawnTime = respawnTime;
    
    std::vector<Item> loot = contents;
    contents.clear();
    
    return loot;
}

void LootChest::respawnChest() {
    isLooted = false;
    currentRespawnTime = 0.0f;
    // Respawn with new loot
    // This would be implemented based on the chest type
}

bool LootChest::canLoot(int playerId) const {
    return !isLooted && !isLocked;
}

void LootChest::addItem(const Item& item) {
    contents.push_back(item);
}

void LootChest::removeItem(const std::string& itemId) {
    contents.erase(std::remove_if(contents.begin(), contents.end(),
        [&itemId](const Item& item) {
            return item.baseType == itemId;
        }), contents.end());
}

std::string LootChest::getDescription() const {
    return chestType + " chest with " + std::to_string(contents.size()) + " items";
}

// SpawnPoint implementation
void SpawnPoint::updateSpawn(float deltaTime) {
    if (isActive) {
        currentRespawnTime -= deltaTime;
    }
}

bool SpawnPoint::canSpawn() const {
    return isActive && currentRespawnTime <= 0.0f;
}

void SpawnPoint::spawnEntity(const std::string& entityType) {
    lastSpawned = entityType;
    resetSpawnTimer();
}

void SpawnPoint::resetSpawnTimer() {
    currentRespawnTime = respawnTime;
}

std::string SpawnPoint::getDescription() const {
    return spawnType + " spawn point (Level " + std::to_string(level) + ")";
}

// PathOfExileSystem implementation
PathOfExileSystem::PathOfExileSystem() {
    // Initialize default data
    initializeDefaultData();
}

PathOfExileSystem::~PathOfExileSystem() {
    // Cleanup
}

void PathOfExileSystem::initializeDefaultData() {
    // Initialize default skill nodes
    SkillNode bluejayNode;
    bluejayNode.id = "bluejay_vision";
    bluejayNode.name = "Enhanced Vision";
    bluejayNode.description = "Increases vision range by 25%";
    bluejayNode.bonuses["Vision"] = 0.25f;
    bluejayNode.cost = 1;
    bluejayNode.archetype = "Bluejay";
    skillTree["bluejay_vision"] = bluejayNode;
    
    SkillNode bearNode;
    bearNode.id = "bear_strength";
    bearNode.name = "Bear Strength";
    bearNode.description = "Increases health by 50%";
    bearNode.bonuses["Health"] = 0.5f;
    bearNode.cost = 1;
    bearNode.archetype = "Bear";
    skillTree["bear_strength"] = bearNode;
    
    // Initialize default currency
    CraftingCurrency transmuteOrb;
    transmuteOrb.name = "Orb of Transmutation";
    transmuteOrb.description = "Upgrades a normal item to magic";
    transmuteOrb.effect = "transmute";
    transmuteOrb.rarity = "common";
    currencyDatabase["transmute_orb"] = transmuteOrb;
    
    CraftingCurrency augmentOrb;
    augmentOrb.name = "Orb of Augmentation";
    augmentOrb.description = "Adds a random affix to a magic item";
    augmentOrb.effect = "augment";
    augmentOrb.rarity = "common";
    currencyDatabase["augment_orb"] = augmentOrb;
}

void PathOfExileSystem::createLeague(const League& league) {
    leagues[league.name] = league;
}

void PathOfExileSystem::activateLeague(const std::string& leagueName) {
    auto it = leagues.find(leagueName);
    if (it != leagues.end()) {
        it->second.activateLeague();
        activeLeague = leagueName;
        
        if (onLeagueStarted) {
            onLeagueStarted(leagueName);
        }
    }
}

void PathOfExileSystem::deactivateLeague(const std::string& leagueName) {
    auto it = leagues.find(leagueName);
    if (it != leagues.end()) {
        it->second.deactivateLeague();
        if (activeLeague == leagueName) {
            activeLeague = "";
        }
        
        if (onLeagueEnded) {
            onLeagueEnded(leagueName);
        }
    }
}

void PathOfExileSystem::updateLeague(const std::string& leagueName, const League& league) {
    leagues[leagueName] = league;
}

void PathOfExileSystem::deleteLeague(const std::string& leagueName) {
    leagues.erase(leagueName);
    if (activeLeague == leagueName) {
        activeLeague = "";
    }
}

std::vector<League> PathOfExileSystem::getActiveLeagues() const {
    std::vector<League> active;
    for (const auto& pair : leagues) {
        if (pair.second.isActive) {
            active.push_back(pair.second);
        }
    }
    return active;
}

std::vector<League> PathOfExileSystem::getLeaguesByLevel(int level) const {
    std::vector<League> result;
    for (const auto& pair : leagues) {
        if (level >= pair.second.minLevel && level <= pair.second.maxLevel) {
            result.push_back(pair.second);
        }
    }
    return result;
}

League* PathOfExileSystem::getLeague(const std::string& leagueName) {
    auto it = leagues.find(leagueName);
    return (it != leagues.end()) ? &it->second : nullptr;
}

void PathOfExileSystem::addSkillNode(const SkillNode& node) {
    skillTree[node.id] = node;
}

void PathOfExileSystem::removeSkillNode(const std::string& nodeId) {
    skillTree.erase(nodeId);
}

void PathOfExileSystem::connectNodes(const std::string& nodeId1, const std::string& nodeId2) {
    auto it1 = skillTree.find(nodeId1);
    auto it2 = skillTree.find(nodeId2);
    
    if (it1 != skillTree.end() && it2 != skillTree.end()) {
        it1->second.addConnection(nodeId2);
        it2->second.addConnection(nodeId1);
    }
}

void PathOfExileSystem::disconnectNodes(const std::string& nodeId1, const std::string& nodeId2) {
    auto it1 = skillTree.find(nodeId1);
    auto it2 = skillTree.find(nodeId2);
    
    if (it1 != skillTree.end() && it2 != skillTree.end()) {
        it1->second.removeConnection(nodeId2);
        it2->second.removeConnection(nodeId1);
    }
}

SkillNode* PathOfExileSystem::getSkillNode(const std::string& nodeId) {
    auto it = skillTree.find(nodeId);
    return (it != skillTree.end()) ? &it->second : nullptr;
}

std::vector<SkillNode> PathOfExileSystem::getNodesByArchetype(const std::string& archetype) const {
    std::vector<SkillNode> result;
    for (const auto& pair : skillTree) {
        if (pair.second.archetype == archetype) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<SkillNode> PathOfExileSystem::getConnectedNodes(const std::string& nodeId) const {
    std::vector<SkillNode> result;
    auto it = skillTree.find(nodeId);
    if (it != skillTree.end()) {
        for (const auto& connectedId : it->second.connections) {
            auto connectedIt = skillTree.find(connectedId);
            if (connectedIt != skillTree.end()) {
                result.push_back(connectedIt->second);
            }
        }
    }
    return result;
}

void PathOfExileSystem::createPlayerBuild(int playerId, const std::string& archetype) {
    PlayerBuild build;
    build.playerId = playerId;
    build.archetype = archetype;
    build.level = 1;
    build.experience = 0;
    build.experienceToNext = 100;
    build.skillPoints = 0;
    build.currentZone = "";
    build.currentZoneIndex = 0;
    build.playTime = 0.0f;
    
    playerBuilds[playerId] = build;
}

void PathOfExileSystem::updatePlayerBuild(int playerId, const PlayerBuild& build) {
    playerBuilds[playerId] = build;
}

void PathOfExileSystem::deletePlayerBuild(int playerId) {
    playerBuilds.erase(playerId);
}

PlayerBuild* PathOfExileSystem::getPlayerBuild(int playerId) {
    auto it = playerBuilds.find(playerId);
    return (it != playerBuilds.end()) ? &it->second : nullptr;
}

void PathOfExileSystem::unlockSkillNode(int playerId, const std::string& nodeId) {
    auto it = playerBuilds.find(playerId);
    if (it != playerBuilds.end()) {
        it->second.unlockNode(nodeId);
        
        if (onSkillNodeUnlocked) {
            onSkillNodeUnlocked(playerId, nodeId);
        }
    }
}

void PathOfExileSystem::lockSkillNode(int playerId, const std::string& nodeId) {
    auto it = playerBuilds.find(playerId);
    if (it != playerBuilds.end()) {
        it->second.lockNode(nodeId);
    }
}

bool PathOfExileSystem::canUnlockNode(int playerId, const std::string& nodeId) const {
    auto it = playerBuilds.find(playerId);
    if (it == playerBuilds.end()) return false;
    
    const PlayerBuild& build = it->second;
    auto nodeIt = skillTree.find(nodeId);
    if (nodeIt == skillTree.end()) return false;
    
    const SkillNode& node = nodeIt->second;
    
    // Check if player has enough skill points
    if (build.skillPoints < node.cost) return false;
    
    // Check level requirement
    if (build.level < node.levelRequirement) return false;
    
    // Check archetype requirement
    if (!node.archetype.empty() && build.archetype != node.archetype) return false;
    
    return true;
}

void PathOfExileSystem::addExperience(int playerId, int exp) {
    auto it = playerBuilds.find(playerId);
    if (it != playerBuilds.end()) {
        it->second.addExperience(exp);
        
        if (it->second.level > it->second.level) {
            if (onPlayerLevelUp) {
                onPlayerLevelUp(playerId, "Level " + std::to_string(it->second.level));
            }
        }
    }
}

void PathOfExileSystem::levelUpPlayer(int playerId) {
    auto it = playerBuilds.find(playerId);
    if (it != playerBuilds.end()) {
        it->second.levelUp();
        
        if (onPlayerLevelUp) {
            onPlayerLevelUp(playerId, "Level " + std::to_string(it->second.level));
        }
    }
}

void PathOfExileSystem::startSystem() {
    systemActive = true;
}

void PathOfExileSystem::stopSystem() {
    systemActive = false;
}

void PathOfExileSystem::updateSystem(float deltaTime) {
    if (!systemActive) return;
    
    updateLeagues(deltaTime);
    updateZones(deltaTime);
    updatePlayerBuilds(deltaTime);
}

void PathOfExileSystem::updateLeagues(float deltaTime) {
    for (auto& pair : leagues) {
        pair.second.updateLeague(deltaTime);
    }
}

void PathOfExileSystem::updateZones(float deltaTime) {
    for (auto& zone : currentZones) {
        zone.updateZone(deltaTime);
    }
}

void PathOfExileSystem::updatePlayerBuilds(float deltaTime) {
    for (auto& pair : playerBuilds) {
        pair.second.playTime += deltaTime;
    }
}

void PathOfExileSystem::resetSystem() {
    leagues.clear();
    skillTree.clear();
    playerBuilds.clear();
    itemDatabase.clear();
    currencyDatabase.clear();
    currentZones.clear();
    activeLeague = "";
    systemActive = false;
}

PathOfExileSystem::PoEStats PathOfExileSystem::getStats() const {
    PoEStats stats;
    stats.totalLeagues = leagues.size();
    stats.activeLeagues = getActiveLeagues().size();
    stats.totalPlayers = playerBuilds.size();
    stats.totalSkillNodes = skillTree.size();
    stats.totalItems = itemDatabase.size();
    stats.totalCurrency = currencyDatabase.size();
    
    // Calculate average player level
    if (!playerBuilds.empty()) {
        float totalLevel = 0.0f;
        for (const auto& pair : playerBuilds) {
            totalLevel += pair.second.level;
        }
        stats.averagePlayerLevel = totalLevel / playerBuilds.size();
    }
    
    // Calculate total play time
    for (const auto& pair : playerBuilds) {
        stats.totalPlayTime += static_cast<int>(pair.second.playTime);
    }
    
    return stats;
}

std::string PathOfExileSystem::getSystemStatus() const {
    std::string status = "Path of Exile System Status:\n";
    status += "Active: " + std::string(systemActive ? "Yes" : "No") + "\n";
    status += "Active League: " + activeLeague + "\n";
    status += "Total Leagues: " + std::to_string(leagues.size()) + "\n";
    status += "Total Players: " + std::to_string(playerBuilds.size()) + "\n";
    status += "Total Skill Nodes: " + std::to_string(skillTree.size()) + "\n";
    return status;
}

// OreTable implementation
OreTable::OreTable() {
    // Initialize default ore types
    OreNode ironOre;
    ironOre.type = "Iron";
    ironOre.richness = 100;
    ironOre.minLevel = 1;
    ironOre.biome = "city";
    oreTemplates["Iron"] = ironOre;
    oreRarity["Iron"] = 0.8f;
    
    OreNode goldOre;
    goldOre.type = "Gold";
    goldOre.richness = 50;
    goldOre.minLevel = 5;
    goldOre.biome = "wasteland";
    oreTemplates["Gold"] = goldOre;
    oreRarity["Gold"] = 0.3f;
    
    OreNode hellstoneOre;
    hellstoneOre.type = "Hellstone";
    hellstoneOre.richness = 75;
    hellstoneOre.minLevel = 10;
    hellstoneOre.biome = "hell";
    oreTemplates["Hellstone"] = hellstoneOre;
    oreRarity["Hellstone"] = 0.1f;
    
    // Initialize biome-ore associations
    biomeOres["city"] = {"Iron", "Steel", "Copper"};
    biomeOres["wasteland"] = {"Iron", "Gold", "Scrap", "Radium"};
    biomeOres["hell"] = {"Hellstone", "Soulstone", "Infernal Iron"};
    biomeOres["frontier"] = {"Iron", "Silver", "Coal"};
}

void OreTable::addOreType(const std::string& oreType, const OreNode& template) {
    oreTemplates[oreType] = template;
}

void OreTable::removeOreType(const std::string& oreType) {
    oreTemplates.erase(oreType);
    oreRarity.erase(oreType);
    
    // Remove from all biomes
    for (auto& pair : biomeOres) {
        pair.second.erase(std::remove(pair.second.begin(), pair.second.end(), oreType), pair.second.end());
    }
}

void OreTable::addOreToBiome(const std::string& biome, const std::string& oreType) {
    biomeOres[biome].push_back(oreType);
}

void OreTable::removeOreFromBiome(const std::string& biome, const std::string& oreType) {
    auto it = biomeOres.find(biome);
    if (it != biomeOres.end()) {
        it->second.erase(std::remove(it->second.begin(), it->second.end(), oreType), it->second.end());
    }
}

std::vector<OreNode> OreTable::generateOreForBiome(const std::string& biome, int count) const {
    std::vector<OreNode> result;
    auto it = biomeOres.find(biome);
    if (it == biomeOres.end()) return result;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, it->second.size() - 1);
    
    for (int i = 0; i < count; i++) {
        const std::string& oreType = it->second[dis(gen)];
        auto templateIt = oreTemplates.find(oreType);
        if (templateIt != oreTemplates.end()) {
            OreNode node = templateIt->second;
            // Randomize position and other properties
            node.position = glm::vec3(
                static_cast<float>(rand() % 1000 - 500),
                0.0f,
                static_cast<float>(rand() % 1000 - 500)
            );
            result.push_back(node);
        }
    }
    
    return result;
}

OreNode OreTable::generateRandomOre(const std::string& biome) const {
    auto it = biomeOres.find(biome);
    if (it == biomeOres.end() || it->second.empty()) {
        return OreNode();
    }
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, it->second.size() - 1);
    
    const std::string& oreType = it->second[dis(gen)];
    auto templateIt = oreTemplates.find(oreType);
    if (templateIt != oreTemplates.end()) {
        OreNode node = templateIt->second;
        node.position = glm::vec3(
            static_cast<float>(rand() % 1000 - 500),
            0.0f,
            static_cast<float>(rand() % 1000 - 500)
        );
        return node;
    }
    
    return OreNode();
}

std::vector<std::string> OreTable::getOresForBiome(const std::string& biome) const {
    auto it = biomeOres.find(biome);
    return (it != biomeOres.end()) ? it->second : std::vector<std::string>();
}

OreNode* OreTable::getOreTemplate(const std::string& oreType) {
    auto it = oreTemplates.find(oreType);
    return (it != oreTemplates.end()) ? &it->second : nullptr;
}

float OreTable::getOreRarity(const std::string& oreType) const {
    auto it = oreRarity.find(oreType);
    return (it != oreRarity.end()) ? it->second : 0.0f;
}

void OreTable::setOreRarity(const std::string& oreType, float rarity) {
    oreRarity[oreType] = std::clamp(rarity, 0.0f, 1.0f);
}

// Utility functions
namespace PoEUtils {
    std::string getRarityName(int rarity) {
        switch (rarity) {
            case 0: return "Normal";
            case 1: return "Magic";
            case 2: return "Rare";
            case 3: return "Unique";
            case 4: return "Legendary";
            default: return "Unknown";
        }
    }
    
    glm::vec3 getRarityColor(int rarity) {
        switch (rarity) {
            case 0: return glm::vec3(1.0f, 1.0f, 1.0f);      // White
            case 1: return glm::vec3(0.0f, 0.5f, 1.0f);      // Blue
            case 2: return glm::vec3(1.0f, 1.0f, 0.0f);      // Yellow
            case 3: return glm::vec3(0.8f, 0.0f, 0.8f);      // Purple
            case 4: return glm::vec3(1.0f, 0.5f, 0.0f);      // Orange
            default: return glm::vec3(0.5f, 0.5f, 0.5f);     // Gray
        }
    }
    
    std::string getBiomeName(const std::string& biome) {
        if (biome == "city") return "Urban";
        if (biome == "wasteland") return "Wasteland";
        if (biome == "hell") return "Hell";
        if (biome == "frontier") return "Frontier";
        return biome;
    }
    
    std::string getArchetypeName(const std::string& archetype) {
        if (archetype == "Bluejay") return "Bluejay Scout";
        if (archetype == "Bear") return "Bear Tank";
        if (archetype == "Wolf") return "Wolf Hunter";
        return archetype;
    }
    
    float calculateItemValue(const Item& item) {
        float baseValue = 10.0f; // Base value for normal items
        baseValue *= (1.0f + item.rarity * 0.5f); // Rarity multiplier
        baseValue *= (1.0f + item.itemLevel * 0.1f); // Level multiplier
        
        // Add affix values
        for (const auto& affix : item.affixes) {
            baseValue += affix.second * 0.1f;
        }
        
        return baseValue;
    }
    
    bool canUseItem(const Item& item, int playerLevel) {
        return playerLevel >= item.itemLevel;
    }
    
    std::string generateItemName(const Item& item) {
        std::string name = item.baseType;
        if (item.rarity > 0) {
            name = getRarityName(item.rarity) + " " + name;
        }
        return name;
    }
    
    std::string generateFlavorText(const Item& item) {
        if (item.flavorText.empty()) {
            return "A " + item.baseType + " of unknown origin.";
        }
        return item.flavorText;
    }
}