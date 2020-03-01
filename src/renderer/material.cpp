#include "../debug/log.hpp"
#include <glad/glad.h>
#include <stb_image.h>
#include <string>

uint32_t textureFromFile(const std::string& path, const std::string& directory)
{
    std::string fileName = directory + '/' + path;

    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, numComponents;
    unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &numComponents, 0);
    if (data)
    {
        GLenum format = 0;
        if (numComponents == 1)
        {
            format = GL_RED;
        }
        else if (numComponents == 3)
        {
            format = GL_RGB;
        }
        else if (numComponents == 4)
        {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        LOG_ERROR("Texture failed to load at {}", path);
    }

    stbi_image_free(data);

    return textureID;
}
