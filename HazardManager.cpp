#include "UniversalBattleRoyale.h"
#include <algorithm>
#include <cmath>

// HazardManager implementation for Hunger Games events

void HazardManager::applyFireWall(const ArenaEvent& event, std::vector<Tribute>& tributes) {
    // FireWall: expanding ring of flame that herds players
    for (auto& tribute : tributes) {
        if (!tribute.alive) continue;
        
        float distance = glm::length(tribute.position - event.center);
        if (distance <= event.radius) {
            // Apply fire damage
            float damage = event.intensity * 10.0f; // 10 damage per second
            // tribute.health -= damage * deltaTime; // Would need health system
            
            // Force movement away from fire
            glm::vec3 direction = glm::normalize(tribute.position - event.center);
            tribute.position += direction * 5.0f; // Push away from fire
            
            // Visual effect: fire particles, heat distortion
            // renderFireEffect(tribute.position, event.intensity);
        }
    }
}

void HazardManager::applyBeastWave(const ArenaEvent& event, std::vector<Tribute>& tributes) {
    // BeastWave: PvE swarm that attacks players
    int beastCount = static_cast<int>(20 * event.intensity); // 20-40 beasts based on intensity
    
    for (int i = 0; i < beastCount; i++) {
        // Spawn beast at random position in event radius
        float angle = randf(0.0f, 2.0f * M_PI);
        float radius = randf(0.0f, event.radius);
        glm::vec3 beastPos = event.center + glm::vec3(radius * cos(angle), 0.0f, radius * sin(angle));
        
        // Find nearest tribute
        Tribute* nearestTribute = nullptr;
        float nearestDistance = std::numeric_limits<float>::max();
        
        for (auto& tribute : tributes) {
            if (!tribute.alive) continue;
            
            float distance = glm::length(tribute.position - beastPos);
            if (distance < nearestDistance) {
                nearestDistance = distance;
                nearestTribute = &tribute;
            }
        }
        
        if (nearestTribute && nearestDistance < 50.0f) {
            // Beast attacks nearest tribute
            // nearestTribute->health -= 25.0f; // Beast damage
            
            // Visual effect: beast model, attack animation
            // renderBeastAttack(beastPos, nearestTribute->position);
        }
    }
}

void HazardManager::applyMazeShift(const ArenaEvent& event, std::vector<Tribute>& tributes) {
    // MazeShift: map reconfigures - walls slide to open/close routes
    // This integrates with your XQZ wireframe system
    
    // Update wall segments' transforms
    // This would call your XQZ map system to reconfigure walls
    // xqzMap.updateWallSegments(event.center, event.radius, event.intensity);
    
    // Force tributes to find new paths
    for (auto& tribute : tributes) {
        if (!tribute.alive) continue;
        
        float distance = glm::length(tribute.position - event.center);
        if (distance <= event.radius) {
            // Check if tribute is trapped by new walls
            // bool isTrapped = xqzMap.isPositionTrapped(tribute.position);
            // if (isTrapped) {
            //     // Force tribute to move to nearest open area
            //     glm::vec3 newPos = xqzMap.findNearestOpenPosition(tribute.position);
            //     tribute.position = newPos;
            // }
            
            // Visual effect: walls sliding, wireframe morphing
            // renderMazeShiftEffect(event.center, event.radius, event.intensity);
        }
    }
}

void HazardManager::applyAcidRain(const ArenaEvent& event, std::vector<Tribute>& tributes) {
    // AcidRain: corrosive rain that damages players
    for (auto& tribute : tributes) {
        if (!tribute.alive) continue;
        
        float distance = glm::length(tribute.position - event.center);
        if (distance <= event.radius) {
            // Apply acid damage
            float damage = event.intensity * 5.0f; // 5 damage per second
            // tribute.health -= damage * deltaTime;
            
            // Reduce movement speed (acid burns)
            // tribute.movementSpeed *= 0.7f;
            
            // Visual effect: acid rain particles, corrosion
            // renderAcidRainEffect(tribute.position, event.intensity);
        }
    }
}

void HazardManager::applyCometShower(const ArenaEvent& event, std::vector<Tribute>& tributes) {
    // CometShower: meteors rain down from the sky
    int cometCount = static_cast<int>(15 * event.intensity); // 15-30 comets based on intensity
    
    for (int i = 0; i < cometCount; i++) {
        // Spawn comet at random position in event radius
        float angle = randf(0.0f, 2.0f * M_PI);
        float radius = randf(0.0f, event.radius);
        glm::vec3 cometPos = event.center + glm::vec3(radius * cos(angle), 0.0f, radius * sin(angle));
        
        // Check for tributes near impact
        for (auto& tribute : tributes) {
            if (!tribute.alive) continue;
            
            float distance = glm::length(tribute.position - cometPos);
            if (distance < 20.0f) { // Impact radius
                // Apply comet damage
                float damage = event.intensity * 50.0f; // 50 damage on impact
                // tribute.health -= damage;
                
                // Knockback effect
                glm::vec3 knockback = glm::normalize(tribute.position - cometPos) * 10.0f;
                tribute.position += knockback;
                
                // Visual effect: comet impact, explosion
                // renderCometImpact(cometPos, event.intensity);
            }
        }
    }
}

