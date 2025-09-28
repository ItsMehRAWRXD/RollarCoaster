#include "MapGenerator.h"
#include <algorithm>
#include <sstream>
#include <iostream>

// Static member initialization
std::mt19937 MapGenerator::s_rng;
bool MapGenerator::s_rngInitialized = false;

MapGenerator::MapGenerator() {
    if (!s_rngInitialized) {
        s_rng.seed(std::random_device{}());
        s_rngInitialized = true;
    }
}

MapGenerator::~MapGenerator() {
}

Map MapGenerator::Generate(int seed, int width, int height, int numRooms) {
    Map map;
    map.seed = seed;
    map.bounds = glm::vec3(width, 10.0f, height);
    map.name = "Generated_" + std::to_string(seed);
    
    // Initialize RNG with seed
    s_rng.seed(seed);
    
    // Generate rooms
    GenerateRooms(map, numRooms, width, height);
    
    // Generate corridors between rooms
    GenerateCorridors(map, width, height);
    
    // Generate outer walls
    GenerateWalls(map, width, height);
    
    // Generate spawn points
    GenerateSpawns(map, numRooms * 2);
    
    std::cout << "Generated map with " << map.walls.size() << " walls and " 
              << map.spawns.size() << " spawns" << std::endl;
    
    return map;
}

Map MapGenerator::GenerateArena(int seed, int size) {
    Map map;
    map.seed = seed;
    map.bounds = glm::vec3(size, 10.0f, size);
    map.name = "Arena_" + std::to_string(seed);
    
    s_rng.seed(seed);
    
    // Create arena walls
    int wallCount = 8 + GetRandomInt(0, 4);
    for (int i = 0; i < wallCount; i++) {
        Wall wall;
        wall.position = GetRandomPosition(size, size);
        wall.size = glm::vec3(
            GetRandomFloat(2.0f, 8.0f),
            GetRandomFloat(3.0f, 6.0f),
            GetRandomFloat(0.5f, 2.0f)
        );
        wall.rotation = glm::vec3(0, GetRandomFloat(0, 360), 0);
        wall.type = 0; // Wall
        
        if (!IsWallOverlapping(map, wall)) {
            map.walls.push_back(wall);
        }
    }
    
    // Add spawn points around the edges
    for (int i = 0; i < 8; i++) {
        SpawnPoint spawn;
        float angle = (i * 45.0f) * 3.14159f / 180.0f;
        spawn.position = glm::vec3(
            cos(angle) * (size * 0.4f),
            1.0f,
            sin(angle) * (size * 0.4f)
        );
        spawn.rotation = glm::vec3(0, -angle * 180.0f / 3.14159f, 0);
        spawn.team = i % 2; // Alternate teams
        
        map.spawns.push_back(spawn);
    }
    
    return map;
}

Map MapGenerator::GenerateCorridor(int seed, int length, int width) {
    Map map;
    map.seed = seed;
    map.bounds = glm::vec3(length, 10.0f, width);
    map.name = "Corridor_" + std::to_string(seed);
    
    s_rng.seed(seed);
    
    // Create corridor walls
    int segments = length / 4;
    for (int i = 0; i < segments; i++) {
        // Left wall
        Wall leftWall;
        leftWall.position = glm::vec3(i * 4.0f, 2.5f, -width/2.0f);
        leftWall.size = glm::vec3(0.5f, 5.0f, 1.0f);
        leftWall.rotation = glm::vec3(0, 0, 0);
        leftWall.type = 0;
        map.walls.push_back(leftWall);
        
        // Right wall
        Wall rightWall;
        rightWall.position = glm::vec3(i * 4.0f, 2.5f, width/2.0f);
        rightWall.size = glm::vec3(0.5f, 5.0f, 1.0f);
        rightWall.rotation = glm::vec3(0, 0, 0);
        rightWall.type = 0;
        map.walls.push_back(rightWall);
    }
    
    // Add spawn points along the corridor
    for (int i = 0; i < length / 8; i++) {
        SpawnPoint spawn;
        spawn.position = glm::vec3(i * 8.0f, 1.0f, 0);
        spawn.rotation = glm::vec3(0, GetRandomFloat(0, 360), 0);
        spawn.team = i % 2;
        map.spawns.push_back(spawn);
    }
    
    return map;
}

