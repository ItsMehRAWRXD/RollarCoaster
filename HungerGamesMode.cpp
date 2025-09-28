#include "HungerGamesMode.h"
#include <algorithm>
#include <cmath>
#include <random>

// Utility functions
float randf(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

float rand01() {
    return randf(0.0f, 1.0f);
}

glm::vec3 randomInRing(const glm::vec3& center, float minRadius, float maxRadius) {
    float angle = randf(0.0f, 2.0f * M_PI);
    float radius = randf(minRadius, maxRadius);
    return center + glm::vec3(radius * cos(angle), 0.0f, radius * sin(angle));
}

// Tribute implementation
void Tribute::updateSurvivalTime(float deltaTime) {
    if (alive) {
        survivalTime += deltaTime;
    }
}

void Tribute::addKill() {
    kills++;
    sponsorCredits += 10; // Earn credits for kills
}

void Tribute::addStylePoints(int points) {
    stylePoints += points;
    sponsorCredits += points; // Style points convert to credits
}

void Tribute::addSponsorCredits(int credits) {
    sponsorCredits += credits;
}

int Tribute::getTotalSponsorCredits() const {
    return sponsorCredits + (stylePoints / 2) + (multikills * 5) + audienceVotes;
}

// SponsorGift implementation
void SponsorGift::updateGift(float deltaTime) {
    if (!isDelivered) {
        dropTime -= deltaTime;
    }
}

bool SponsorGift::isExpired() const {
    return dropTime <= 0.0f && !isDelivered;
}

// ArenaEvent implementation
void ArenaEvent::updateEvent(float deltaTime) {
    if (isActive) {
        duration -= deltaTime;
        if (duration <= 0.0f) {
            endEvent();
        }
    }
}

bool ArenaEvent::isExpired() const {
    return duration <= 0.0f;
}

void ArenaEvent::triggerEvent() {
    isActive = true;
}

void ArenaEvent::endEvent() {
    isActive = false;
}

// HungerGamesMode implementation
HungerGamesMode::HungerGamesMode(const HGConfig& cfg) 
    : config(cfg), rng(std::random_device{}()), randomFloat(0.0f, 1.0f), randomInt(0, 100) {
    graceTimeLeft = config.gracePeriod;
}

HungerGamesMode::~HungerGamesMode() {
    // Cleanup
}

void HungerGamesMode::initializeGame(const WorldSettings& settings) {
    worldSettings = settings;
    tributes.clear();
    liveEvents.clear();
    pendingGifts.clear();
    gameTime = 0.0f;
    graceTimeLeft = config.gracePeriod;
    gameActive = false;
    gameOver = false;
    
    // Initialize systems
    zoneManager.initializeZone(ZoneType::CIRCLE_SHRINK, glm::vec3(0, 0, 0), config.scatterRadius);
    hazardManager.initializeHazards(worldSettings);
    lootManager.initializeLoot(worldSettings);
}

void HungerGamesMode::startGame() {
    gameActive = true;
    gameOver = false;
    gameTime = 0.0f;
    graceTimeLeft = config.gracePeriod;
    
    // Scatter spawn all tributes
    scatterSpawn();
    
    // Spawn cornucopia
    spawnCornucopia();
    
    // Broadcast game start
    broadcastEvent(HGEvents::GAME_START, "Hunger Games begins!");
    
    if (onGameEvent) {
        onGameEvent("Hunger Games started with " + std::to_string(tributes.size()) + " tributes");
    }
}

void HungerGamesMode::updateGame(float deltaTime) {
    if (!gameActive || gameOver) return;
    
    gameTime += deltaTime;
    
    // Update grace period
    if (graceTimeLeft > 0.0f) {
        graceTimeLeft -= deltaTime;
        if (graceTimeLeft <= 0.0f) {
            broadcastEvent(HGEvents::GRACE_PERIOD_END, "Grace period ended - PvP enabled!");
        }
    }
    
    // Update tributes
    for (auto& tribute : tributes) {
        if (tribute.alive) {
            tribute.updateSurvivalTime(deltaTime);
        }
    }
    
    // Update zone and events
    applyRingAndEvents(deltaTime);
    
    // Roll for arena events
    rollArenaEvents(deltaTime);
    
    // Update sponsor gifts
    updateSponsorGifts(deltaTime);
    
    // Check win condition
    checkWinCondition();
}

void HungerGamesMode::endGame() {
    gameActive = false;
    gameOver = true;
    
    int winnerId = getWinnerId();
    if (winnerId != -1) {
        broadcastEvent(HGEvents::TRIBUTE_WIN, "Tribute " + std::to_string(winnerId) + " wins!");
        if (onTributeWin) {
            onTributeWin(winnerId, "Winner of Hunger Games");
        }
    }
    
    broadcastEvent(HGEvents::GAME_END, "Hunger Games ended");
    
    if (onGameEvent) {
        onGameEvent("Hunger Games ended");
    }
}

void HungerGamesMode::scatterSpawn() {
    tributes.clear();
    tributes.reserve(config.playerCount);
    
    for (int i = 0; i < config.playerCount; i++) {
        float angle = randf(0.0f, 2.0f * M_PI);
        float radius = randf(0.3f * config.scatterRadius, config.scatterRadius);
        glm::vec3 position = glm::vec3(radius * cos(angle), 0.0f, radius * sin(angle));
        
        // Snap to ground (assuming terrain height function exists)
        // position.y = sampleTerrainHeight(position.x, position.z) + 1.6f;
        position.y = 1.6f; // Placeholder for ground level
        
        Tribute tribute;
        tribute.id = i;
        tribute.position = position;
        tribute.alive = true;
        tribute.district = i % 12; // Assign districts 0-11
        tribute.kills = 0;
        tribute.sponsorCredits = 0;
        tribute.survivalTime = 0.0f;
        tribute.name = "Tribute " + std::to_string(i);
        tribute.inventory = {"fists"}; // Start with fists only
        tribute.inGracePeriod = true;
        tribute.stylePoints = 0;
        tribute.multikills = 0;
        tribute.audienceVotes = 0;
        
        tributes.push_back(tribute);
        
        // Server spawn player (placeholder)
        // serverSpawnPlayer(i, position);
    }
}

void HungerGamesMode::spawnCornucopia() {
    // Spawn high-tier loot at center
    lootManager.spawnLoot(glm::vec3(0, 0, 0), 5); // Mythic tier loot
    
    // Add trap ring around cornucopia
    hazardManager.spawnHazard("trap_ring", glm::vec3(0, 0, 0));
    
    broadcastEvent(HGEvents::CORNUCOPIA_OPEN, "Cornucopia is open - rich loot awaits!");
}

void HungerGamesMode::rollArenaEvents(float deltaTime) {
    if (!config.enableArenaEvents) return;
    
    // FireWall event
    if (rand01() < eventWeights.fireWall * deltaTime) {
        triggerFireWall(zoneManager.getCenter(), zoneManager.getCurrentRadius() * 0.9f);
    }
    
    // BeastWave event
    if (rand01() < eventWeights.beastWave * deltaTime) {
        glm::vec3 center = randomInRing(zoneManager.getCenter(), 
                                      0.2f * zoneManager.getCurrentRadius(), 
                                      zoneManager.getCurrentRadius());
        triggerBeastWave(center, 100.0f);
    }
    
    // MazeShift event
    if (rand01() < eventWeights.mazeShift * deltaTime && config.enableMazeShift) {
        triggerMazeShift(zoneManager.getCenter(), zoneManager.getCurrentRadius());
    }
    
    // AcidRain event
    if (rand01() < eventWeights.acidRain * deltaTime) {
        glm::vec3 center = randomInRing(zoneManager.getCenter(), 
                                      0.1f * zoneManager.getCurrentRadius(), 
                                      0.8f * zoneManager.getCurrentRadius());
        triggerAcidRain(center, 80.0f);
    }
    
    // CometShower event
    if (rand01() < eventWeights.cometShower * deltaTime) {
        glm::vec3 center = randomInRing(zoneManager.getCenter(), 
                                      0.3f * zoneManager.getCurrentRadius(), 
                                      zoneManager.getCurrentRadius());
        triggerCometShower(center, 120.0f);
    }
}

void HungerGamesMode::applyRingAndEvents(float deltaTime) {
    // Update zone
    zoneManager.updateZone(deltaTime);
    
    // Apply events to tributes
    for (auto& event : liveEvents) {
        if (event.isActive) {
            // Apply event effects to tributes
            for (auto& tribute : tributes) {
                if (tribute.alive) {
                    float distance = glm::length(tribute.position - event.center);
                    if (distance <= event.radius) {
                        // Apply event damage/effects
                        if (event.type == "fireWall") {
                            // Fire damage
                            handleTributeDeath(tribute.id, "FireWall");
                        } else if (event.type == "beastWave") {
                            // Beast attack
                            handleTributeDeath(tribute.id, "BeastWave");
                        } else if (event.type == "acidRain") {
                            // Acid damage
                            handleTributeDeath(tribute.id, "AcidRain");
                        } else if (event.type == "cometShower") {
                            // Comet impact
                            handleTributeDeath(tribute.id, "CometShower");
                        }
                    }
                }
            }
        }
    }
    
    // Remove expired events
    liveEvents.erase(
        std::remove_if(liveEvents.begin(), liveEvents.end(),
            [](const ArenaEvent& event) { return event.isExpired(); }),
        liveEvents.end()
    );
}

void HungerGamesMode::triggerFireWall(const glm::vec3& center, float radius) {
    ArenaEvent event;
    event.type = "fireWall";
    event.center = center;
    event.radius = radius;
    event.duration = 30.0f;
    event.startTime = gameTime;
    event.isActive = true;
    event.description = "A ring of fire sweeps across the arena";
    event.intensity = 1.0f;
    
    liveEvents.push_back(event);
    broadcastEvent(HGEvents::FIRE_WALL, "FireWall event triggered!");
}

void HungerGamesMode::triggerBeastWave(const glm::vec3& center, float radius) {
    ArenaEvent event;
    event.type = "beastWave";
    event.center = center;
    event.radius = radius;
    event.duration = 60.0f;
    event.startTime = gameTime;
    event.isActive = true;
    event.description = "A wave of beasts attacks the area";
    event.intensity = 1.0f;
    
    liveEvents.push_back(event);
    broadcastEvent(HGEvents::BEAST_WAVE, "BeastWave event triggered!");
}

void HungerGamesMode::triggerMazeShift(const glm::vec3& center, float radius) {
    ArenaEvent event;
    event.type = "mazeShift";
    event.center = center;
    event.radius = radius;
    event.duration = 12.0f;
    event.startTime = gameTime;
    event.isActive = true;
    event.description = "The arena reconfigures - walls shift and move";
    event.intensity = 1.0f;
    
    liveEvents.push_back(event);
    broadcastEvent(HGEvents::MAZE_SHIFT, "MazeShift event triggered!");
}

void HungerGamesMode::triggerAcidRain(const glm::vec3& center, float radius) {
    ArenaEvent event;
    event.type = "acidRain";
    event.center = center;
    event.radius = radius;
    event.duration = 45.0f;
    event.startTime = gameTime;
    event.isActive = true;
    event.description = "Acid rain falls in the area";
    event.intensity = 1.0f;
    
    liveEvents.push_back(event);
    broadcastEvent(HGEvents::ACID_RAIN, "AcidRain event triggered!");
}

void HungerGamesMode::triggerCometShower(const glm::vec3& center, float radius) {
    ArenaEvent event;
    event.type = "cometShower";
    event.center = center;
    event.radius = radius;
    event.duration = 30.0f;
    event.startTime = gameTime;
    event.isActive = true;
    event.description = "Comets rain down from the sky";
    event.intensity = 1.0f;
    
    liveEvents.push_back(event);
    broadcastEvent(HGEvents::COMET_SHOWER, "CometShower event triggered!");
}

void HungerGamesMode::checkWinCondition() {
    int aliveCount = 0;
    int lastAliveId = -1;
    
    for (const auto& tribute : tributes) {
        if (tribute.alive) {
            aliveCount++;
            lastAliveId = tribute.id;
        }
    }
    
    if (aliveCount <= 1) {
        endGame();
    }
}

int HungerGamesMode::getWinnerId() const {
    for (const auto& tribute : tributes) {
        if (tribute.alive) {
            return tribute.id;
        }
    }
    return -1;
}

std::vector<int> HungerGamesMode::getAliveTributes() const {
    std::vector<int> alive;
    for (const auto& tribute : tributes) {
        if (tribute.alive) {
            alive.push_back(tribute.id);
        }
    }
    return alive;
}

void HungerGamesMode::handleTributeDeath(int id, const std::string& cause) {
    for (auto& tribute : tributes) {
        if (tribute.id == id && tribute.alive) {
            tribute.alive = false;
            
            // Enable sponsor mode
            if (config.sponsorsEnabled) {
                enableSponsorMode(id);
            }
            
            broadcastEvent(HGEvents::TRIBUTE_DEATH, "Tribute " + std::to_string(id) + " died: " + cause);
            
            if (onTributeDeath) {
                onTributeDeath(id, cause);
            }
            break;
        }
    }
}

void HungerGamesMode::handleTributeKill(int killerId, int victimId) {
    for (auto& tribute : tributes) {
        if (tribute.id == killerId && tribute.alive) {
            tribute.addKill();
            tribute.addStylePoints(5); // Style points for kills
            break;
        }
    }
    
    handleTributeDeath(victimId, "Killed by Tribute " + std::to_string(killerId));
}

void HungerGamesMode::enableSponsorMode(int tributeId) {
    // Convert eliminated tribute to sponsor
    for (auto& tribute : tributes) {
        if (tribute.id == tributeId) {
            tribute.sponsorCredits += 50; // Starting sponsor credits
            break;
        }
    }
}

void HungerGamesMode::sendSponsorGift(int sponsorId, int targetTributeId, const std::string& giftType) {
    // Find target tribute position
    glm::vec3 targetPos = glm::vec3(0, 0, 0);
    for (const auto& tribute : tributes) {
        if (tribute.id == targetTributeId && tribute.alive) {
            targetPos = tribute.position;
            break;
        }
    }
    
    SponsorGift gift;
    gift.dropPosition = targetPos;
    gift.type = giftType;
    gift.senderId = sponsorId;
    gift.targetTributeId = targetTributeId;
    gift.dropTime = 10.0f; // 10 second drop time
    gift.isDelivered = false;
    gift.description = "Sponsor gift: " + giftType;
    gift.value = 10; // Sponsor credit cost
    
    pendingGifts.push_back(gift);
    
    broadcastEvent(HGEvents::SPONSOR_GIFT, "Sponsor " + std::to_string(sponsorId) + " sent gift to Tribute " + std::to_string(targetTributeId));
}

void HungerGamesMode::updateSponsorGifts(float deltaTime) {
    for (auto& gift : pendingGifts) {
        gift.updateGift(deltaTime);
        
        if (gift.isExpired() && !gift.isDelivered) {
            // Deliver gift to target tribute
            deliverGift(gift);
        }
    }
    
    // Remove delivered gifts
    pendingGifts.erase(
        std::remove_if(pendingGifts.begin(), pendingGifts.end(),
            [](const SponsorGift& gift) { return gift.isDelivered; }),
        pendingGifts.end()
    );
}

void HungerGamesMode::deliverGift(const SponsorGift& gift) {
    // Find target tribute and add item to inventory
    for (auto& tribute : tributes) {
        if (tribute.id == gift.targetTributeId && tribute.alive) {
            tribute.inventory.push_back(gift.type);
            break;
        }
    }
    
    if (onSponsorGift) {
        onSponsorGift(gift.senderId, gift.targetTributeId);
    }
}

void HungerGamesMode::broadcastEvent(const std::string& eventType, const std::string& data) {
    // Broadcast to all clients
    // This would integrate with your networking system
    if (onGameEvent) {
        onGameEvent(eventType + ": " + data);
    }
}

// Callback setters
void HungerGamesMode::setOnTributeDeath(std::function<void(int, const std::string&)> callback) {
    onTributeDeath = callback;
}

void HungerGamesMode::setOnTributeWin(std::function<void(int, const std::string&)> callback) {
    onTributeWin = callback;
}

void HungerGamesMode::setOnGameEvent(std::function<void(const std::string&)> callback) {
    onGameEvent = callback;
}

void HungerGamesMode::setOnSponsorGift(std::function<void(int, int)> callback) {
    onSponsorGift = callback;
}

// Getters
const HGConfig& HungerGamesMode::getConfig() const { return config; }
const std::vector<Tribute>& HungerGamesMode::getTributes() const { return tributes; }
const std::vector<ArenaEvent>& HungerGamesMode::getLiveEvents() const { return liveEvents; }
const std::vector<SponsorGift>& HungerGamesMode::getPendingGifts() const { return pendingGifts; }

bool HungerGamesMode::isGameActive() const { return gameActive; }
float HungerGamesMode::getGameTime() const { return gameTime; }
float HungerGamesMode::getGraceTimeLeft() const { return graceTimeLeft; }

int HungerGamesMode::getAliveCount() const {
    int count = 0;
    for (const auto& tribute : tributes) {
        if (tribute.alive) count++;
    }
    return count;
}

int HungerGamesMode::getTotalKills() const {
    int total = 0;
    for (const auto& tribute : tributes) {
        total += tribute.kills;
    }
    return total;
}

HungerGamesMode::GameStats HungerGamesMode::getGameStats() const {
    GameStats stats;
    stats.totalKills = getTotalKills();
    stats.totalDeaths = config.playerCount - getAliveCount();
    stats.averageSurvivalTime = 0.0f;
    stats.mostActiveEvent = "fireWall";
    stats.totalSponsorGifts = pendingGifts.size();
    
    // Calculate average survival time
    float totalTime = 0.0f;
    for (const auto& tribute : tributes) {
        totalTime += tribute.survivalTime;
    }
    if (tributes.size() > 0) {
        stats.averageSurvivalTime = totalTime / tributes.size();
    }
    
    return stats;
}