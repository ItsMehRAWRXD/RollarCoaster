#include "BluejayArchetype.h"
#include <algorithm>
#include <cmath>
#include <random>

// Archetype implementation
void Archetype::applyEraBonus(Era era, float bonus) {
    eraBonuses[era] = bonus;
}

void Archetype::applyHazardResistance(Hazard hazard, float resistance) {
    hazardResistances[hazard] = resistance;
}

float Archetype::getEraBonus(Era era) const {
    auto it = eraBonuses.find(era);
    return (it != eraBonuses.end()) ? it->second : 0.0f;
}

float Archetype::getHazardResistance(Hazard hazard) const {
    auto it = hazardResistances.find(hazard);
    return (it != hazardResistances.end()) ? it->second : 0.0f;
}

// BluejayArchetype implementation
BluejayArchetype::BluejayArchetype() {
    // Initialize Bluejay archetype data
    bluejayData.name = "Bluejay";
    bluejayData.type = ArchetypeType::BLUEJAY;
    bluejayData.speed = 1.3f;        // 30% faster movement
    bluejayData.health = 80.0f;      // 20% less health
    bluejayData.vision = 1.5f;       // 50% better vision
    bluejayData.stealth = 0.2f;      // 20% stealth bonus
    bluejayData.damage = 0.9f;       // 10% less damage
    bluejayData.defense = 0.8f;      // 20% less defense
    bluejayData.abilities = {"echoPing", "wallRun", "rooftopPerch", "lootDetection", "hazardSpotting", "earlyWarning"};
    bluejayData.description = "Fast, perceptive scout with territorial instincts";
    bluejayData.culturalFlavor = "American bluejay - territorial, intelligent, and adaptable";
    
    // Era-specific bonuses
    bluejayData.applyEraBonus(Era::MODERN, 0.2f);      // +20% speed in cities
    bluejayData.applyEraBonus(Era::WASTELAND, 0.3f);   // +30% loot detection in wasteland
    bluejayData.applyEraBonus(Era::FRONTIER, 0.25f);    // +25% vision in open areas
    bluejayData.applyEraBonus(Era::MEDIEVAL, 0.15f);   // +15% stealth in medieval
    bluejayData.applyEraBonus(Era::WESTWORLD, 0.4f);   // +40% glitch resistance in Westworld
    
    // Hazard resistances
    bluejayData.applyHazardResistance(Hazard::NUCLEAR_FALLOUT, 0.3f);  // 30% radiation resistance
    bluejayData.applyHazardResistance(Hazard::WARZONE, 0.2f);          // 20% warzone resistance
    bluejayData.applyHazardResistance(Hazard::STORM, 0.4f);            // 40% storm resistance
    bluejayData.applyHazardResistance(Hazard::PLAGUE, 0.25f);          // 25% plague resistance
    bluejayData.applyHazardResistance(Hazard::DEMONIC_CORRUPTION, 0.5f); // 50% corruption resistance
    
    // Initialize abilities
    abilities.wallRun = true;
    abilities.rooftopPerch = true;
    abilities.earlyWarning = true;
    abilities.lootDetection = true;
    abilities.hazardSpotting = true;
    abilities.echoPing = true;
}

void BluejayArchetype::updateArchetype(float deltaTime) {
    // Update echo ping cooldown
    if (echoPingCooldown > 0.0f) {
        echoPingCooldown -= deltaTime;
    }
    
    // Update echo ping duration
    if (echoPingActive) {
        echoPingTimeLeft -= deltaTime;
        if (echoPingTimeLeft <= 0.0f) {
            deactivateEchoPing();
        }
    }
}

void BluejayArchetype::activateEchoPing() {
    if (canEchoPing()) {
        echoPingActive = true;
        echoPingTimeLeft = echoPingDuration;
        echoPingCooldown = 30.0f; // 30 second cooldown
        
        if (onEchoPing) {
            onEchoPing(glm::vec3(0, 0, 0), echoPingRadius);
        }
    }
}

void BluejayArchetype::deactivateEchoPing() {
    echoPingActive = false;
    echoPingTimeLeft = 0.0f;
}

bool BluejayArchetype::canEchoPing() const {
    return echoPingCooldown <= 0.0f && !echoPingActive;
}

glm::vec3 BluejayArchetype::getWallRunDirection(const glm::vec3& position, const glm::vec3& direction) const {
    if (!abilities.wallRun) return direction;
    
    // Calculate wall run direction (simplified)
    // This would integrate with your physics system
    glm::vec3 wallRunDir = direction;
    wallRunDir.y = 0.0f; // Keep horizontal
    return glm::normalize(wallRunDir);
}

