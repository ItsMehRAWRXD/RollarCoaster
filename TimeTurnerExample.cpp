#include "TimeTurner.h"
#include "EraMapGenerator.h"
#include "TimeTurnerUI.h"
#include <iostream>
#include <thread>
#include <chrono>

// Example usage of the Time-Turner system
class TimeTurnerExample {
private:
    TimeTurner timeTurner;
    EraMapGenerator mapGenerator;
    TimeTurnerUI ui;
    TimeTurnerConsole console;
    
public:
    TimeTurnerExample() : ui(&timeTurner), console(&timeTurner) {
        setupCallbacks();
    }
    
    void setupCallbacks() {
        // Set up UI callbacks
        ui.setOnEraChanged([this](Era era) {
            std::cout << "Era changed to: " << getEraName(era) << std::endl;
            regenerateMap();
        });
        
        ui.setOnHazardChanged([this](Hazard hazard) {
            std::cout << "Hazard changed to: " << getHazardName(hazard) << std::endl;
            regenerateMap();
        });
        
        ui.setOnCityToggled([this](bool enabled) {
            std::cout << "City living: " << (enabled ? "ON" : "OFF") << std::endl;
            regenerateMap();
        });
        
        ui.setOnPresetSelected([this](const std::string& preset) {
            std::cout << "Preset selected: " << preset << std::endl;
            timeTurner.applyPreset(preset);
            regenerateMap();
        });
    }
    
    void runExample() {
        std::cout << "=== TIME-TURNER SYSTEM EXAMPLE ===" << std::endl;
        std::cout << "This demonstrates the time-turner meta-system for dynamic era switching." << std::endl;
        std::cout << std::endl;
        
        // Show initial state
        showCurrentState();
        
        // Demonstrate era switching
        demonstrateEraSwitching();
        
        // Demonstrate hazard overlays
        demonstrateHazardOverlays();
        
        // Demonstrate presets
        demonstratePresets();
        
        // Demonstrate UI
        demonstrateUI();
        
        // Demonstrate console
        demonstrateConsole();
        
        // Demonstrate map generation
        demonstrateMapGeneration();
    }
    
    void showCurrentState() {
        std::cout << "=== CURRENT STATE ===" << std::endl;
        WorldSettings current = timeTurner.current();
        std::cout << "Current Settings: " << current.getDescription() << std::endl;
        std::cout << "NPCs: " << timeTurner.getNPCs().size() << std::endl;
        std::cout << std::endl;
    }
    