Map MapGenerator::GenerateMaze(int seed, int size) {
    Map map;
    map.seed = seed;
    map.bounds = glm::vec3(size, 10.0f, size);
    map.name = "Maze_" + std::to_string(seed);
    
    s_rng.seed(seed);
    
    // Simple maze generation using recursive backtracking
    std::vector<std::vector<bool>> grid(size, std::vector<bool>(size, true));
    
    // Generate maze paths
    std::function<void(int, int)> carve = [&](int x, int y) {
        grid[x][y] = false;
        
        std::vector<std::pair<int, int>> directions = {{0, 2}, {2, 0}, {0, -2}, {-2, 0}};
        std::shuffle(directions.begin(), directions.end(), s_rng);
        
        for (auto& dir : directions) {
            int nx = x + dir.first;
            int ny = y + dir.second;
            
            if (nx >= 0 && nx < size && ny >= 0 && ny < size && grid[nx][ny]) {
                grid[x + dir.first/2][y + dir.second/2] = false;
                carve(nx, ny);
            }
        }
    };
    
    carve(1, 1);
    
    // Convert grid to walls
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            if (grid[x][y]) {
                Wall wall;
                wall.position = glm::vec3(x * 2.0f, 2.5f, y * 2.0f);
                wall.size = glm::vec3(2.0f, 5.0f, 2.0f);
                wall.rotation = glm::vec3(0, 0, 0);
                wall.type = 0;
                map.walls.push_back(wall);
            }
        }
    }
    
    // Add spawn points at maze corners
    map.spawns.push_back({glm::vec3(2, 1, 2), glm::vec3(0, 0, 0), 0});
    map.spawns.push_back({glm::vec3(size*2-2, 1, size*2-2), glm::vec3(0, 180, 0), 1});
    
    return map;
}

bool MapGenerator::ValidateMap(const Map& map) {
    if (map.walls.empty()) return false;
    if (map.spawns.empty()) return false;
    if (map.bounds.x <= 0 || map.bounds.z <= 0) return false;
    
    return true;
}

bool MapGenerator::IsPointInBounds(const Map& map, const glm::vec3& point) {
    return point.x >= -map.bounds.x/2 && point.x <= map.bounds.x/2 &&
           point.z >= -map.bounds.z/2 && point.z <= map.bounds.z/2 &&
           point.y >= 0 && point.y <= map.bounds.y;
}

std::vector<glm::vec3> MapGenerator::GetSpawnPositions(const Map& map, int team) {
    std::vector<glm::vec3> positions;
    for (const auto& spawn : map.spawns) {
        if (spawn.team == team || team == 0) {
            positions.push_back(spawn.position);
        }
    }
    return positions;
}

std::vector<Wall> MapGenerator::GetWallsInRadius(const Map& map, const glm::vec3& center, float radius) {
    std::vector<Wall> nearbyWalls;
    for (const auto& wall : map.walls) {
        float distance = glm::length(wall.position - center);
        if (distance <= radius) {
            nearbyWalls.push_back(wall);
        }
    }
    return nearbyWalls;
}

std::string MapGenerator::SerializeMap(const Map& map) {
    std::ostringstream oss;
    oss << map.seed << " " << map.bounds.x << " " << map.bounds.y << " " << map.bounds.z << " ";
    oss << map.walls.size() << " ";
    for (const auto& wall : map.walls) {
        oss << wall.position.x << " " << wall.position.y << " " << wall.position.z << " ";
        oss << wall.size.x << " " << wall.size.y << " " << wall.size.z << " ";
        oss << wall.rotation.x << " " << wall.rotation.y << " " << wall.rotation.z << " ";
        oss << wall.type << " ";
    }
    oss << map.spawns.size() << " ";
    for (const auto& spawn : map.spawns) {
        oss << spawn.position.x << " " << spawn.position.y << " " << spawn.position.z << " ";
        oss << spawn.rotation.x << " " << spawn.rotation.y << " " << spawn.rotation.z << " ";
        oss << spawn.team << " ";
    }
    return oss.str();
}

Map MapGenerator::DeserializeMap(const std::string& data) {
    Map map;
    std::istringstream iss(data);
    
    iss >> map.seed >> map.bounds.x >> map.bounds.y >> map.bounds.z;
    
    size_t wallCount;
    iss >> wallCount;
    map.walls.resize(wallCount);
    for (auto& wall : map.walls) {
        iss >> wall.position.x >> wall.position.y >> wall.position.z;
        iss >> wall.size.x >> wall.size.y >> wall.size.z;
        iss >> wall.rotation.x >> wall.rotation.y >> wall.rotation.z;
        iss >> wall.type;
    }
    
    size_t spawnCount;
    iss >> spawnCount;
    map.spawns.resize(spawnCount);
    for (auto& spawn : map.spawns) {
        iss >> spawn.position.x >> spawn.position.y >> spawn.position.z;
        iss >> spawn.rotation.x >> spawn.rotation.y >> spawn.rotation.z;
        iss >> spawn.team;
    }
    
    return map;
}

