#pragma once
#include "TimeTurner.h"
#include <vector>
#include <string>
#include <map>

// Biome types for different eras
enum class BiomeType {
    URBAN_CITY,        // Modern skyscrapers, streets, urban sprawl
    URBAN_RUINS,       // Destroyed city, rubble, abandoned buildings
    WASTELAND_OPEN,    // Open irradiated wasteland
    WASTELAND_URBAN,   // Irradiated city ruins
    FRONTIER_TOWN,     // Western town with saloons, general stores
    FRONTIER_WILDERNESS, // Open plains, mountains, forests
    WESTWORLD_PARK,    // Mixed-era theme park
    WESTWORLD_CASINO,  // Westworld casino complex
    WESTWORLD_MAZE     // Westworld maze/underground
};

// Structure for map generation parameters
struct MapGenerationParams {
    WorldSettings worldSettings;
    int mapSize = 1000;  // Map size in units
    int detailLevel = 5; // Detail level (1-10)
    float biomeBlend = 0.3f; // How much biomes blend together
    bool enableProcedural = true;
    std::string seed = ""; // Random seed for generation
};

// Structure for generated map data
struct GeneratedMap {
    std::vector<BiomeType> biomes;
    std::vector<glm::vec3> buildingPositions;
    std::vector<glm::vec3> roadPositions;
    std::vector<glm::vec3> hazardZones;
    std::vector<glm::vec3> safeZones;
    std::vector<glm::vec3> spawnPoints;
    std::map<std::string, std::vector<glm::vec3>> specialLocations;
    
    // Era-specific locations
    std::vector<glm::vec3> saloons;      // Frontier
    std::vector<glm::vec3> bunkers;      // Wasteland
    std::vector<glm::vec3> skyscrapers;  // Modern
    std::vector<glm::vec3> hostStations; // Westworld
};

// Era-specific map generator
class EraMapGenerator {
private:
    MapGenerationParams currentParams;
    GeneratedMap currentMap;
    
    // Biome generation methods
    void generateModernBiome();
    void generateWastelandBiome();
    void generateFrontierBiome();
    void generateWestworldBiome();
    
    // Hazard application
    void applyNuclearFallout();
    void applyWarzone();
    void applyStorm();
    void applyPlague();
    
    // Utility methods
    glm::vec3 getRandomPosition();
    bool isPositionValid(const glm::vec3& pos);
    BiomeType getBiomeAtPosition(const glm::vec3& pos);
    void generateRoads();
    void generateBuildings();
    void generateSpecialLocations();
    
public:
    EraMapGenerator();
    
    // Main generation method
    GeneratedMap generateMap(const MapGenerationParams& params);
    
    // Era-specific generation
    GeneratedMap generateModernCity();
    GeneratedMap generateWasteland();
    GeneratedMap generateFrontier();
    GeneratedMap generateWestworld();
    
    // Hazard overlays
    void applyHazardOverlay(GeneratedMap& map, Hazard hazard, float intensity);
    
    // Utility methods
    std::vector<glm::vec3> getSpawnPoints(const GeneratedMap& map);
    std::vector<glm::vec3> getHazardZones(const GeneratedMap& map);
    std::vector<glm::vec3> getSafeZones(const GeneratedMap& map);
    
    // Validation
    bool validateMap(const GeneratedMap& map);
    
    // Preset generation
    GeneratedMap generatePresetMap(const std::string& presetName);
    
    // Get current map
    const GeneratedMap& getCurrentMap() const { return currentMap; }
};

// Specialized generators for different scenarios
class FalloutMapGenerator : public EraMapGenerator {
public:
    GeneratedMap generateFalloutWasteland();
    GeneratedMap generateIrradiatedCity();
    void addRadiationZones(GeneratedMap& map, float intensity);
};

class FrontierMapGenerator : public EraMapGenerator {
public:
    GeneratedMap generateCowboyTown();
    GeneratedMap generateWilderness();
    void addFrontierLocations(GeneratedMap& map);
};

class WestworldMapGenerator : public EraMapGenerator {
public:
    GeneratedMap generateThemePark();
    GeneratedMap generateCasinoComplex();
    GeneratedMap generateMaze();
    void addHostStations(GeneratedMap& map);
    void addEraTransitionZones(GeneratedMap& map);
};

class WarzoneMapGenerator : public EraMapGenerator {
public:
    GeneratedMap generateUrbanWarzone();
    GeneratedMap generateMilitaryBase();
    void addCombatZones(GeneratedMap& map, float intensity);
    void addAirstrikeZones(GeneratedMap& map);
};