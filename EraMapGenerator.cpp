#include "EraMapGenerator.h"
#include <random>
#include <algorithm>
#include <cmath>

EraMapGenerator::EraMapGenerator() {
    currentParams = MapGenerationParams();
}

GeneratedMap EraMapGenerator::generateMap(const MapGenerationParams& params) {
    currentParams = params;
    currentMap = GeneratedMap();
    
    // Generate base biome based on era
    switch (params.worldSettings.era) {
        case Era::MODERN:
            generateModernBiome();
            break;
        case Era::WASTELAND:
            generateWastelandBiome();
            break;
        case Era::FRONTIER:
            generateFrontierBiome();
            break;
        case Era::WESTWORLD:
            generateWestworldBiome();
            break;
    }
    
    // Apply city/rural settings
    if (params.worldSettings.cityLiving) {
        generateBuildings();
        generateRoads();
    }
    
    // Apply hazard overlays
    if (params.worldSettings.hazard != Hazard::NONE) {
        applyHazardOverlay(currentMap, params.worldSettings.hazard, params.worldSettings.hazardIntensity);
    }
    
    // Generate special locations
    generateSpecialLocations();
    
    return currentMap;
}

void EraMapGenerator::generateModernBiome() {
    // Generate modern city layout
    int mapSize = currentParams.mapSize;
    int gridSize = mapSize / 10; // 10x10 grid
    
    for (int x = 0; x < gridSize; x++) {
        for (int z = 0; z < gridSize; z++) {
            glm::vec3 pos(x * 100.0f, 0.0f, z * 100.0f);
            
            if (currentParams.worldSettings.cityLiving) {
                currentMap.biomes.push_back(BiomeType::URBAN_CITY);
                currentMap.buildingPositions.push_back(pos);
                
                // Add skyscrapers in city center
                if (x > gridSize/3 && x < 2*gridSize/3 && z > gridSize/3 && z < 2*gridSize/3) {
                    currentMap.skyscrapers.push_back(pos);
                }
            } else {
                currentMap.biomes.push_back(BiomeType::FRONTIER_WILDERNESS);
            }
        }
    }
    
    // Add spawn points
    currentMap.spawnPoints.push_back(glm::vec3(0, 0, 0));
    currentMap.spawnPoints.push_back(glm::vec3(mapSize/2, 0, 0));
    currentMap.spawnPoints.push_back(glm::vec3(0, 0, mapSize/2));
    currentMap.spawnPoints.push_back(glm::vec3(mapSize/2, 0, mapSize/2));
}

void EraMapGenerator::generateWastelandBiome() {
    int mapSize = currentParams.mapSize;
    int gridSize = mapSize / 10;
    
    for (int x = 0; x < gridSize; x++) {
        for (int z = 0; z < gridSize; z++) {
            glm::vec3 pos(x * 100.0f, 0.0f, z * 100.0f);
            
            if (currentParams.worldSettings.cityLiving) {
                currentMap.biomes.push_back(BiomeType::URBAN_RUINS);
                currentMap.buildingPositions.push_back(pos);
            } else {
                currentMap.biomes.push_back(BiomeType::WASTELAND_OPEN);
            }
        }
    }
    
    // Add bunkers and safe zones
    for (int i = 0; i < 5; i++) {
        glm::vec3 bunkerPos = getRandomPosition();
        currentMap.bunkers.push_back(bunkerPos);
        currentMap.safeZones.push_back(bunkerPos);
    }
    
    // Add spawn points
    currentMap.spawnPoints.push_back(glm::vec3(0, 0, 0));
    currentMap.spawnPoints.push_back(glm::vec3(mapSize/3, 0, mapSize/3));
    currentMap.spawnPoints.push_back(glm::vec3(2*mapSize/3, 0, 2*mapSize/3));
}

void EraMapGenerator::generateFrontierBiome() {
    int mapSize = currentParams.mapSize;
    int gridSize = mapSize / 10;
    
    for (int x = 0; x < gridSize; x++) {
        for (int z = 0; z < gridSize; z++) {
            glm::vec3 pos(x * 100.0f, 0.0f, z * 100.0f);
            
            if (currentParams.worldSettings.cityLiving) {
                currentMap.biomes.push_back(BiomeType::FRONTIER_TOWN);
                currentMap.buildingPositions.push_back(pos);
                
                // Add saloons in town center
                if (x > gridSize/3 && x < 2*gridSize/3 && z > gridSize/3 && z < 2*gridSize/3) {
                    currentMap.saloons.push_back(pos);
                }
            } else {
                currentMap.biomes.push_back(BiomeType::FRONTIER_WILDERNESS);
            }
        }
    }
    
    // Add spawn points
    currentMap.spawnPoints.push_back(glm::vec3(0, 0, 0));
    currentMap.spawnPoints.push_back(glm::vec3(mapSize/4, 0, mapSize/4));
    currentMap.spawnPoints.push_back(glm::vec3(3*mapSize/4, 0, 3*mapSize/4));
}

