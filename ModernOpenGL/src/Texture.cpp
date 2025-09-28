#include "Texture.h"
#include <iostream>
#include <stb_image.h>

Texture::Texture() 
    : m_textureID(0)
    , m_width(0)
    , m_height(0)
    , m_internalFormat(GL_RGBA)
    , m_format(GL_RGBA)
    , m_type(GL_UNSIGNED_BYTE)
    , m_name("")
    , m_initialized(false)
{
}

Texture::~Texture() {
    Destroy();
}

bool Texture::Create(int width, int height, GLenum internalFormat, GLenum format, GLenum type, const std::string& name) {
    if (m_initialized) {
        Destroy();
    }
    
    m_width = width;
    m_height = height;
    m_internalFormat = internalFormat;
    m_format = format;
    m_type = type;
    m_name = name;
    
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    
    glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height, 0, m_format, m_type, nullptr);
    
    SetupDefaultParameters();
    
    m_initialized = true;
    return true;
}

bool Texture::LoadFromFile(const std::string& filepath) {
    int width, height, channels;
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
    
    if (!data) {
        std::cerr << "Failed to load texture: " << filepath << std::endl;
        return false;
    }
    
    GLenum format;
    GLenum internalFormat;
    
    switch (channels) {
        case 1:
            format = GL_RED;
            internalFormat = GL_R8;
            break;
        case 3:
            format = GL_RGB;
            internalFormat = GL_RGB8;
            break;
        case 4:
            format = GL_RGBA;
            internalFormat = GL_RGBA8;
            break;
        default:
            std::cerr << "Unsupported texture format: " << channels << " channels" << std::endl;
            stbi_image_free(data);
            return false;
    }
    
    bool success = Create(width, height, internalFormat, format, GL_UNSIGNED_BYTE, filepath);
    if (success) {
        SetData(data);
    }
    
    stbi_image_free(data);
    return success;
}

bool Texture::LoadCubemap(const std::vector<std::string>& faces) {
    if (faces.size() != 6) {
        std::cerr << "Cubemap requires exactly 6 faces" << std::endl;
        return false;
    }
    
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
    
    int width, height, channels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
        if (data) {
            GLenum format;
            if (channels == 1) format = GL_RED;
            else if (channels == 3) format = GL_RGB;
            else if (channels == 4) format = GL_RGBA;
            else {
                std::cerr << "Unsupported cubemap format: " << channels << " channels" << std::endl;
                stbi_image_free(data);
                return false;
            }
            
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cerr << "Failed to load cubemap face: " << faces[i] << std::endl;
            return false;
        }
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    m_width = width;
    m_height = height;
    m_initialized = true;
    return true;
}

void Texture::Bind(int unit) {
    if (!m_initialized) return;
    
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void Texture::BindArray(int unit) {
    if (!m_initialized) return;
    
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureID);
}

void Texture::Unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetData(const void* data) {
    if (!m_initialized) return;
    
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, m_format, m_type, data);
}

void Texture::SetData(const void* data, int width, int height, GLenum format, GLenum type) {
    if (!m_initialized) return;
    
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, type, data);
}

void Texture::SetMinFilter(GLenum filter) {
    if (!m_initialized) return;
    
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
}

void Texture::SetMagFilter(GLenum filter) {
    if (!m_initialized) return;
    
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
}

void Texture::SetWrapS(GLenum wrap) {
    if (!m_initialized) return;
    
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
}

void Texture::SetWrapT(GLenum wrap) {
    if (!m_initialized) return;
    
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
}

void Texture::SetWrapR(GLenum wrap) {
    if (!m_initialized) return;
    
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrap);
}

void Texture::SetAnisotropicFiltering(float maxAniso) {
    if (!m_initialized) return;
    
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
}

void Texture::GenerateMipmaps() {
    if (!m_initialized) return;
    
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::Destroy() {
    if (m_textureID) {
        glDeleteTextures(1, &m_textureID);
        m_textureID = 0;
    }
    m_initialized = false;
}

void Texture::SetupDefaultParameters() {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}