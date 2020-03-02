#ifndef KUMIGAME_RENDERER_MATERIAL_HPP
#define KUMIGAME_RENDERER_MATERIAL_HPP

#include "texture.hpp"
#include <glad/glad.h>
#include <string>
#include <memory>
#include <string>

struct Material
{
    std::shared_ptr<Texture> diffuse = nullptr;
    std::shared_ptr<Texture> specular = nullptr;
    float shininess = 0.0f;
};

#endif //KUMIGAME_RENDERER_MATERIAL_HPP
