#include "textRenderer.hpp"
#include "debug/log.hpp"
#include "shader.hpp"
#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <string>

TextRenderer::TextRenderer(GLuint width, GLuint height, std::shared_ptr<Shader> shader) : textShader(shader)
{
    textShader->setMatrix4(
        "projection", glm::ortho(0.0f, static_cast<GLfloat>(width), static_cast<GLfloat>(height), 0.0f), true);
    textShader->setInteger("text", 0);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextRenderer::load(std::string font, GLuint fontSize)
{
    characters.clear();

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        LOG_ERROR("FreeType: Could not initialize FreeType!");
    }

    FT_Face face;
    if (FT_New_Face(ft, font.c_str(), 0, &face))
    {
        LOG_ERROR("FreeType: Failed to load font!");
    }

    FT_Set_Pixel_Sizes(face, 0, fontSize);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (GLubyte c = 0; c < 128; ++c)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            LOG_ERROR("FreeType: Failed to load glyph!");
        }

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows,
            0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };

        characters.insert(std::pair<GLchar, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void TextRenderer::draw(std::string text, glm::vec2 position, GLfloat scale, glm::vec4 color, bool rightToLeft)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    textShader->use();
    textShader->setVector4f("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    if (rightToLeft)
    {
        for (auto c = text.rbegin(); c != text.rend(); ++c)
        {
            Character ch = characters[*c];

            GLfloat xpos = position.x + ch.bearing.x * scale;
            GLfloat ypos = position.y + (characters['H'].bearing.y - ch.bearing.y) * scale;

            GLfloat w = ch.size.x * scale;
            GLfloat h = ch.size.y * scale;

            GLfloat vertices[6][4] = {
                    { xpos, ypos + h, 0.0f, 1.0f },
                    { xpos + w, ypos, 1.0f, 0.0f },
                    { xpos, ypos, 0.0f, 0.0f },

                    { xpos, ypos + h, 0.0f, 1.0f },
                    { xpos + w, ypos + h, 1.0f, 1.0f },
                    { xpos + w, ypos, 1.0f, 0.0f }
                };

                glBindTexture(GL_TEXTURE_2D, ch.textureID);

                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

                glDrawArrays(GL_TRIANGLES, 0, 6);

                position.x -= (ch.advance >> 6) * scale;
        }
    }
    else
    {
        for (auto c = text.begin(); c != text.end(); ++c)
        {
            Character ch = characters[*c];

            GLfloat xpos = position.x + ch.bearing.x * scale;
            GLfloat ypos = position.y + (characters['H'].bearing.y - ch.bearing.y) * scale;

            GLfloat w = ch.size.x * scale;
            GLfloat h = ch.size.y * scale;

            GLfloat vertices[6][4] = {
                    { xpos, ypos + h, 0.0f, 1.0f },
                    { xpos + w, ypos, 1.0f, 0.0f },
                    { xpos, ypos, 0.0f, 0.0f },

                    { xpos, ypos + h, 0.0f, 1.0f },
                    { xpos + w, ypos + h, 1.0f, 1.0f },
                    { xpos + w, ypos, 1.0f, 0.0f }
                };

                glBindTexture(GL_TEXTURE_2D, ch.textureID);

                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

                glDrawArrays(GL_TRIANGLES, 0, 6);

                position.x += (ch.advance >> 6) * scale;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
}
