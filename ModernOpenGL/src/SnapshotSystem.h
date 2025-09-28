#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <deque>
#include <chrono>

// Snapshot data structure
struct Snapshot {
    double timestamp;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 velocity;
    int entityId;
    uint8_t flags; // alive, shooting, jumping, etc.
    
    Snapshot() : timestamp(0.0), position(0), rotation(1, 0, 0, 0), velocity(0), entityId(0), flags(0) {}
    Snapshot(double t, const glm::vec3& pos, const glm::quat& rot, const glm::vec3& vel, int id, uint8_t f)
        : timestamp(t), position(pos), rotation(rot), velocity(vel), entityId(id), flags(f) {}
};

// Snapshot buffer for interpolation
class SnapshotBuffer {
public:
    SnapshotBuffer();
    ~SnapshotBuffer();
    
    // Buffer management
    void AddSnapshot(const Snapshot& snapshot);
    void Clear();
    size_t Size() const { return m_snapshots.size(); }
    
    // Interpolation
    Snapshot Interpolate(double renderTime, double interpolationDelay = 0.1) const;
    Snapshot Extrapolate(double renderTime, double maxExtrapolation = 0.05) const;
    
    // Validation
    bool HasValidSnapshots() const;
    double GetLatestTimestamp() const;
    double GetOldestTimestamp() const;
    
    // Settings
    void SetMaxSnapshots(size_t max) { m_maxSnapshots = max; }
    void SetInterpolationDelay(double delay) { m_interpolationDelay = delay; }
    
private:
    std::deque<Snapshot> m_snapshots;
    size_t m_maxSnapshots;
    double m_interpolationDelay;
    
    // Helper functions
    std::pair<const Snapshot*, const Snapshot*> FindBoundingSnapshots(double renderTime) const;
    Snapshot LerpSnapshots(const Snapshot& a, const Snapshot& b, double t) const;
    void TrimSnapshots();
};

// Network prediction system
class NetworkPrediction {
public:
    NetworkPrediction();
    ~NetworkPrediction();
    
    // Input buffering
    void AddInput(const Snapshot& input);
    void ReconcileWithSnapshot(const Snapshot& serverSnapshot);
    
    // Prediction
    Snapshot Predict(double renderTime) const;
    void UpdatePrediction(double deltaTime);
    
    // Settings
    void SetPredictionEnabled(bool enabled) { m_predictionEnabled = enabled; }
    void SetReconciliationEnabled(bool enabled) { m_reconciliationEnabled = enabled; }
    void SetMaxInputBuffer(size_t max) { m_maxInputBuffer = max; }
    
    // Getters
    bool IsPredictionEnabled() const { return m_predictionEnabled; }
    bool IsReconciliationEnabled() const { return m_reconciliationEnabled; }
    size_t GetInputBufferSize() const { return m_inputBuffer.size(); }
    
private:
    std::deque<Snapshot> m_inputBuffer;
    Snapshot m_lastServerSnapshot;
    Snapshot m_predictedState;
    
    bool m_predictionEnabled;
    bool m_reconciliationEnabled;
    size_t m_maxInputBuffer;
    
    // Helper functions
    void TrimInputBuffer();
    Snapshot LerpInputs(const Snapshot& a, const Snapshot& b, double t) const;
};

// Ping compensation system
class PingCompensation {
public:
    PingCompensation();
    ~PingCompensation();
    
    // Ping measurement
    void StartPingMeasurement();
    void EndPingMeasurement();
    double GetPing() const { return m_ping; }
    double GetRTT() const { return m_rtt; }
    
    // Compensation
    double GetCompensatedTime() const;
    double GetRewindTime() const;
    
    // Settings
    void SetPing(double ping) { m_ping = ping; }
    void SetRTT(double rtt) { m_rtt = rtt; }
    void SetCompensationEnabled(bool enabled) { m_compensationEnabled = enabled; }
    
private:
    double m_ping;
    double m_rtt;
    std::chrono::high_resolution_clock::time_point m_pingStart;
    bool m_compensationEnabled;
};

// Muzzle correction system
class MuzzleCorrection {
public:
    MuzzleCorrection();
    ~MuzzleCorrection();
    
    // Muzzle offset
    void SetMuzzleOffset(const glm::vec3& offset) { m_muzzleOffset = offset; }
    glm::vec3 GetMuzzleOffset() const { return m_muzzleOffset; }
    
    // Correction calculation
    glm::vec3 CalculateMuzzlePosition(const glm::vec3& playerPos, const glm::quat& playerRot) const;
    glm::vec3 CalculateMuzzleDirection(const glm::quat& playerRot, const glm::vec3& aimDirection) const;
    
    // Settings
    void SetCorrectionEnabled(bool enabled) { m_correctionEnabled = enabled; }
    bool IsCorrectionEnabled() const { return m_correctionEnabled; }
    
private:
    glm::vec3 m_muzzleOffset;
    bool m_correctionEnabled;
};