glm::vec3 BluejayArchetype::getRooftopPerchPosition(const glm::vec3& position) const {
    if (!abilities.rooftopPerch) return position;
    
    // Find nearest rooftop position
    // This would integrate with your map system
    glm::vec3 perchPos = position;
    perchPos.y += 10.0f; // Move up to rooftop level
    return perchPos;
}

std::vector<glm::vec3> BluejayArchetype::detectLoot(const glm::vec3& position, float radius) const {
    std::vector<glm::vec3> lootPositions;
    if (!abilities.lootDetection) return lootPositions;
    
    // Enhanced loot detection radius
    float enhancedRadius = radius * bluejayData.vision;
    
    // This would integrate with your loot system
    // lootPositions = lootManager.getLootInRadius(position, enhancedRadius);
    
    return lootPositions;
}

std::vector<glm::vec3> BluejayArchetype::spotHazards(const glm::vec3& position, float radius) const {
    std::vector<glm::vec3> hazardPositions;
    if (!abilities.hazardSpotting) return hazardPositions;
    
    // Enhanced hazard detection radius
    float enhancedRadius = radius * bluejayData.vision;
    
    // This would integrate with your hazard system
    // hazardPositions = hazardManager.getHazardsInRadius(position, enhancedRadius);
    
    return hazardPositions;
}

std::vector<glm::vec3> BluejayArchetype::getEarlyWarning(const glm::vec3& position, float radius) const {
    std::vector<glm::vec3> warningPositions;
    if (!abilities.earlyWarning) return warningPositions;
    
    // Early warning system - detect incoming threats
    float enhancedRadius = radius * bluejayData.vision * 1.5f; // 50% better early warning
    
    // This would integrate with your threat detection system
    // warningPositions = threatManager.getIncomingThreats(position, enhancedRadius);
    
    return warningPositions;
}

void BluejayArchetype::adaptToEra(Era era) {
    // Apply era-specific adaptations
    float eraBonus = bluejayData.getEraBonus(era);
    
    switch (era) {
        case Era::MODERN:
            // Enhanced mobility in cities
            bluejayData.speed = 1.3f + eraBonus;
            abilities.wallRun = true;
            abilities.rooftopPerch = true;
            break;
            
        case Era::WASTELAND:
            // Enhanced survival in wasteland
            bluejayData.vision = 1.5f + eraBonus;
            abilities.lootDetection = true;
            abilities.hazardSpotting = true;
            break;
            
        case Era::FRONTIER:
            // Enhanced scouting in open areas
            bluejayData.vision = 1.5f + eraBonus;
            abilities.earlyWarning = true;
            break;
            
        case Era::MEDIEVAL:
            // Enhanced stealth in medieval
            bluejayData.stealth = 0.2f + eraBonus;
            abilities.wallRun = false; // No walls to run on
            abilities.rooftopPerch = true;
            break;
            
        case Era::WESTWORLD:
            // Enhanced glitch resistance
            bluejayData.health = 80.0f + (eraBonus * 20.0f);
            abilities.earlyWarning = true;
            break;
    }
}

void BluejayArchetype::adaptToHazard(Hazard hazard) {
    // Apply hazard-specific adaptations
    float resistance = bluejayData.getHazardResistance(hazard);
    
    switch (hazard) {
        case Hazard::NUCLEAR_FALLOUT:
            // Enhanced radiation resistance
            bluejayData.health = 80.0f + (resistance * 20.0f);
            abilities.hazardSpotting = true;
            break;
            
        case Hazard::WARZONE:
            // Enhanced warzone survival
            bluejayData.defense = 0.8f + resistance;
            abilities.earlyWarning = true;
            break;
            
        case Hazard::STORM:
            // Enhanced storm resistance
            bluejayData.speed = 1.3f + resistance;
            abilities.hazardSpotting = true;
            break;
            
        case Hazard::PLAGUE:
            // Enhanced plague resistance
            bluejayData.health = 80.0f + (resistance * 15.0f);
            abilities.earlyWarning = true;
            break;
            
        case Hazard::DEMONIC_CORRUPTION:
            // Enhanced corruption resistance
            bluejayData.health = 80.0f + (resistance * 25.0f);
            abilities.hazardSpotting = true;
            break;
    }
}

void BluejayArchetype::adaptToWorldSettings(const WorldSettings& settings) {
    adaptToEra(settings.era);
    adaptToHazard(settings.hazard);
    
    // City living adaptations
    if (settings.cityLiving) {
        abilities.wallRun = true;
        abilities.rooftopPerch = true;
        bluejayData.speed = 1.4f; // Even faster in cities
    } else {
        abilities.wallRun = false;
        abilities.rooftopPerch = false;
        bluejayData.vision = 1.6f; // Better vision in open areas
    }
}

