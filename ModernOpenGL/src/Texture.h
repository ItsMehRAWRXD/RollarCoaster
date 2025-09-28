#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <memory>

class Texture {
public:
    Texture();
    ~Texture();

    bool Create(int width, int height, GLenum internalFormat, GLenum format, GLenum type, const std::string& name = "");
    bool LoadFromFile(const std::string& filepath);
    bool LoadCubemap(const std::vector<std::string>& faces);
    
    void Bind(int unit = 0);
    void BindArray(int unit = 0);
    void Unbind();
    
    void SetData(const void* data);
    void SetData(const void* data, int width, int height, GLenum format, GLenum type);
    
    // Getters
    GLuint GetID() const { return m_textureID; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    GLenum GetFormat() const { return m_format; }
    GLenum GetInternalFormat() const { return m_internalFormat; }
    const std::string& GetName() const { return m_name; }
    
    // Setters
    void SetName(const std::string& name) { m_name = name; }
    
    // Texture parameters
    void SetMinFilter(GLenum filter);
    void SetMagFilter(GLenum filter);
    void SetWrapS(GLenum wrap);
    void SetWrapT(GLenum wrap);
    void SetWrapR(GLenum wrap);
    void SetAnisotropicFiltering(float maxAniso);
    void GenerateMipmaps();
    
    // Utility
    bool IsValid() const { return m_textureID != 0; }
    void Destroy();

private:
    GLuint m_textureID;
    int m_width, m_height;
    GLenum m_internalFormat;
    GLenum m_format;
    GLenum m_type;
    std::string m_name;
    bool m_initialized;
    
    void SetupDefaultParameters();
};