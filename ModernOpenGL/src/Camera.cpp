#include "Camera.h"
#include <algorithm>
#include <iostream>

Camera::Camera() 
    : m_position(0.0f, 0.0f, 0.0f)
    , m_target(0.0f, 0.0f, -1.0f)
    , m_forward(0.0f, 0.0f, -1.0f)
    , m_right(1.0f, 0.0f, 0.0f)
    , m_up(0.0f, 1.0f, 0.0f)
    , m_mode(CameraMode::FIRST_PERSON)
    , m_fov(70.0f)
    , m_aspectRatio(16.0f / 9.0f)
    , m_nearPlane(0.1f)
    , m_farPlane(1000.0f)
    , m_movementSpeed(5.0f)
    , m_mouseSensitivity(0.1f)
    , m_scrollSensitivity(2.0f)
    , m_lastMouseX(0.0)
    , m_lastMouseY(0.0)
    , m_firstMouse(true)
    , m_yaw(-90.0f)
    , m_pitch(0.0f)
    , m_thirdPersonDistance(5.0f)
    , m_thirdPersonHeight(2.0f)
    , m_thirdPersonOffset(0.0f, 0.0f, 0.0f)
    , m_firstPersonHeight(1.6f)
    , m_firstPersonOffset(0.0f, 0.0f, 0.0f)
    , m_toggleHeld(false)
{
    for (int i = 0; i < 1024; i++) {
        m_keys[i] = false;
    }
    
    UpdateVectors();
    UpdateProjection();
}

Camera::Camera(const glm::vec3& position, const glm::vec3& target) 
    : Camera()
{
    m_position = position;
    m_target = target;
    UpdateVectors();
}

Camera::~Camera() {
}

void Camera::SetPosition(const glm::vec3& position) {
    m_position = position;
    UpdateVectors();
}

void Camera::SetTarget(const glm::vec3& target) {
    m_target = target;
    UpdateVectors();
}

void Camera::SetMode(CameraMode mode) {
    m_mode = mode;
}

void Camera::SetFOV(float fov) {
    m_fov = fov;
    UpdateProjection();
}

void Camera::SetAspectRatio(float aspect) {
    m_aspectRatio = aspect;
    UpdateProjection();
}

void Camera::Update(float deltaTime) {
    // Handle camera movement based on current mode
    // This would integrate with your input system
}

void Camera::HandleKeyInput(int key, int action) {
    if (action == GLFW_PRESS) {
        m_keys[key] = true;
        
        // Handle camera mode toggle
        if (key == 86 && !m_toggleHeld) { // V key
            m_mode = (m_mode == CameraMode::FIRST_PERSON) 
                ? CameraMode::THIRD_PERSON 
                : CameraMode::FIRST_PERSON;
            m_toggleHeld = true;
            std::cout << "Camera mode: " << (m_mode == CameraMode::FIRST_PERSON ? "First Person" : "Third Person") << std::endl;
        }
    } else if (action == GLFW_RELEASE) {
        m_keys[key] = false;
        
        if (key == 86) { // V key
            m_toggleHeld = false;
        }
    }
}

void Camera::HandleMouseInput(double xpos, double ypos) {
    if (m_firstMouse) {
        m_lastMouseX = xpos;
        m_lastMouseY = ypos;
        m_firstMouse = false;
    }
    
    double xoffset = xpos - m_lastMouseX;
    double yoffset = m_lastMouseY - ypos; // Reversed since y-coordinates go from bottom to top
    
    m_lastMouseX = xpos;
    m_lastMouseY = ypos;
    
    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;
    
    m_yaw += static_cast<float>(xoffset);
    m_pitch += static_cast<float>(yoffset);
    
    // Constrain pitch
    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
    
    UpdateVectors();
}

void Camera::HandleScrollInput(double xoffset, double yoffset) {
    m_fov -= static_cast<float>(yoffset) * m_scrollSensitivity;
    m_fov = std::clamp(m_fov, 1.0f, 120.0f);
    UpdateProjection();
}

void Camera::UpdateFirstPerson(const glm::vec3& playerPos, const glm::quat& playerRot) {
    // Position camera at player's eye level
    glm::vec3 eyeOffset = m_firstPersonOffset + glm::vec3(0.0f, m_firstPersonHeight, 0.0f);
    m_position = playerPos + (playerRot * eyeOffset);
    
    // Target is in the direction the player is facing
    m_target = m_position + (playerRot * glm::vec3(0.0f, 0.0f, -1.0f));
    
    UpdateVectors();
}

void Camera::UpdateThirdPerson(const glm::vec3& playerPos, const glm::quat& playerRot) {
    // Position camera behind and above the player
    glm::vec3 offset = m_thirdPersonOffset + glm::vec3(0.0f, m_thirdPersonHeight, -m_thirdPersonDistance);
    m_position = playerPos + (playerRot * offset);
    
    // Target is the player's center
    m_target = playerPos + glm::vec3(0.0f, 1.0f, 0.0f);
    
    UpdateVectors();
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(m_position, m_target, m_up);
}

glm::mat4 Camera::GetProjectionMatrix() const {
    return glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearPlane, m_farPlane);
}

glm::mat4 Camera::GetViewProjectionMatrix() const {
    return GetProjectionMatrix() * GetViewMatrix();
}

void Camera::UpdateVectors() {
    // Calculate the new forward vector
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_forward = glm::normalize(front);
    
    // Re-calculate the right and up vector
    m_right = glm::normalize(glm::cross(m_forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    m_up = glm::normalize(glm::cross(m_right, m_forward));
}

void Camera::UpdateProjection() {
    // Projection matrix is updated when FOV or aspect ratio changes
    // This is handled in GetProjectionMatrix()
}