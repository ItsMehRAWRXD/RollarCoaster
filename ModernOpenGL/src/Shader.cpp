#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader() : m_program(0) {
}

Shader::~Shader() {
    if (m_program) {
        glDeleteProgram(m_program);
    }
}

bool Shader::Load(const std::string& vertexSource, const std::string& fragmentSource) {
    return Load(vertexSource, "", fragmentSource);
}

bool Shader::Load(const std::string& vertexSource, const std::string& geometrySource, const std::string& fragmentSource) {
    m_program = glCreateProgram();
    
    // Compile vertex shader
    GLuint vertexShader = CompileShader(vertexSource, GL_VERTEX_SHADER);
    if (!vertexShader) return false;
    glAttachShader(m_program, vertexShader);
    
    // Compile geometry shader (if provided)
    GLuint geometryShader = 0;
    if (!geometrySource.empty()) {
        geometryShader = CompileShader(geometrySource, GL_GEOMETRY_SHADER);
        if (!geometryShader) {
            glDeleteShader(vertexShader);
            return false;
        }
        glAttachShader(m_program, geometryShader);
    }
    
    // Compile fragment shader
    GLuint fragmentShader = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);
    if (!fragmentShader) {
        glDeleteShader(vertexShader);
        if (geometryShader) glDeleteShader(geometryShader);
        return false;
    }
    glAttachShader(m_program, fragmentShader);
    
    // Link program
    glLinkProgram(m_program);
    
    // Check for linking errors
    GLint success;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[1024];
        glGetProgramInfoLog(m_program, 1024, nullptr, infoLog);
        std::cerr << "Shader linking failed: " << infoLog << std::endl;
        
        glDeleteShader(vertexShader);
        if (geometryShader) glDeleteShader(geometryShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(m_program);
        m_program = 0;
        return false;
    }
    
    // Clean up shaders
    glDeleteShader(vertexShader);
    if (geometryShader) glDeleteShader(geometryShader);
    glDeleteShader(fragmentShader);
    
    return true;
}

bool Shader::LoadCompute(const std::string& computeSource) {
    m_program = glCreateProgram();
    
    GLuint computeShader = CompileShader(computeSource, GL_COMPUTE_SHADER);
    if (!computeShader) return false;
    
    glAttachShader(m_program, computeShader);
    glLinkProgram(m_program);
    
    GLint success;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[1024];
        glGetProgramInfoLog(m_program, 1024, nullptr, infoLog);
        std::cerr << "Compute shader linking failed: " << infoLog << std::endl;
        
        glDeleteShader(computeShader);
        glDeleteProgram(m_program);
        m_program = 0;
        return false;
    }
    
    glDeleteShader(computeShader);
    return true;
}

void Shader::Use() {
    glUseProgram(m_program);
}

void Shader::Unuse() {
    glUseProgram(0);
}

void Shader::SetBool(const std::string& name, bool value) {
    glUniform1i(GetUniformLocation(name), (int)value);
}