// EchoPing implementation
void EchoPing::updatePing(float deltaTime) {
    if (isActive) {
        timeLeft -= deltaTime;
        if (timeLeft <= 0.0f) {
            isActive = false;
        }
    }
}

bool EchoPing::isExpired() const {
    return !isActive || timeLeft <= 0.0f;
}

void EchoPing::addRevealedPlayer(int playerId) {
    if (std::find(revealedPlayers.begin(), revealedPlayers.end(), playerId) == revealedPlayers.end()) {
        revealedPlayers.push_back(playerId);
    }
}

bool EchoPing::hasRevealedPlayer(int playerId) const {
    return std::find(revealedPlayers.begin(), revealedPlayers.end(), playerId) != revealedPlayers.end();
}

// EchoPingManager implementation
EchoPingManager::EchoPingManager() {
    globalCooldown = 0.0f;
    globalCooldownDuration = 30.0f;
}

void EchoPingManager::createEchoPing(int playerId, const glm::vec3& position, float radius, float duration) {
    if (!canCreatePing()) return;
    
    EchoPing ping;
    ping.position = position;
    ping.radius = radius;
    ping.duration = duration;
    ping.timeLeft = duration;
    ping.playerId = playerId;
    ping.isActive = true;
    
    activePings.push_back(ping);
    globalCooldown = globalCooldownDuration;
    
    if (onPingCreated) {
        onPingCreated(ping);
    }
}

void EchoPingManager::updatePings(float deltaTime) {
    globalCooldown = std::max(0.0f, globalCooldown - deltaTime);
    
    for (auto& ping : activePings) {
        ping.updatePing(deltaTime);
    }
    
    removeExpiredPings();
}

void EchoPingManager::removeExpiredPings() {
    for (auto it = activePings.begin(); it != activePings.end();) {
        if (it->isExpired()) {
            if (onPingExpired) {
                onPingExpired(*it);
            }
            it = activePings.erase(it);
        } else {
            ++it;
        }
    }
}

std::vector<EchoPing> EchoPingManager::getPingsNearPosition(const glm::vec3& position, float radius) const {
    std::vector<EchoPing> nearbyPings;
    
    for (const auto& ping : activePings) {
        if (ping.isActive) {
            float distance = glm::length(ping.position - position);
            if (distance <= radius) {
                nearbyPings.push_back(ping);
            }
        }
    }
    
    return nearbyPings;
}

std::vector<int> EchoPingManager::getRevealedPlayers(const glm::vec3& position, float radius) const {
    std::vector<int> revealed;
    
    for (const auto& ping : activePings) {
        if (ping.isActive) {
            float distance = glm::length(ping.position - position);
            if (distance <= radius) {
                for (int playerId : ping.revealedPlayers) {
                    if (std::find(revealed.begin(), revealed.end(), playerId) == revealed.end()) {
                        revealed.push_back(playerId);
                    }
                }
            }
        }
    }
    
    return revealed;
}

bool EchoPingManager::isPlayerRevealed(int playerId, const glm::vec3& position, float radius) const {
    for (const auto& ping : activePings) {
        if (ping.isActive && ping.hasRevealedPlayer(playerId)) {
            float distance = glm::length(ping.position - position);
            if (distance <= radius) {
                return true;
            }
        }
    }
    return false;
}

bool EchoPingManager::canCreatePing() const {
    return globalCooldown <= 0.0f;
}

// ArchetypeManager implementation
ArchetypeManager::ArchetypeManager() {
    // Initialize default archetypes
    initializeDefaultArchetypes();
}

