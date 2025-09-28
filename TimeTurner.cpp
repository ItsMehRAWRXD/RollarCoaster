#include "TimeTurner.h"
#include <algorithm>
#include <random>
#include <sstream>

// WorldSettings implementation
std::string WorldSettings::getDescription() const {
    std::stringstream ss;
    
    // Era description
    switch (era) {
        case Era::MODERN: ss << "Modern"; break;
        case Era::WASTELAND: ss << "Wasteland"; break;
        case Era::FRONTIER: ss << "Frontier"; break;
        case Era::WESTWORLD: ss << "Westworld"; break;
    }
    
    // Hazard description
    if (hazard != Hazard::NONE) {
        ss << " + ";
        switch (hazard) {
            case Hazard::NUCLEAR_FALLOUT: ss << "Nuclear Fallout"; break;
            case Hazard::WARZONE: ss << "Warzone"; break;
            case Hazard::STORM: ss << "Storm"; break;
            case Hazard::PLAGUE: ss << "Plague"; break;
            default: break;
        }
    }
    
    // City living
    ss << (cityLiving ? " (Urban)" : " (Rural)");
    
    return ss.str();
}

bool WorldSettings::isValid() const {
    // Check for invalid combinations
    if (hazardIntensity < 0.0f || hazardIntensity > 1.0f) return false;
    
    // Some era/hazard combinations might not make sense
    if (era == Era::FRONTIER && hazard == Hazard::NUCLEAR_FALLOUT) {
        // Frontier + Nuclear could be interesting (post-apocalyptic western)
        return true;
    }
    
    return true;
}

// NPC implementation
std::string NPC::getEraAppearance() const {
    switch (era) {
        case Era::MODERN:
            return hostile ? "Military uniform" : "Civilian clothes";
        case Era::WASTELAND:
            return hostile ? "Raider gear" : "Wasteland survivor";
        case Era::FRONTIER:
            return hostile ? "Bandit outfit" : "Frontier settler";
        case Era::WESTWORLD:
            return isHost ? "Host uniform" : "Guest attire";
    }
    return "Unknown";
}

std::string NPC::getEraBehavior() const {
    switch (era) {
        case Era::MODERN:
            return hostile ? "Military tactics" : "Civilian behavior";
        case Era::WASTELAND:
            return hostile ? "Raider aggression" : "Survival instincts";
        case Era::FRONTIER:
            return hostile ? "Bandit tactics" : "Frontier hospitality";
        case Era::WESTWORLD:
            return isHost ? "Host programming" : "Guest behavior";
    }
    return "Unknown";
}

bool NPC::shouldSpawnInEra(Era targetEra) const {
    // Some NPCs can exist across eras (like hosts in Westworld)
    if (targetEra == Era::WESTWORLD) return true;
    return era == targetEra;
}

// TimeTurner implementation
TimeTurner::TimeTurner() {
    settings = WorldSettings();
    worldNeedsRebuild = false;
}

void TimeTurner::toggleEra(Era newEra) {
    if (settings.era == newEra) return;
    
    Era oldEra = settings.era;
    settings.era = newEra;
    
    // Check if we can transition
    if (!canTransitionTo(newEra)) {
        settings.era = oldEra;
        return;
    }
    
    worldNeedsRebuild = true;
    rebuildWorld();
}

void TimeTurner::toggleHazard(Hazard newHazard) {
    if (settings.hazard == newHazard) return;
    
    Hazard oldHazard = settings.hazard;
    settings.hazard = newHazard;
    
    // Check if we can apply this hazard
    if (!canApplyHazard(newHazard)) {
        settings.hazard = oldHazard;
        return;
    }
    
    applyHazard();
}

void TimeTurner::toggleCity(bool enable) {
    if (settings.cityLiving == enable) return;
    
    settings.cityLiving = enable;
    rebuildCity();
}

void TimeTurner::toggleTimeShift(bool enable) {
    settings.enableTimeShift = enable;
}

void TimeTurner::setHazardIntensity(float intensity) {
    settings.hazardIntensity = std::clamp(intensity, 0.0f, 1.0f);
}

