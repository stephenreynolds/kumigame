#ifndef KUMIGAME_TEXT_RENDERER_HPP
#define KUMIGAME_TEXT_RENDERER_HPP

#include "shader.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <string>

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

    TextRenderer(GLuint width, GLuint height, std::shared_ptr<Shader> shader);

    void load(std::string font, GLuint fontSize);
    void draw(std::string text, glm::vec2 position, GLfloat scale,
        glm::vec4 color = glm::vec4(1.0f), bool rightToLeft = false);

private:
    GLuint VAO;
    GLuint VBO;
};

#endif
