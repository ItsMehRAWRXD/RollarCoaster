#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

// Era definitions - baseline themes that define the world
enum class Era {
    MODERN,      // Contemporary settings (GTA, CoD, MoH)
    WASTELAND,   // Post-apocalyptic (Fallout)
    FRONTIER,    // 1500s-1800s frontier (RDR, Westworld)
    WESTWORLD,   // Meta-simulation layer with mixed eras
    MEDIEVAL     // Medieval fantasy (castles, knights, feudal systems)
};

// Hazard overlays - environmental threats that can be applied to any era
enum class Hazard {
    NONE,           // No additional hazards
    NUCLEAR_FALLOUT, // Radiation storms, mutants, irradiated zones
    WARZONE,        // Active combat zones, airstrikes, military presence
    STORM,          // Weather hazards (sandstorms, blizzards)
    PLAGUE,         // Disease outbreaks, contaminated areas
    DEMONIC_CORRUPTION // Diablo-style demonic invasion, Lilith's influence
};

// Core world configuration structure
struct WorldSettings {
    Era era = Era::MODERN;
    Hazard hazard = Hazard::NONE;
    bool cityLiving = true;
    bool enableTimeShift = false;  // Allow dynamic era switching mid-game
    float hazardIntensity = 1.0f;  // 0.0 = no effect, 1.0 = maximum intensity
    
    // Additional settings for fine-tuning
    bool enableHostileNPCs = true;
    bool enableVehicles = true;
    bool enableWeapons = true;
    std::string customTheme = "";  // Custom theme override
    
    // Get a human-readable description of current settings
    std::string getDescription() const;
    
    // Check if settings are valid
    bool isValid() const;
};

// NPC that adapts to different eras
struct NPC {
    glm::vec3 position;
    std::string faction;
    Era era;
    bool hostile;
    bool isHost;  // For Westworld era - indicates if this is a host/robot
    float health;
    std::string weapon;
    std::string dialogue;
    
    // Era-specific properties
    std::string getEraAppearance() const;
    std::string getEraBehavior() const;
    bool shouldSpawnInEra(Era targetEra) const;
};

// Time-Turner Director System
class TimeTurner {
private:
    WorldSettings settings;
    std::vector<NPC> npcs;
    bool worldNeedsRebuild = false;
    
    // Internal methods
    void rebuildWorld();
    void applyHazard();
    void rebuildCity();
    void spawnEraAppropriateNPCs();
    void updateNPCsForEra();
    
public:
    TimeTurner();
    
    // Core toggle methods
    void toggleEra(Era newEra);
    void toggleHazard(Hazard newHazard);
    void toggleCity(bool enable);
    void toggleTimeShift(bool enable);
    void setHazardIntensity(float intensity);
    
    // Get current state
    WorldSettings current() const { return settings; }
    const std::vector<NPC>& getNPCs() const { return npcs; }
    
    // Advanced controls
    void setCustomTheme(const std::string& theme);
    void randomizeSettings();
    void resetToDefaults();
    
    // Update loop
    void update(float deltaTime);
    
    // Preset configurations for common scenarios
    void applyPreset(const std::string& presetName);
    std::vector<std::string> getAvailablePresets() const;
    
    // Validation
    bool canTransitionTo(Era targetEra) const;
    bool canApplyHazard(Hazard targetHazard) const;
};

// Preset configurations for quick setup
namespace WorldPresets {
    const std::string FALLOUT_WASTELAND = "fallout_wasteland";
    const std::string COD_WARZONE = "cod_warzone";
    const std::string RDR_FRONTIER = "rdr_frontier";
    const std::string WESTWORLD_SIMULATION = "westworld_simulation";
    const std::string GTA_CHAOS = "gta_chaos";
    const std::string COWBOY_BR_NUCLEAR = "cowboy_br_nuclear";
    const std::string WESTWORLD_CASINO = "westworld_casino";
}