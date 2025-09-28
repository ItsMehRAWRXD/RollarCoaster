#include "SnapshotSystem.h"
#include <algorithm>
#include <iostream>

// SnapshotBuffer implementation
SnapshotBuffer::SnapshotBuffer() 
    : m_maxSnapshots(64)
    , m_interpolationDelay(0.1)
{
}

SnapshotBuffer::~SnapshotBuffer() {
    Clear();
}

void SnapshotBuffer::AddSnapshot(const Snapshot& snapshot) {
    m_snapshots.push_back(snapshot);
    TrimSnapshots();
}

void SnapshotBuffer::Clear() {
    m_snapshots.clear();
}

Snapshot SnapshotBuffer::Interpolate(double renderTime, double interpolationDelay) const {
    if (m_snapshots.empty()) {
        return Snapshot();
    }
    
    if (m_snapshots.size() == 1) {
        return m_snapshots.front();
    }
    
    double targetTime = renderTime - interpolationDelay;
    
    // Find bounding snapshots
    auto bounds = FindBoundingSnapshots(targetTime);
    if (!bounds.first || !bounds.second) {
        return m_snapshots.back();
    }
    
    const Snapshot& a = *bounds.first;
    const Snapshot& b = *bounds.second;
    
    if (a.timestamp == b.timestamp) {
        return a;
    }
    
    double t = (targetTime - a.timestamp) / (b.timestamp - a.timestamp);
    t = std::clamp(t, 0.0, 1.0);
    
    return LerpSnapshots(a, b, t);
}

Snapshot SnapshotBuffer::Extrapolate(double renderTime, double maxExtrapolation) const {
    if (m_snapshots.size() < 2) {
        return m_snapshots.empty() ? Snapshot() : m_snapshots.back();
    }
    
    const Snapshot& latest = m_snapshots.back();
    const Snapshot& previous = m_snapshots[m_snapshots.size() - 2];
    
    double deltaTime = renderTime - latest.timestamp;
    deltaTime = std::min(deltaTime, maxExtrapolation);
    
    // Extrapolate based on velocity
    Snapshot extrapolated = latest;
    extrapolated.position += latest.velocity * static_cast<float>(deltaTime);
    extrapolated.timestamp = renderTime;
    
    return extrapolated;
}

bool SnapshotBuffer::HasValidSnapshots() const {
    return !m_snapshots.empty();
}

double SnapshotBuffer::GetLatestTimestamp() const {
    return m_snapshots.empty() ? 0.0 : m_snapshots.back().timestamp;
}

double SnapshotBuffer::GetOldestTimestamp() const {
    return m_snapshots.empty() ? 0.0 : m_snapshots.front().timestamp;
}

std::pair<const Snapshot*, const Snapshot*> SnapshotBuffer::FindBoundingSnapshots(double renderTime) const {
    if (m_snapshots.size() < 2) {
        return {nullptr, nullptr};
    }
    
    for (size_t i = 0; i < m_snapshots.size() - 1; i++) {
        if (m_snapshots[i].timestamp <= renderTime && m_snapshots[i + 1].timestamp >= renderTime) {
            return {&m_snapshots[i], &m_snapshots[i + 1]};
        }
    }
    
    // If we're before the first snapshot, return first two
    if (renderTime < m_snapshots[0].timestamp) {
        return {&m_snapshots[0], &m_snapshots[1]};
    }
    
    // If we're after the last snapshot, return last two
    return {&m_snapshots[m_snapshots.size() - 2], &m_snapshots[m_snapshots.size() - 1]};
}

Snapshot SnapshotBuffer::LerpSnapshots(const Snapshot& a, const Snapshot& b, double t) const {
    Snapshot result;
    result.timestamp = a.timestamp + (b.timestamp - a.timestamp) * t;
    result.position = glm::mix(a.position, b.position, static_cast<float>(t));
    result.rotation = glm::slerp(a.rotation, b.rotation, static_cast<float>(t));
    result.velocity = glm::mix(a.velocity, b.velocity, static_cast<float>(t));
    result.entityId = a.entityId;
    result.flags = a.flags; // Use flags from first snapshot
    
    return result;
}

void SnapshotBuffer::TrimSnapshots() {
    while (m_snapshots.size() > m_maxSnapshots) {
        m_snapshots.pop_front();
    }
}