    void demonstrateEraSwitching() {
        std::cout << "=== ERA SWITCHING DEMONSTRATION ===" << std::endl;
        
        // Switch to different eras
        std::vector<Era> eras = {Era::MODERN, Era::WASTELAND, Era::FRONTIER, Era::WESTWORLD};
        
        for (Era era : eras) {
            std::cout << "Switching to " << getEraName(era) << " era..." << std::endl;
            timeTurner.toggleEra(era);
            showCurrentState();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        
        std::cout << std::endl;
    }
    
    void demonstrateHazardOverlays() {
        std::cout << "=== HAZARD OVERLAYS DEMONSTRATION ===" << std::endl;
        
        // Set to modern era first
        timeTurner.toggleEra(Era::MODERN);
        
        // Apply different hazards
        std::vector<Hazard> hazards = {Hazard::NONE, Hazard::NUCLEAR_FALLOUT, Hazard::WARZONE, Hazard::STORM, Hazard::PLAGUE};
        
        for (Hazard hazard : hazards) {
            std::cout << "Applying " << getHazardName(hazard) << " hazard..." << std::endl;
            timeTurner.toggleHazard(hazard);
            timeTurner.setHazardIntensity(0.8f);
            showCurrentState();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        
        std::cout << std::endl;
    }
    
    void demonstratePresets() {
        std::cout << "=== PRESETS DEMONSTRATION ===" << std::endl;
        
        std::vector<std::string> presets = {
            "fallout_wasteland",
            "cod_warzone", 
            "rdr_frontier",
            "westworld_simulation",
            "gta_chaos",
            "cowboy_br_nuclear",
            "westworld_casino"
        };
        
        for (const std::string& preset : presets) {
            std::cout << "Applying preset: " << preset << std::endl;
            timeTurner.applyPreset(preset);
            showCurrentState();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        
        std::cout << std::endl;
    }
    
    void demonstrateUI() {
        std::cout << "=== UI DEMONSTRATION ===" << std::endl;
        
        // Show UI
        ui.setVisible(true);
        ui.setExpanded(true);
        ui.render();
        
        // Demonstrate UI interactions
        std::cout << "Simulating UI interactions..." << std::endl;
        
        // Change era via UI
        ui.handleEraSelection("Wasteland");
        ui.handleHazardSelection("Nuclear");
        ui.handleCityToggle(false);
        ui.handleHazardIntensityChange(0.9f);
        
        showCurrentState();
        
        std::cout << std::endl;
    }
    
    void demonstrateConsole() {
        std::cout << "=== CONSOLE DEMONSTRATION ===" << std::endl;
        
        // Show console
        console.setVisible(true);
        console.render();
        
        // Demonstrate console commands
        std::cout << "Simulating console commands..." << std::endl;
        
        console.handleInput("era frontier");
        console.handleInput("hazard none");
        console.handleInput("city off");
        console.handleInput("intensity 0.5");
        console.handleInput("status");
        
        std::cout << std::endl;
    }
    
    void demonstrateMapGeneration() {
        std::cout << "=== MAP GENERATION DEMONSTRATION ===" << std::endl;
        
        // Generate maps for different scenarios
        std::vector<std::string> scenarios = {
            "fallout_wasteland",
            "cod_warzone",
            "rdr_frontier", 
            "westworld_simulation"
        };
        
        for (const std::string& scenario : scenarios) {
            std::cout << "Generating map for: " << scenario << std::endl;
            
            // Apply preset
            timeTurner.applyPreset(scenario);
            
            // Generate map
            MapGenerationParams params;
            params.worldSettings = timeTurner.current();
            params.mapSize = 1000;
            params.detailLevel = 5;
            
            GeneratedMap map = mapGenerator.generateMap(params);
            
            // Show map info
            std::cout << "  Biomes: " << map.biomes.size() << std::endl;
            std::cout << "  Buildings: " << map.buildingPositions.size() << std::endl;
            std::cout << "  Roads: " << map.roadPositions.size() << std::endl;
            std::cout << "  Hazard Zones: " << map.hazardZones.size() << std::endl;
            std::cout << "  Spawn Points: " << map.spawnPoints.size() << std::endl;
            
            // Show era-specific locations
            if (timeTurner.current().era == Era::FRONTIER) {
                std::cout << "  Saloons: " << map.saloons.size() << std::endl;
            }
            if (timeTurner.current().era == Era::WASTELAND) {
                std::cout << "  Bunkers: " << map.bunkers.size() << std::endl;
            }
            if (timeTurner.current().era == Era::MODERN) {
                std::cout << "  Skyscrapers: " << map.skyscrapers.size() << std::endl;
            }
            if (timeTurner.current().era == Era::WESTWORLD) {
                std::cout << "  Host Stations: " << map.hostStations.size() << std::endl;
            }
            
            std::cout << std::endl;
        }
    }
    
    void regenerateMap() {
        // This would be called when settings change to regenerate the map
        MapGenerationParams params;
        params.worldSettings = timeTurner.current();
        params.mapSize = 1000;
        params.detailLevel = 5;
        
        GeneratedMap map = mapGenerator.generateMap(params);
        std::cout << "Map regenerated with " << map.biomes.size() << " biomes" << std::endl;
    }
    
    std::string getEraName(Era era) const {
        switch (era) {
            case Era::MODERN: return "Modern";
            case Era::WASTELAND: return "Wasteland";
            case Era::FRONTIER: return "Frontier";
            case Era::WESTWORLD: return "Westworld";
            default: return "Unknown";
        }
    }
    
    std::string getHazardName(Hazard hazard) const {
        switch (hazard) {
            case Hazard::NONE: return "None";
            case Hazard::NUCLEAR_FALLOUT: return "Nuclear Fallout";
            case Hazard::WARZONE: return "Warzone";
            case Hazard::STORM: return "Storm";
            case Hazard::PLAGUE: return "Plague";
            default: return "Unknown";
        }
    }
};

// Main function to run the example
int main() {
    TimeTurnerExample example;
    example.runExample();
    
    std::cout << "=== TIME-TURNER SYSTEM COMPLETE ===" << std::endl;
    std::cout << "The time-turner system is now ready for use!" << std::endl;
    std::cout << "You can dynamically switch between eras, apply hazards, and generate appropriate maps." << std::endl;
    
    return 0;
}