void EraMapGenerator::generateWestworldBiome() {
    int mapSize = currentParams.mapSize;
    int gridSize = mapSize / 10;
    
    for (int x = 0; x < gridSize; x++) {
        for (int z = 0; z < gridSize; z++) {
            glm::vec3 pos(x * 100.0f, 0.0f, z * 100.0f);
            
            // Westworld can have mixed biomes
            if (currentParams.worldSettings.customTheme == "casino") {
                currentMap.biomes.push_back(BiomeType::WESTWORLD_CASINO);
            } else {
                currentMap.biomes.push_back(BiomeType::WESTWORLD_PARK);
            }
            
            currentMap.buildingPositions.push_back(pos);
            
            // Add host stations
            if (x % 3 == 0 && z % 3 == 0) {
                currentMap.hostStations.push_back(pos);
            }
        }
    }
    
    // Add spawn points
    currentMap.spawnPoints.push_back(glm::vec3(0, 0, 0));
    currentMap.spawnPoints.push_back(glm::vec3(mapSize/2, 0, 0));
    currentMap.spawnPoints.push_back(glm::vec3(0, 0, mapSize/2));
    currentMap.spawnPoints.push_back(glm::vec3(mapSize/2, 0, mapSize/2));
}

void EraMapGenerator::applyHazardOverlay(GeneratedMap& map, Hazard hazard, float intensity) {
    switch (hazard) {
        case Hazard::NUCLEAR_FALLOUT:
            applyNuclearFallout();
            break;
        case Hazard::WARZONE:
            applyWarzone();
            break;
        case Hazard::STORM:
            applyStorm();
            break;
        case Hazard::PLAGUE:
            applyPlague();
            break;
        default:
            break;
    }
}

void EraMapGenerator::applyNuclearFallout() {
    // Add radiation zones
    int numZones = static_cast<int>(5 * currentParams.worldSettings.hazardIntensity);
    
    for (int i = 0; i < numZones; i++) {
        glm::vec3 zonePos = getRandomPosition();
        currentMap.hazardZones.push_back(zonePos);
        
        // Add radiation effect radius
        for (int j = 0; j < 10; j++) {
            glm::vec3 offset(
                (rand() % 200 - 100) * currentParams.worldSettings.hazardIntensity,
                0.0f,
                (rand() % 200 - 100) * currentParams.worldSettings.hazardIntensity
            );
            currentMap.hazardZones.push_back(zonePos + offset);
        }
    }
}

void EraMapGenerator::applyWarzone() {
    // Add combat zones
    int numZones = static_cast<int>(3 * currentParams.worldSettings.hazardIntensity);
    
    for (int i = 0; i < numZones; i++) {
        glm::vec3 zonePos = getRandomPosition();
        currentMap.hazardZones.push_back(zonePos);
    }
    
    // Add airstrike zones
    for (int i = 0; i < numZones; i++) {
        glm::vec3 zonePos = getRandomPosition();
        currentMap.hazardZones.push_back(zonePos);
    }
}

void EraMapGenerator::applyStorm() {
    // Add storm zones
    int numZones = static_cast<int>(4 * currentParams.worldSettings.hazardIntensity);
    
    for (int i = 0; i < numZones; i++) {
        glm::vec3 zonePos = getRandomPosition();
        currentMap.hazardZones.push_back(zonePos);
    }
}

void EraMapGenerator::applyPlague() {
    // Add contaminated zones
    int numZones = static_cast<int>(6 * currentParams.worldSettings.hazardIntensity);
    
    for (int i = 0; i < numZones; i++) {
        glm::vec3 zonePos = getRandomPosition();
        currentMap.hazardZones.push_back(zonePos);
    }
}

glm::vec3 EraMapGenerator::getRandomPosition() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, static_cast<float>(currentParams.mapSize));
    
    return glm::vec3(dist(gen), 0.0f, dist(gen));
}

