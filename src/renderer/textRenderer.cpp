#include "renderer/textRenderer.hpp"
#include "renderer/shader.hpp"
#include "debug/log.hpp"
#include <glm/gtc/matrix_transform.hpp>

TextRenderer::TextRenderer(GLuint width, GLuint height, std::shared_ptr<Shader> &shader, const std::string& fontPath, GLuint fontSize)
{
    initRenderDescriptor(width, height, shader);
    loadFont(fontPath, fontSize);
}

void TextRenderer::initRenderDescriptor(GLuint width, GLuint height, std::shared_ptr<Shader> &textShader)
{
    shader = textShader;

    shader->use();
    shader->setMatrix4(
        "projection", glm::ortho(0.0f, static_cast<GLfloat>(width), static_cast<GLfloat>(height), 0.0f));
    shader->setInteger("text", 0);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
}

void TextRenderer::loadFont(const std::string& fontPath, GLuint fontSize)
{
    characters.clear();

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        LOG_ERROR("FreeType: Could not initialize FreeType!");
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
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

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void TextRenderer::render(std::string text, glm::vec2 position, GLfloat scale, glm::vec4 color, bool rightToLeft)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader->use();
    shader->setVector4f("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao);

    float lineHeight = characters['H'].size.y;
    float xStart = position.x;

    if (rightToLeft)
    {
        auto newLineCount = std::count(text.begin(), text.end(), '\n');
        position.y += newLineCount * lineHeight * scale * 2.0f;

        for (auto c = text.rbegin(); c != text.rend(); ++c)
        {
            Character ch = characters[*c];

            if (*c == '\n')
            {
                position.x = xStart;
                position.y -= lineHeight * scale * 2.0f;
                continue;
            }

            GLfloat xpos = position.x + ch.bearing.x * scale;
            GLfloat ypos = position.y + static_cast<float>(lineHeight - ch.bearing.y) * scale;

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

            glBindBuffer(GL_ARRAY_BUFFER, vbo);
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

            if (*c == '\n')
            {
                position.x = xStart;
                position.y += lineHeight * scale * 2.0f;
                continue;
            }

            GLfloat xpos = position.x + ch.bearing.x * scale;
            GLfloat ypos = position.y + static_cast<float>(lineHeight - ch.bearing.y) * scale;

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

            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            position.x += (ch.advance >> 6) * scale;
        }
    }

    glBindVertexArray(0);

    glDisable(GL_BLEND);
}
