#ifndef KUMIGAME_RENDERER_TEXT_RENDERER_HPP
#define KUMIGAME_RENDERER_TEXT_RENDERER_HPP

#include "shader.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <string>
#include <glad/glad.h>

struct Character
{
    GLuint textureID;
    glm::ivec2 size;
    glm::ivec2 bearing;
    FT_Pos advance;
};

class TextRenderer
{
public:
    std::map<GLchar, Character> characters;
    std::shared_ptr<Shader> textShader;

    TextRenderer(GLuint width, GLuint height, std::shared_ptr<Shader>& shader);
    ~TextRenderer();

    void load(std::string font, GLuint fontSize);
    void draw(std::string text, glm::vec2 position, GLfloat scale,
              glm::vec4 color = glm::vec4(1.0f), bool rightToLeft = false);

private:
    GLuint vao = 0;
    GLuint vbo = 0;
};

#endif //KUMIGAME_RENDERER_TEXT_RENDERER_HPP
