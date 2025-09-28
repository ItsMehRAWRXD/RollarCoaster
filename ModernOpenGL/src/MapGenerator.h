#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <random>
#include <memory>

// Map generation structures
struct Wall {
    glm::vec3 position;
    glm::vec3 size;
    glm::vec3 rotation;
    int type; // 0=wall, 1=floor, 2=ceiling
};

struct SpawnPoint {
    glm::vec3 position;
    glm::vec3 rotation;
    int team; // 0=neutral, 1=team1, 2=team2
};

struct Map {
    std::vector<Wall> walls;
    std::vector<SpawnPoint> spawns;
    glm::vec3 bounds; // Map dimensions
    int seed;
    std::string name;
};

// Procedural map generator for XQZ wireframe layouts
class MapGenerator {
public:
    MapGenerator();
    ~MapGenerator();

    // Map generation
    static Map Generate(int seed, int width, int height, int numRooms);
    static Map GenerateArena(int seed, int size);
    static Map GenerateCorridor(int seed, int length, int width);
    static Map GenerateMaze(int seed, int size);
    
    // Map validation
    static bool ValidateMap(const Map& map);
    static bool IsPointInBounds(const Map& map, const glm::vec3& point);
    
    // Map utilities
    static std::vector<glm::vec3> GetSpawnPositions(const Map& map, int team = 0);
    static std::vector<Wall> GetWallsInRadius(const Map& map, const glm::vec3& center, float radius);
    
    // Map serialization
    static std::string SerializeMap(const Map& map);
    static Map DeserializeMap(const std::string& data);
    
    // Map statistics
    static int GetWallCount(const Map& map);
    static int GetSpawnCount(const Map& map);
    static float GetMapArea(const Map& map);

private:
    // Generation algorithms
    static void GenerateRooms(Map& map, int numRooms, int width, int height);
    static void GenerateCorridors(Map& map, int width, int height);
    static void GenerateWalls(Map& map, int width, int height);
    static void GenerateSpawns(Map& map, int numSpawns);
    
    // Utility functions
    static bool IsWallOverlapping(const Map& map, const Wall& wall);
    static bool IsSpawnValid(const Map& map, const SpawnPoint& spawn);
    static glm::vec3 GetRandomPosition(int width, int height);
    static int GetRandomInt(int min, int max);
    static float GetRandomFloat(float min, float max);
    
    // Random number generation
    static std::mt19937 s_rng;
    static bool s_rngInitialized;
};