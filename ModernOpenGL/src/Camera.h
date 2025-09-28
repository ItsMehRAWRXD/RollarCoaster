#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

// Camera modes
enum class CameraMode {
    FIRST_PERSON,
    THIRD_PERSON
};

// Modern camera system with first/third person toggle
class Camera {
public:
    Camera();
    Camera(const glm::vec3& position, const glm::vec3& target);
    ~Camera();

    // Camera control
    void SetPosition(const glm::vec3& position);
    void SetTarget(const glm::vec3& target);
    void SetMode(CameraMode mode);
    void SetFOV(float fov);
    void SetAspectRatio(float aspect);
    
    // Camera movement
    void Update(float deltaTime);
    void HandleKeyInput(int key, int action);
    void HandleMouseInput(double xpos, double ypos);
    void HandleScrollInput(double xoffset, double yoffset);
    
    // Camera positioning for different modes
    void UpdateFirstPerson(const glm::vec3& playerPos, const glm::quat& playerRot);
    void UpdateThirdPerson(const glm::vec3& playerPos, const glm::quat& playerRot);
    
    // Matrix generation
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetViewProjectionMatrix() const;
    
    // Getters
    glm::vec3 GetPosition() const { return m_position; }
    glm::vec3 GetTarget() const { return m_target; }
    glm::vec3 GetForward() const { return m_forward; }
    glm::vec3 GetRight() const { return m_right; }
    glm::vec3 GetUp() const { return m_up; }
    CameraMode GetMode() const { return m_mode; }
    float GetFOV() const { return m_fov; }
    float GetAspectRatio() const { return m_aspectRatio; }
    
    // Camera settings
    void SetMovementSpeed(float speed) { m_movementSpeed = speed; }
    void SetMouseSensitivity(float sensitivity) { m_mouseSensitivity = sensitivity; }
    void SetScrollSensitivity(float sensitivity) { m_scrollSensitivity = sensitivity; }
    
    // Third-person settings
    void SetThirdPersonDistance(float distance) { m_thirdPersonDistance = distance; }
    void SetThirdPersonHeight(float height) { m_thirdPersonHeight = height; }
    void SetThirdPersonOffset(const glm::vec3& offset) { m_thirdPersonOffset = offset; }
    
    // First-person settings
    void SetFirstPersonHeight(float height) { m_firstPersonHeight = height; }
    void SetFirstPersonOffset(const glm::vec3& offset) { m_firstPersonOffset = offset; }

private:
    void UpdateVectors();
    void UpdateProjection();
    
    // Camera state
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_forward;
    glm::vec3 m_right;
    glm::vec3 m_up;
    
    // Camera mode
    CameraMode m_mode;
    
    // Projection
    float m_fov;
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;
    
    // Movement
    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_scrollSensitivity;
    
    // Mouse state
    double m_lastMouseX;
    double m_lastMouseY;
    bool m_firstMouse;
    
    // Yaw and pitch
    float m_yaw;
    float m_pitch;
    
    // Third-person settings
    float m_thirdPersonDistance;
    float m_thirdPersonHeight;
    glm::vec3 m_thirdPersonOffset;
    
    // First-person settings
    float m_firstPersonHeight;
    glm::vec3 m_firstPersonOffset;
    
    // Input state
    bool m_keys[1024];
    bool m_toggleHeld;
};