void ArchetypeManager::initializeDefaultArchetypes() {
    // Bluejay archetype
    Archetype bluejay;
    bluejay.name = "Bluejay";
    bluejay.type = ArchetypeType::BLUEJAY;
    bluejay.speed = 1.3f;
    bluejay.health = 80.0f;
    bluejay.vision = 1.5f;
    bluejay.stealth = 0.2f;
    bluejay.damage = 0.9f;
    bluejay.defense = 0.8f;
    bluejay.description = "Fast, perceptive scout with territorial instincts";
    bluejay.culturalFlavor = "American bluejay - territorial, intelligent, and adaptable";
    archetypes[ArchetypeType::BLUEJAY] = bluejay;
    
    // Bear archetype
    Archetype bear;
    bear.name = "Bear";
    bear.type = ArchetypeType::BEAR;
    bear.speed = 0.7f;
    bear.health = 150.0f;
    bear.vision = 0.8f;
    bear.stealth = -0.3f;
    bear.damage = 1.3f;
    bear.defense = 1.4f;
    bear.description = "Slow, strong tank with high defense";
    bear.culturalFlavor = "American grizzly - powerful, territorial, and protective";
    archetypes[ArchetypeType::BEAR] = bear;
    
    // Wolf archetype
    Archetype wolf;
    wolf.name = "Wolf";
    wolf.type = ArchetypeType::WOLF;
    wolf.speed = 1.1f;
    wolf.health = 100.0f;
    wolf.vision = 1.2f;
    wolf.stealth = 0.1f;
    wolf.damage = 1.1f;
    wolf.defense = 1.0f;
    wolf.description = "Balanced hunter with pack tactics";
    wolf.culturalFlavor = "American wolf - loyal, intelligent, and pack-oriented";
    archetypes[ArchetypeType::WOLF] = wolf;
    
    // Eagle archetype
    Archetype eagle;
    eagle.name = "Eagle";
    eagle.type = ArchetypeType::EAGLE;
    eagle.speed = 1.0f;
    eagle.health = 90.0f;
    eagle.vision = 2.0f;
    eagle.stealth = 0.3f;
    eagle.damage = 1.2f;
    eagle.defense = 0.9f;
    eagle.description = "Long-range sniper with high ground advantage";
    eagle.culturalFlavor = "American bald eagle - majestic, powerful, and patriotic";
    archetypes[ArchetypeType::EAGLE] = eagle;
    
    // Fox archetype
    Archetype fox;
    fox.name = "Fox";
    fox.type = ArchetypeType::FOX;
    fox.speed = 1.2f;
    fox.health = 85.0f;
    fox.vision = 1.3f;
    fox.stealth = 0.4f;
    fox.damage = 1.0f;
    fox.defense = 0.9f;
    fox.description = "Sneaky, cunning stealth specialist";
    fox.culturalFlavor = "American red fox - clever, adaptable, and resourceful";
    archetypes[ArchetypeType::FOX] = fox;
}

void ArchetypeManager::registerArchetype(const Archetype& archetype) {
    archetypes[archetype.type] = archetype;
}

void ArchetypeManager::setPlayerArchetype(int playerId, ArchetypeType type) {
    playerArchetypes[playerId] = type;
    
    // Create Bluejay instance if needed
    if (type == ArchetypeType::BLUEJAY) {
        createBluejayPlayer(playerId);
    }
}

ArchetypeType ArchetypeManager::getPlayerArchetype(int playerId) const {
    auto it = playerArchetypes.find(playerId);
    return (it != playerArchetypes.end()) ? it->second : ArchetypeType::WOLF; // Default to Wolf
}

const Archetype& ArchetypeManager::getArchetype(ArchetypeType type) const {
    auto it = archetypes.find(type);
    return (it != archetypes.end()) ? it->second : archetypes.at(ArchetypeType::WOLF);
}

void ArchetypeManager::createBluejayPlayer(int playerId) {
    bluejayPlayers[playerId] = BluejayArchetype();
}

void ArchetypeManager::removeBluejayPlayer(int playerId) {
    bluejayPlayers.erase(playerId);
}

BluejayArchetype* ArchetypeManager::getBluejayPlayer(int playerId) {
    auto it = bluejayPlayers.find(playerId);
    return (it != bluejayPlayers.end()) ? &it->second : nullptr;
}

void ArchetypeManager::updateArchetypes(float deltaTime) {
    // Update all Bluejay players
    for (auto& pair : bluejayPlayers) {
        pair.second.updateArchetype(deltaTime);
    }
}

void ArchetypeManager::adaptAllToEra(Era era) {
    for (auto& pair : bluejayPlayers) {
        pair.second.adaptToEra(era);
    }
}

void ArchetypeManager::adaptAllToHazard(Hazard hazard) {
    for (auto& pair : bluejayPlayers) {
        pair.second.adaptToHazard(hazard);
    }
}

void ArchetypeManager::adaptAllToWorldSettings(const WorldSettings& settings) {
    for (auto& pair : bluejayPlayers) {
        pair.second.adaptToWorldSettings(settings);
    }
}

int ArchetypeManager::getArchetypeCount(ArchetypeType type) const {
    int count = 0;
    for (const auto& pair : playerArchetypes) {
        if (pair.second == type) {
            count++;
        }
    }
    return count;
}

std::map<ArchetypeType, int> ArchetypeManager::getArchetypeDistribution() const {
    std::map<ArchetypeType, int> distribution;
    
    for (const auto& pair : playerArchetypes) {
        distribution[pair.second]++;
    }
    
    return distribution;
}