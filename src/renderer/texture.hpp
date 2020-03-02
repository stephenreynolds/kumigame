#ifndef KUMIGAME_RENDERER_TEXTURE_HPP
#define KUMIGAME_RENDERER_TEXTURE_HPP

#include <glad/glad.h>
#include <string>

struct Texture
{
    GLuint id;
    std::string type = "";
    std::string path = "";
};

uint32_t textureFromFile(const std::string& path, const std::string& directory);

#endif //KUMIGAME_RENDERER_TEXTURE_HPP