bool EraMapGenerator::isPositionValid(const glm::vec3& pos) {
    // Check if position is within map bounds
    return pos.x >= 0 && pos.x < currentParams.mapSize && 
           pos.z >= 0 && pos.z < currentParams.mapSize;
}

BiomeType EraMapGenerator::getBiomeAtPosition(const glm::vec3& pos) {
    // Simple grid-based biome lookup
    int gridX = static_cast<int>(pos.x / 100.0f);
    int gridZ = static_cast<int>(pos.z / 100.0f);
    int index = gridX * (currentParams.mapSize / 100) + gridZ;
    
    if (index >= 0 && index < currentMap.biomes.size()) {
        return currentMap.biomes[index];
    }
    
    return BiomeType::FRONTIER_WILDERNESS; // Default
}

void EraMapGenerator::generateRoads() {
    if (!currentParams.worldSettings.cityLiving) return;
    
    int mapSize = currentParams.mapSize;
    int roadSpacing = 200; // Roads every 200 units
    
    // Generate horizontal roads
    for (int z = 0; z < mapSize; z += roadSpacing) {
        for (int x = 0; x < mapSize; x += 10) {
            currentMap.roadPositions.push_back(glm::vec3(x, 0, z));
        }
    }
    
    // Generate vertical roads
    for (int x = 0; x < mapSize; x += roadSpacing) {
        for (int z = 0; z < mapSize; z += 10) {
            currentMap.roadPositions.push_back(glm::vec3(x, 0, z));
        }
    }
}

void EraMapGenerator::generateBuildings() {
    if (!currentParams.worldSettings.cityLiving) return;
    
    int mapSize = currentParams.mapSize;
    int buildingSpacing = 150; // Buildings every 150 units
    
    for (int x = 0; x < mapSize; x += buildingSpacing) {
        for (int z = 0; z < mapSize; z += buildingSpacing) {
            // Skip road intersections
            if (x % 200 == 0 || z % 200 == 0) continue;
            
            currentMap.buildingPositions.push_back(glm::vec3(x, 0, z));
        }
    }
}

void EraMapGenerator::generateSpecialLocations() {
    // Add era-specific special locations
    switch (currentParams.worldSettings.era) {
        case Era::MODERN:
            // Add police stations, hospitals, etc.
            break;
        case Era::WASTELAND:
            // Add bunkers, supply caches, etc.
            break;
        case Era::FRONTIER:
            // Add saloons, general stores, etc.
            break;
        case Era::WESTWORLD:
            // Add host stations, control rooms, etc.
            break;
    }
}

std::vector<glm::vec3> EraMapGenerator::getSpawnPoints(const GeneratedMap& map) {
    return map.spawnPoints;
}

std::vector<glm::vec3> EraMapGenerator::getHazardZones(const GeneratedMap& map) {
    return map.hazardZones;
}

std::vector<glm::vec3> EraMapGenerator::getSafeZones(const GeneratedMap& map) {
    return map.safeZones;
}

bool EraMapGenerator::validateMap(const GeneratedMap& map) {
    // Check if map has required elements
    if (map.spawnPoints.empty()) return false;
    if (map.biomes.empty()) return false;
    
    return true;
}

GeneratedMap EraMapGenerator::generatePresetMap(const std::string& presetName) {
    MapGenerationParams params;
    
    if (presetName == "fallout_wasteland") {
        params.worldSettings.era = Era::WASTELAND;
        params.worldSettings.hazard = Hazard::NUCLEAR_FALLOUT;
        params.worldSettings.cityLiving = false;
        params.worldSettings.hazardIntensity = 0.8f;
    }
    else if (presetName == "cod_warzone") {
        params.worldSettings.era = Era::MODERN;
        params.worldSettings.hazard = Hazard::WARZONE;
        params.worldSettings.cityLiving = true;
        params.worldSettings.hazardIntensity = 1.0f;
    }
    else if (presetName == "rdr_frontier") {
        params.worldSettings.era = Era::FRONTIER;
        params.worldSettings.hazard = Hazard::NONE;
        params.worldSettings.cityLiving = false;
    }
    else if (presetName == "westworld_simulation") {
        params.worldSettings.era = Era::WESTWORLD;
        params.worldSettings.hazard = Hazard::NONE;
        params.worldSettings.cityLiving = true;
    }
    
    return generateMap(params);
}