void TimeTurner::setCustomTheme(const std::string& theme) {
    settings.customTheme = theme;
}

void TimeTurner::randomizeSettings() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    // Random era
    std::uniform_int_distribution<> eraDist(0, 3);
    settings.era = static_cast<Era>(eraDist(gen));
    
    // Random hazard (including NONE)
    std::uniform_int_distribution<> hazardDist(0, 4);
    settings.hazard = static_cast<Hazard>(hazardDist(gen));
    
    // Random city living
    std::uniform_int_distribution<> cityDist(0, 1);
    settings.cityLiving = cityDist(gen) == 1;
    
    // Random hazard intensity
    std::uniform_real_distribution<> intensityDist(0.3f, 1.0f);
    settings.hazardIntensity = intensityDist(gen);
    
    worldNeedsRebuild = true;
    rebuildWorld();
}

void TimeTurner::resetToDefaults() {
    settings = WorldSettings();
    npcs.clear();
    worldNeedsRebuild = true;
    rebuildWorld();
}

void TimeTurner::update(float deltaTime) {
    if (worldNeedsRebuild) {
        rebuildWorld();
        worldNeedsRebuild = false;
    }
    
    // Update NPCs based on current era
    updateNPCsForEra();
    
    // Apply hazard effects over time
    if (settings.hazard != Hazard::NONE) {
        applyHazard();
    }
}

void TimeTurner::applyPreset(const std::string& presetName) {
    if (presetName == WorldPresets::FALLOUT_WASTELAND) {
        settings.era = Era::WASTELAND;
        settings.hazard = Hazard::NUCLEAR_FALLOUT;
        settings.cityLiving = false;
        settings.hazardIntensity = 0.8f;
    }
    else if (presetName == WorldPresets::COD_WARZONE) {
        settings.era = Era::MODERN;
        settings.hazard = Hazard::WARZONE;
        settings.cityLiving = true;
        settings.hazardIntensity = 1.0f;
    }
    else if (presetName == WorldPresets::RDR_FRONTIER) {
        settings.era = Era::FRONTIER;
        settings.hazard = Hazard::NONE;
        settings.cityLiving = false;
        settings.hazardIntensity = 0.0f;
    }
    else if (presetName == WorldPresets::WESTWORLD_SIMULATION) {
        settings.era = Era::WESTWORLD;
        settings.hazard = Hazard::NONE;
        settings.cityLiving = true;
        settings.enableTimeShift = true;
    }
    else if (presetName == WorldPresets::GTA_CHAOS) {
        settings.era = Era::MODERN;
        settings.hazard = Hazard::NONE;
        settings.cityLiving = true;
        settings.hostileNPCs = true;
    }
    else if (presetName == WorldPresets::COWBOY_BR_NUCLEAR) {
        settings.era = Era::FRONTIER;
        settings.hazard = Hazard::NUCLEAR_FALLOUT;
        settings.cityLiving = false;
        settings.hazardIntensity = 0.6f;
    }
    else if (presetName == WorldPresets::WESTWORLD_CASINO) {
        settings.era = Era::WESTWORLD;
        settings.hazard = Hazard::NONE;
        settings.cityLiving = true;
        settings.customTheme = "casino";
    }
    
    worldNeedsRebuild = true;
    rebuildWorld();
}

std::vector<std::string> TimeTurner::getAvailablePresets() const {
    return {
        WorldPresets::FALLOUT_WASTELAND,
        WorldPresets::COD_WARZONE,
        WorldPresets::RDR_FRONTIER,
        WorldPresets::WESTWORLD_SIMULATION,
        WorldPresets::GTA_CHAOS,
        WorldPresets::COWBOY_BR_NUCLEAR,
        WorldPresets::WESTWORLD_CASINO
    };
}

bool TimeTurner::canTransitionTo(Era targetEra) const {
    // All era transitions are allowed, but some might have special requirements
    if (targetEra == Era::WESTWORLD) {
        // Westworld can incorporate any era
        return true;
    }
    
    return true;
}