// NetworkPrediction implementation
NetworkPrediction::NetworkPrediction() 
    : m_predictionEnabled(true)
    , m_reconciliationEnabled(true)
    , m_maxInputBuffer(64)
{
}

NetworkPrediction::~NetworkPrediction() {
}

void NetworkPrediction::AddInput(const Snapshot& input) {
    m_inputBuffer.push_back(input);
    TrimInputBuffer();
}

void NetworkPrediction::ReconcileWithSnapshot(const Snapshot& serverSnapshot) {
    if (!m_reconciliationEnabled) return;
    
    m_lastServerSnapshot = serverSnapshot;
    
    // Find inputs that need to be reapplied
    double serverTime = serverSnapshot.timestamp;
    auto it = std::find_if(m_inputBuffer.begin(), m_inputBuffer.end(),
        [serverTime](const Snapshot& input) { return input.timestamp > serverTime; });
    
    if (it != m_inputBuffer.end()) {
        // Reapply inputs from this point forward
        std::cout << "Reconciling " << std::distance(it, m_inputBuffer.end()) << " inputs" << std::endl;
    }
}

Snapshot NetworkPrediction::Predict(double renderTime) const {
    if (!m_predictionEnabled || m_inputBuffer.empty()) {
        return m_lastServerSnapshot;
    }
    
    // Find the most recent input
    const Snapshot& latestInput = m_inputBuffer.back();
    
    // Extrapolate from latest input
    double deltaTime = renderTime - latestInput.timestamp;
    Snapshot predicted = latestInput;
    predicted.position += latestInput.velocity * static_cast<float>(deltaTime);
    predicted.timestamp = renderTime;
    
    return predicted;
}

void NetworkPrediction::UpdatePrediction(double deltaTime) {
    if (m_inputBuffer.empty()) return;
    
    // Update predicted state based on latest input
    const Snapshot& latestInput = m_inputBuffer.back();
    m_predictedState = latestInput;
    m_predictedState.timestamp = latestInput.timestamp + deltaTime;
}

void NetworkPrediction::TrimInputBuffer() {
    while (m_inputBuffer.size() > m_maxInputBuffer) {
        m_inputBuffer.pop_front();
    }
}

Snapshot NetworkPrediction::LerpInputs(const Snapshot& a, const Snapshot& b, double t) const {
    Snapshot result;
    result.timestamp = a.timestamp + (b.timestamp - a.timestamp) * t;
    result.position = glm::mix(a.position, b.position, static_cast<float>(t));
    result.rotation = glm::slerp(a.rotation, b.rotation, static_cast<float>(t));
    result.velocity = glm::mix(a.velocity, b.velocity, static_cast<float>(t));
    result.entityId = a.entityId;
    result.flags = a.flags;
    
    return result;
}

// PingCompensation implementation
PingCompensation::PingCompensation() 
    : m_ping(0.0)
    , m_rtt(0.0)
    , m_compensationEnabled(true)
{
}

PingCompensation::~PingCompensation() {
}

void PingCompensation::StartPingMeasurement() {
    m_pingStart = std::chrono::high_resolution_clock::now();
}

void PingCompensation::EndPingMeasurement() {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_pingStart);
    m_rtt = duration.count();
    m_ping = m_rtt / 2.0; // Ping is half of RTT
}

double PingCompensation::GetCompensatedTime() const {
    if (!m_compensationEnabled) return 0.0;
    return m_ping / 1000.0; // Convert to seconds
}

double PingCompensation::GetRewindTime() const {
    if (!m_compensationEnabled) return 0.0;
    return m_ping / 1000.0; // Convert to seconds
}

// MuzzleCorrection implementation
MuzzleCorrection::MuzzleCorrection() 
    : m_muzzleOffset(0.0f, 0.0f, 0.0f)
    , m_correctionEnabled(true)
{
}

MuzzleCorrection::~MuzzleCorrection() {
}

glm::vec3 MuzzleCorrection::CalculateMuzzlePosition(const glm::vec3& playerPos, const glm::quat& playerRot) const {
    if (!m_correctionEnabled) return playerPos;
    
    return playerPos + (playerRot * m_muzzleOffset);
}

glm::vec3 MuzzleCorrection::CalculateMuzzleDirection(const glm::quat& playerRot, const glm::vec3& aimDirection) const {
    if (!m_correctionEnabled) return aimDirection;
    
    return playerRot * aimDirection;
}