// Main apply function that routes to specific event handlers
void HazardManager::apply(const ArenaEvent& event, std::vector<Tribute>& tributes) {
    if (!event.isActive) return;
    
    if (event.type == "fireWall") {
        applyFireWall(event, tributes);
    } else if (event.type == "beastWave") {
        applyBeastWave(event, tributes);
    } else if (event.type == "mazeShift") {
        applyMazeShift(event, tributes);
    } else if (event.type == "acidRain") {
        applyAcidRain(event, tributes);
    } else if (event.type == "cometShower") {
        applyCometShower(event, tributes);
    }
}

// Additional utility functions for Hunger Games

void HazardManager::placeTrapRing(const glm::vec3& center, float radius) {
    // Place traps around cornucopia
    int trapCount = 20;
    for (int i = 0; i < trapCount; i++) {
        float angle = (2.0f * M_PI * i) / trapCount;
        glm::vec3 trapPos = center + glm::vec3(radius * cos(angle), 0.0f, radius * sin(angle));
        
        // Spawn trap
        spawnHazard("blade_trap", trapPos);
    }
}

void HazardManager::spawnHazard(const std::string& type, const glm::vec3& position) {
    HazardEvent hazard;
    hazard.type = type;
    hazard.position = position;
    hazard.radius = 5.0f;
    hazard.damage = 25.0f;
    hazard.duration = 300.0f; // 5 minutes
    hazard.isActive = true;
    hazard.description = "Hazard: " + type;
    
    activeHazards.push_back(hazard);
}

// Event weight adjustments based on game phase
void HungerGamesMode::adjustEventWeightsForPhase(float gamePhase) {
    // gamePhase: 0.0 = start, 1.0 = end
    // Increase event intensity as game progresses
    
    float phaseMultiplier = 1.0f + (gamePhase * 2.0f); // 1x to 3x intensity
    
    eventWeights.fireWall *= phaseMultiplier;
    eventWeights.beastWave *= phaseMultiplier;
    eventWeights.mazeShift *= phaseMultiplier;
    eventWeights.acidRain *= phaseMultiplier;
    eventWeights.cometShower *= phaseMultiplier;
}

// Integration with TimeTurner system
void HungerGamesMode::adaptToWorldSettings(const WorldSettings& settings) {
    // Adapt events based on era and hazards
    
    if (settings.era == Era::MODERN) {
        // Modern events: gas attacks, collapsing buildings
        eventWeights.fireWall = 0.006f; // More frequent
        eventWeights.acidRain = 0.004f; // Gas attacks
    } else if (settings.era == Era::WASTELAND) {
        // Wasteland events: radiation storms, mutant waves
        eventWeights.beastWave = 0.005f; // More mutants
        eventWeights.acidRain = 0.003f; // Radiation storms
    } else if (settings.era == Era::FRONTIER) {
        // Frontier events: dust storms, stampedes
        eventWeights.fireWall = 0.002f; // Dust storms
        eventWeights.beastWave = 0.004f; // Stampedes
    } else if (settings.era == Era::MEDIEVAL) {
        // Medieval events: demonic corruption, magical storms
        eventWeights.beastWave = 0.006f; // Demonic hordes
        eventWeights.cometShower = 0.004f; // Magical meteors
    } else if (settings.era == Era::WESTWORLD) {
        // Westworld events: simulation glitches, reality distortions
        eventWeights.mazeShift = 0.008f; // Reality glitches
        eventWeights.fireWall = 0.003f; // Simulation errors
    }
    
    // Apply hazard overlays
    if (settings.hazard == Hazard::NUCLEAR_FALLOUT) {
        eventWeights.acidRain = 0.007f; // Radiation storms
    } else if (settings.hazard == Hazard::WARZONE) {
        eventWeights.cometShower = 0.006f; // Airstrikes
    } else if (settings.hazard == Hazard::DEMONIC_CORRUPTION) {
        eventWeights.beastWave = 0.008f; // Demon hordes
        eventWeights.fireWall = 0.005f; // Hellfire
    }
}

// Networking integration
void HungerGamesMode::serializeGameState(std::vector<uint8_t>& data) const {
    // Serialize game state for networking
    // This would integrate with your networking system
    
    // Serialize tributes
    data.push_back(static_cast<uint8_t>(tributes.size()));
    for (const auto& tribute : tributes) {
        // Serialize tribute data
        // serializeTribute(tribute, data);
    }
    
    // Serialize events
    data.push_back(static_cast<uint8_t>(liveEvents.size()));
    for (const auto& event : liveEvents) {
        // Serialize event data
        // serializeEvent(event, data);
    }
    
    // Serialize zone
    // serializeZone(zoneManager, data);
}

void HungerGamesMode::deserializeGameState(const std::vector<uint8_t>& data) {
    // Deserialize game state from network
    // This would integrate with your networking system
    
    size_t offset = 0;
    
    // Deserialize tributes
    uint8_t tributeCount = data[offset++];
    tributes.clear();
    tributes.reserve(tributeCount);
    
    for (int i = 0; i < tributeCount; i++) {
        // Deserialize tribute data
        // Tribute tribute = deserializeTribute(data, offset);
        // tributes.push_back(tribute);
    }
    
    // Deserialize events
    uint8_t eventCount = data[offset++];
    liveEvents.clear();
    liveEvents.reserve(eventCount);
    
    for (int i = 0; i < eventCount; i++) {
        // Deserialize event data
        // ArenaEvent event = deserializeEvent(data, offset);
        // liveEvents.push_back(event);
    }
    
    // Deserialize zone
    // deserializeZone(zoneManager, data, offset);
}