bool TimeTurner::canApplyHazard(Hazard targetHazard) const {
    // Some hazards might not make sense in certain eras
    if (settings.era == Era::FRONTIER && targetHazard == Hazard::NUCLEAR_FALLOUT) {
        // Post-apocalyptic western is actually cool
        return true;
    }
    
    return true;
}

// Private implementation methods
void TimeTurner::rebuildWorld() {
    // Clear existing NPCs
    npcs.clear();
    
    // Spawn era-appropriate NPCs
    spawnEraAppropriateNPCs();
    
    // Apply city/rural settings
    rebuildCity();
    
    // Apply any active hazards
    if (settings.hazard != Hazard::NONE) {
        applyHazard();
    }
}

void TimeTurner::applyHazard() {
    // This would integrate with your existing hazard systems
    // For now, we'll just mark NPCs as affected by hazards
    
    for (auto& npc : npcs) {
        if (settings.hazard == Hazard::NUCLEAR_FALLOUT) {
            // NPCs in irradiated areas might become mutants
            if (settings.hazardIntensity > 0.7f) {
                npc.hostile = true;
            }
        }
        else if (settings.hazard == Hazard::WARZONE) {
            // All NPCs become more aggressive in warzone
            npc.hostile = true;
        }
    }
}

void TimeTurner::rebuildCity() {
    // This would integrate with your map generation system
    // For now, we'll adjust NPC spawn patterns based on city/rural setting
    
    if (settings.cityLiving) {
        // Spawn more NPCs in urban areas
        // Adjust NPC behavior for city environment
    }
    else {
        // Spawn fewer NPCs in rural areas
        // Adjust for wilderness survival
    }
}

void TimeTurner::spawnEraAppropriateNPCs() {
    // Clear existing NPCs
    npcs.clear();
    
    // Spawn NPCs based on current era
    switch (settings.era) {
        case Era::MODERN:
            // Spawn civilians, police, military
            npcs.push_back({glm::vec3(0, 0, 0), "civilian", Era::MODERN, false, false, 100.0f, "none", "Hello there!"});
            npcs.push_back({glm::vec3(10, 0, 10), "police", Era::MODERN, false, false, 120.0f, "pistol", "Stay safe!"});
            break;
            
        case Era::WASTELAND:
            // Spawn raiders, mutants, survivors
            npcs.push_back({glm::vec3(0, 0, 0), "raider", Era::WASTELAND, true, false, 80.0f, "pipe_rifle", "Your caps or your life!"});
            npcs.push_back({glm::vec3(15, 0, 15), "survivor", Era::WASTELAND, false, false, 90.0f, "hunting_rifle", "Got any clean water?"});
            break;
            
        case Era::FRONTIER:
            // Spawn cowboys, bandits, settlers
            npcs.push_back({glm::vec3(0, 0, 0), "cowboy", Era::FRONTIER, false, false, 100.0f, "revolver", "Howdy partner!"});
            npcs.push_back({glm::vec3(20, 0, 20), "bandit", Era::FRONTIER, true, false, 85.0f, "shotgun", "This is a stickup!"});
            break;
            
        case Era::WESTWORLD:
            // Spawn hosts and guests
            npcs.push_back({glm::vec3(0, 0, 0), "host", Era::WESTWORLD, false, true, 150.0f, "none", "Welcome to Westworld!"});
            npcs.push_back({glm::vec3(25, 0, 25), "guest", Era::WESTWORLD, false, false, 100.0f, "pistol", "This place is incredible!"});
            break;
    }
}

void TimeTurner::updateNPCsForEra() {
    // Update NPC behavior based on current era
    for (auto& npc : npcs) {
        // Update NPC behavior, appearance, etc. based on current settings
        if (npc.era != settings.era && settings.era != Era::WESTWORLD) {
            // NPC doesn't belong in this era, might need to be removed or transformed
            if (settings.era == Era::WESTWORLD) {
                // In Westworld, any era NPC can exist
                continue;
            }
        }
    }
}