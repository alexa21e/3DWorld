#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <GL/glew.h>
#include <string>
#include <map>

class TextureManager {
public:
    TextureManager();
    ~TextureManager();

    // Load a texture and store it with the given name
    GLuint loadTexture(const char* filename, const char* textureName = nullptr);

    // Bind a texture for rendering
    void bindTexture(const char* textureName);
    void bindTexture(GLuint textureID);

    GLuint getTexture(const char* textureName);

    void clearTextures();

private:
    std::map<std::string, GLuint> textureMap;
};

#endif // TEXTURE_MANAGER_H