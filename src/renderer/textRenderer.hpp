#ifndef KUMIGAME_RENDERER_TEXT_RENDERER_HPP
#define KUMIGAME_RENDERER_TEXT_RENDERER_HPP

#include "shader.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <glad/glad.h>
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
    TextRenderer(GLuint width, GLuint height, std::shared_ptr<Shader>& shader, const std::string& fontPath, GLuint fontSize);

    void render(std::string text, glm::vec2 position, GLfloat scale,
                glm::vec4 color = glm::vec4(1.0f), bool rightToLeft = false);

private:
    GLuint vao = 0;
    GLuint vbo = 0;
    std::shared_ptr<Shader> shader;
    std::map<GLchar, Character> characters;

    void initRenderDescriptor(GLuint width, GLuint height, std::shared_ptr<Shader> &textShader);
    void loadFont(const std::string& fontPath, GLuint fontSize);
};

#endif //KUMIGAME_RENDERER_TEXT_RENDERER_HPP