int MapGenerator::GetWallCount(const Map& map) {
    return static_cast<int>(map.walls.size());
}

int MapGenerator::GetSpawnCount(const Map& map) {
    return static_cast<int>(map.spawns.size());
}

float MapGenerator::GetMapArea(const Map& map) {
    return map.bounds.x * map.bounds.z;
}

void MapGenerator::GenerateRooms(Map& map, int numRooms, int width, int height) {
    for (int i = 0; i < numRooms; i++) {
        Wall room;
        room.position = GetRandomPosition(width, height);
        room.size = glm::vec3(
            GetRandomFloat(4.0f, 12.0f),
            GetRandomFloat(3.0f, 6.0f),
            GetRandomFloat(4.0f, 12.0f)
        );
        room.rotation = glm::vec3(0, GetRandomFloat(0, 360), 0);
        room.type = 0; // Wall
        
        if (!IsWallOverlapping(map, room)) {
            map.walls.push_back(room);
        }
    }
}

void MapGenerator::GenerateCorridors(Map& map, int width, int height) {
    // Simple corridor generation between rooms
    for (size_t i = 0; i < map.walls.size() - 1; i++) {
        if (GetRandomFloat(0, 1) < 0.3f) { // 30% chance of corridor
            Wall corridor;
            corridor.position = (map.walls[i].position + map.walls[i+1].position) * 0.5f;
            corridor.size = glm::vec3(2.0f, 3.0f, 1.0f);
            corridor.rotation = glm::vec3(0, 0, 0);
            corridor.type = 0;
            
            if (!IsWallOverlapping(map, corridor)) {
                map.walls.push_back(corridor);
            }
        }
    }
}

void MapGenerator::GenerateWalls(Map& map, int width, int height) {
    // Outer boundary walls
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    
    // North wall
    map.walls.push_back({glm::vec3(0, 2.5f, -halfHeight), glm::vec3(width, 5.0f, 1.0f), glm::vec3(0, 0, 0), 0});
    // South wall
    map.walls.push_back({glm::vec3(0, 2.5f, halfHeight), glm::vec3(width, 5.0f, 1.0f), glm::vec3(0, 0, 0), 0});
    // East wall
    map.walls.push_back({glm::vec3(halfWidth, 2.5f, 0), glm::vec3(1.0f, 5.0f, height), glm::vec3(0, 0, 0), 0});
    // West wall
    map.walls.push_back({glm::vec3(-halfWidth, 2.5f, 0), glm::vec3(1.0f, 5.0f, height), glm::vec3(0, 0, 0), 0});
}

void MapGenerator::GenerateSpawns(Map& map, int numSpawns) {
    for (int i = 0; i < numSpawns; i++) {
        SpawnPoint spawn;
        spawn.position = GetRandomPosition(static_cast<int>(map.bounds.x), static_cast<int>(map.bounds.z));
        spawn.position.y = 1.0f;
        spawn.rotation = glm::vec3(0, GetRandomFloat(0, 360), 0);
        spawn.team = i % 2;
        
        if (IsSpawnValid(map, spawn)) {
            map.spawns.push_back(spawn);
        }
    }
}

bool MapGenerator::IsWallOverlapping(const Map& map, const Wall& wall) {
    for (const auto& existingWall : map.walls) {
        float distance = glm::length(wall.position - existingWall.position);
        if (distance < 3.0f) { // Minimum distance between walls
            return true;
        }
    }
    return false;
}

bool MapGenerator::IsSpawnValid(const Map& map, const SpawnPoint& spawn) {
    // Check if spawn is in bounds
    if (!IsPointInBounds(map, spawn.position)) return false;
    
    // Check if spawn is too close to walls
    for (const auto& wall : map.walls) {
        float distance = glm::length(spawn.position - wall.position);
        if (distance < 2.0f) return false;
    }
    
    return true;
}

glm::vec3 MapGenerator::GetRandomPosition(int width, int height) {
    return glm::vec3(
        GetRandomFloat(-width/2.0f, width/2.0f),
        0,
        GetRandomFloat(-height/2.0f, height/2.0f)
    );
}

int MapGenerator::GetRandomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(s_rng);
}

float MapGenerator::GetRandomFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(s_rng);
}