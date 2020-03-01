#ifndef KUMIGAME_RENDERER_MATERIAL_HPP
#define KUMIGAME_RENDERER_MATERIAL_HPP

#include <glad/glad.h>
#include <string>
#include <memory>
#include <string>

struct Texture
{
    GLuint id;
    std::string type = "";
    std::string path = "";
};

struct Material
{
    std::shared_ptr<Texture> diffuse = nullptr;
    std::shared_ptr<Texture> specular = nullptr;
    float shininess = 0.0f;
};

uint32_t textureFromFile(const std::string& path, const std::string& directory);

#endif //KUMIGAME_RENDERER_MATERIAL_HPP