void Shader::SetInt(const std::string& name, int value) {
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetFloat(const std::string& name, float value) {
    glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetVec2(const std::string& name, const glm::vec2& value) {
    glUniform2fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) {
    glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetVec4(const std::string& name, const glm::vec4& value) {
    glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetMat2(const std::string& name, const glm::mat2& value) {
    glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetMat3(const std::string& name, const glm::mat3& value) {
    glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) {
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetUniformBlock(const std::string& name, GLuint binding) {
    GLuint blockIndex = glGetUniformBlockIndex(m_program, name.c_str());
    if (blockIndex != GL_INVALID_INDEX) {
        glUniformBlockBinding(m_program, blockIndex, binding);
    }
}

GLuint Shader::CompileShader(const std::string& source, GLenum type) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    CheckCompileErrors(shader, type == GL_VERTEX_SHADER ? "VERTEX" : 
                      type == GL_FRAGMENT_SHADER ? "FRAGMENT" : 
                      type == GL_GEOMETRY_SHADER ? "GEOMETRY" : 
                      type == GL_COMPUTE_SHADER ? "COMPUTE" : "UNKNOWN");
    
    return shader;
}

GLint Shader::GetUniformLocation(const std::string& name) {
    if (m_uniformLocations.find(name) != m_uniformLocations.end()) {
        return m_uniformLocations[name];
    }
    
    GLint location = glGetUniformLocation(m_program, name.c_str());
    m_uniformLocations[name] = location;
    return location;
}

void Shader::CheckCompileErrors(GLuint shader, const std::string& type) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cerr << "Shader compilation failed (" << type << "): " << infoLog << std::endl;
    }
}

// Modern shader source generators
std::string Shader::GetPBRVertexShader() {
    return R"(
#version 450 core
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inTangent;

layout(std140, binding = 0) uniform Camera {
    mat4 uView;
    mat4 uProj;
    mat4 uViewProj;
    vec4 uEyePos;
    vec4 uScreenSize;
};

out VS_OUT {
    vec3 worldPos;
    vec3 normal;
    vec2 uv;
    vec3 tangent;
    vec3 bitangent;
} vs_out;

uniform mat4 uModel;

void main() {
    vec4 worldPos = uModel * vec4(inPos, 1.0);
    vs_out.worldPos = worldPos.xyz;
    vs_out.normal = mat3(uModel) * inNormal;
    vs_out.uv = inUV;
    vs_out.tangent = mat3(uModel) * inTangent;
    vs_out.bitangent = cross(vs_out.normal, vs_out.tangent);
    
    gl_Position = uViewProj * worldPos;
}
)";
}

std::string Shader::GetPBRFragmentShader() {
    return R"(
#version 450 core
in VS_OUT {
    vec3 worldPos;
    vec3 normal;
    vec2 uv;
    vec3 tangent;
    vec3 bitangent;
} fs_in;

layout(location = 0) out vec4 FragColor;

layout(std140, binding = 0) uniform Camera {
    mat4 uView;
    mat4 uProj;
    mat4 uViewProj;
    vec4 uEyePos;
    vec4 uScreenSize;
};

// PBR Material
struct Material {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
    vec3 emissive;
};

uniform Material uMaterial;
uniform sampler2D uAlbedoMap;
uniform sampler2D uNormalMap;
uniform sampler2D uMetallicMap;
uniform sampler2D uRoughnessMap;
uniform sampler2D uAOMap;
uniform sampler2D uEmissiveMap;

// IBL
uniform samplerCube uIrradianceMap;
uniform samplerCube uPrefilterMap;
uniform sampler2D uBRDFLUT;

// Lights
struct Light {
    vec3 position;
    vec3 color;
    float intensity;
    float radius;
};

uniform Light uLights[32];
uniform int uLightCount;

const float PI = 3.14159265359;

// PBR Functions
vec3 F_Schlick(vec3 F0, float VoH) {
    return F0 + (1.0 - F0) * pow(1.0 - VoH, 5.0);
}

float D_GGX(float NoH, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NoH2 = NoH * NoH;
    float denom = NoH2 * (a2 - 1.0) + 1.0;
    return a2 / (PI * denom * denom);
}

float G_SchlickGGX(float NoV, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NoV / (NoV * (1.0 - k) + k);
}

float G_Smith(float NoV, float NoL, float roughness) {
    return G_SchlickGGX(NoV, roughness) * G_SchlickGGX(NoL, roughness);
}

vec3 getNormalFromMap() {
    vec3 tangentNormal = texture(uNormalMap, fs_in.uv).xyz * 2.0 - 1.0;
    
    vec3 N = normalize(fs_in.normal);
    vec3 T = normalize(fs_in.tangent);
    vec3 B = normalize(fs_in.bitangent);
    mat3 TBN = mat3(T, B, N);
    
    return normalize(TBN * tangentNormal);
}

void main() {
    // Sample textures
    vec3 albedo = texture(uAlbedoMap, fs_in.uv).rgb * uMaterial.albedo;
    float metallic = texture(uMetallicMap, fs_in.uv).r * uMaterial.metallic;
    float roughness = texture(uRoughnessMap, fs_in.uv).r * uMaterial.roughness;
    float ao = texture(uAOMap, fs_in.uv).r * uMaterial.ao;
    vec3 emissive = texture(uEmissiveMap, fs_in.uv).rgb * uMaterial.emissive;
    
    // Get normal
    vec3 N = getNormalFromMap();
    vec3 V = normalize(uEyePos.xyz - fs_in.worldPos);
    float NoV = max(dot(N, V), 0.0);
    
    // Calculate F0
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    
    // Direct lighting
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < uLightCount; i++) {
        vec3 L = normalize(uLights[i].position - fs_in.worldPos);
        vec3 H = normalize(V + L);
        float NoL = max(dot(N, L), 0.0);
        float NoH = max(dot(N, H), 0.0);
        float VoH = max(dot(V, H), 0.0);
        
        // Distance attenuation
        float distance = length(uLights[i].position - fs_in.worldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = uLights[i].color * uLights[i].intensity * attenuation;
        
        // BRDF
        vec3 F = F_Schlick(F0, VoH);
        float D = D_GGX(NoH, roughness);
        float G = G_Smith(NoV, NoL, roughness);
        
        vec3 specular = (D * G * F) / (4.0 * max(NoV, 0.001) * max(NoL, 0.001));
        vec3 kD = (1.0 - F) * (1.0 - metallic);
        vec3 diffuse = kD * albedo / PI;
        
        Lo += (diffuse + specular) * radiance * NoL;
    }
    
    // IBL
    vec3 R = reflect(-V, N);
    vec3 F = F_Schlick(F0, NoV);
    
    vec3 irradiance = texture(uIrradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;
    
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(uPrefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(uBRDFLUT, vec2(NoV, roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
    
    vec3 ambient = (diffuse + specular) * ao;
    vec3 color = ambient + Lo + emissive;
    
    FragColor = vec4(color, 1.0);
}
)";
}

std::string Shader::GetACESToneMapShader() {
    return R"(
#version 450 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D uHDRTexture;
uniform float uExposure;
uniform bool uUseACES;

vec3 ACESFilm(vec3 x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

vec3 ReinhardToneMap(vec3 color) {
    return color / (color + vec3(1.0));
}

void main() {
    vec3 hdrColor = texture(uHDRTexture, TexCoords).rgb;
    
    // Exposure
    hdrColor *= uExposure;
    
    // Tone mapping
    vec3 color;
    if (uUseACES) {
        color = ACESFilm(hdrColor);
    } else {
        color = ReinhardToneMap(hdrColor);
    }
    
    // Gamma correction
    color = pow(color, vec3(1.0 / 2.2));
    
    FragColor = vec4(color, 1.0);
}
)";
}

std::string Shader::GetGBufferVertexShader() {
    return R"(
#version 450 core
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inTangent;

layout(std140, binding = 0) uniform Camera {
    mat4 uView;
    mat4 uProj;
    mat4 uViewProj;
    vec4 uEyePos;
    vec4 uScreenSize;
};

out VS_OUT {
    vec3 worldPos;
    vec3 normal;
    vec2 uv;
    vec3 tangent;
} vs_out;

uniform mat4 uModel;

void main() {
    vec4 worldPos = uModel * vec4(inPos, 1.0);
    vs_out.worldPos = worldPos.xyz;
    vs_out.normal = mat3(uModel) * inNormal;
    vs_out.uv = inUV;
    vs_out.tangent = mat3(uModel) * inTangent;
    
    gl_Position = uViewProj * worldPos;
}
)";
}

std::string Shader::GetGBufferFragmentShader() {
    return R"(
#version 450 core
in VS_OUT {
    vec3 worldPos;
    vec3 normal;
    vec2 uv;
    vec3 tangent;
} fs_in;

layout(location = 0) out vec4 gAlbedo;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gMaterial;

uniform sampler2D uAlbedoMap;
uniform sampler2D uNormalMap;
uniform sampler2D uMetallicMap;
uniform sampler2D uRoughnessMap;
uniform sampler2D uAOMap;

uniform vec3 uAlbedo;
uniform float uMetallic;
uniform float uRoughness;
uniform float uAO;

vec3 getNormalFromMap() {
    vec3 tangentNormal = texture(uNormalMap, fs_in.uv).xyz * 2.0 - 1.0;
    
    vec3 N = normalize(fs_in.normal);
    vec3 T = normalize(fs_in.tangent);
    vec3 B = normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);
    
    return normalize(TBN * tangentNormal);
}

void main() {
    // Sample textures
    vec3 albedo = texture(uAlbedoMap, fs_in.uv).rgb * uAlbedo;
    float metallic = texture(uMetallicMap, fs_in.uv).r * uMetallic;
    float roughness = texture(uRoughnessMap, fs_in.uv).r * uRoughness;
    float ao = texture(uAOMap, fs_in.uv).r * uAO;
    
    // Store in G-buffer
    gAlbedo = vec4(albedo, 1.0);
    gNormal = vec4(normalize(getNormalFromMap()), 1.0);
    gMaterial = vec4(metallic, roughness, ao, 1.0);
}
)";
}

std::string Shader::GetLightingFragmentShader() {
    return R"(
#version 450 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D gAlbedo;
uniform sampler2D gNormal;
uniform sampler2D gMaterial;
uniform sampler2D gDepth;

uniform vec3 uEyePos;
uniform mat4 uInvViewProj;

struct Light {
    vec3 position;
    vec3 color;
    float intensity;
    float radius;
};

uniform Light uLights[32];
uniform int uLightCount;

const float PI = 3.14159265359;

vec3 F_Schlick(vec3 F0, float VoH) {
    return F0 + (1.0 - F0) * pow(1.0 - VoH, 5.0);
}

float D_GGX(float NoH, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NoH2 = NoH * NoH;
    float denom = NoH2 * (a2 - 1.0) + 1.0;
    return a2 / (PI * denom * denom);
}

float G_SchlickGGX(float NoV, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NoV / (NoV * (1.0 - k) + k);
}

float G_Smith(float NoV, float NoL, float roughness) {
    return G_SchlickGGX(NoV, roughness) * G_SchlickGGX(NoL, roughness);
}

vec3 getWorldPos(vec2 uv, float depth) {
    vec4 clipSpace = vec4(uv * 2.0 - 1.0, depth, 1.0);
    vec4 worldPos = uInvViewProj * clipSpace;
    return worldPos.xyz / worldPos.w;
}

void main() {
    // Sample G-buffer
    vec3 albedo = texture(gAlbedo, TexCoords).rgb;
    vec3 normal = texture(gNormal, TexCoords).rgb;
    vec3 material = texture(gMaterial, TexCoords).rgb;
    float depth = texture(gDepth, TexCoords).r;
    
    vec3 worldPos = getWorldPos(TexCoords, depth);
    vec3 V = normalize(uEyePos - worldPos);
    float NoV = max(dot(normal, V), 0.0);
    
    float metallic = material.r;
    float roughness = material.g;
    float ao = material.b;
    
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    
    // Direct lighting
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < uLightCount; i++) {
        vec3 L = normalize(uLights[i].position - worldPos);
        vec3 H = normalize(V + L);
        float NoL = max(dot(normal, L), 0.0);
        float NoH = max(dot(normal, H), 0.0);
        float VoH = max(dot(V, H), 0.0);
        
        // Distance attenuation
        float distance = length(uLights[i].position - worldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = uLights[i].color * uLights[i].intensity * attenuation;
        
        // BRDF
        vec3 F = F_Schlick(F0, VoH);
        float D = D_GGX(NoH, roughness);
        float G = G_Smith(NoV, NoL, roughness);
        
        vec3 specular = (D * G * F) / (4.0 * max(NoV, 0.001) * max(NoL, 0.001));
        vec3 kD = (1.0 - F) * (1.0 - metallic);
        vec3 diffuse = kD * albedo / PI;
        
        Lo += (diffuse + specular) * radiance * NoL;
    }
    
    vec3 color = Lo * ao;
    FragColor = vec4(color, 1.0);
}
)";
}