#include "TextureManager.h"
#include <iostream>
#include "stb_image.h"

TextureManager::TextureManager() {
}

TextureManager::~TextureManager() {
    clearTextures();
}

GLuint TextureManager::loadTexture(const char* filename, const char* textureName) {
    std::string name = (textureName == nullptr) ? filename : textureName;

    // check if texture is already loaded
    if (textureMap.find(name) != textureMap.end()) {
        return textureMap[name];
    }

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << filename << "\n";
        std::cerr << "stb_image error: " << stbi_failure_reason() << "\n";
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    textureMap[name] = textureID;

    return textureID;
}

void TextureManager::bindTexture(const char* textureName) {
    auto it = textureMap.find(textureName);
    if (it != textureMap.end()) {
        glBindTexture(GL_TEXTURE_2D, it->second);
    }
    else {
        std::cerr << "Texture not found: " << textureName << std::endl;
    }
}

void TextureManager::bindTexture(GLuint textureID) {
    glBindTexture(GL_TEXTURE_2D, textureID);
}

GLuint TextureManager::getTexture(const char* textureName) {
    auto it = textureMap.find(textureName);
    if (it != textureMap.end()) {
        return it->second;
    }
    std::cerr << "Texture not found: " << textureName << std::endl;
    return 0;
}

void TextureManager::clearTextures() {
    for (auto& pair : textureMap) {
        glDeleteTextures(1, &pair.second);
    }
    textureMap.clear();
}