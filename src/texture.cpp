#include "texture.hpp"
#include "debug/log.hpp"
#include <glad/glad.h>
#include <stb_image.h>
#include <map>

std::map<const char*, Texture2D> Texture2D::textures;

Texture2D::Texture2D()
    : width(0), height(0), internalFormat(GL_RGB), imageFormat(GL_RGB), wrapS(GL_REPEAT), wrapT(GL_REPEAT),
      filterMin(GL_LINEAR), filterMag(GL_LINEAR)
{
    glGenTextures(1, &id);
}

Texture2D::Texture2D(const char* name, const char* path, bool alpha)
    : Texture2D()
{
    loadFromFile(path, alpha);
    textures[name] = *this;
}

Texture2D::~Texture2D()
{
    glDeleteTextures(1, &id);
}

void Texture2D::bind() const
{
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture2D::clear()
{
    textures.clear();
}

Texture2D Texture2D::get(const char* name)
{
    return textures[name];
}

void Texture2D::loadFromFile(const char* path, bool alpha)
{
    int desiredChannels = STBI_rgb;
    if (alpha)
    {
        internalFormat = GL_RGBA;
        imageFormat = GL_RGBA;
        desiredChannels = STBI_rgb_alpha;
    }

    stbi_set_flip_vertically_on_load(true);
    int width;
    int height;
    int numChannels;
    unsigned char* data = stbi_load(path, &width, &height, &numChannels, desiredChannels);

    if (data)
    {
        generate(width, height, data);
    }
    else
    {
        LOG_ERROR("Failed to load texture {}", path);
    }

    stbi_image_free(data);
}

void Texture2D::generate(GLuint width, GLuint height, unsigned char* data)
{
    this->width = width;
    this->height = height;

    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMag);
}
