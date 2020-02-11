#ifndef KUMIGAME_TEXTURE_HPP
#define KUMIGAME_TEXTURE_HPP

#include <glad/glad.h>

class Texture2D
{
public:
    GLuint id;
    GLuint width;
    GLuint height;
    GLuint internalFormat;
    GLuint imageFormat;
    GLuint wrapS;
    GLuint wrapT;
    GLuint filterMin;
    GLuint filterMax;

    Texture2D();
    ~Texture2D();

    void generate(GLuint width, GLuint height, unsigned char* data);
    void bind() const;
};

#endif //KUMIGAME_TEXTURE_HPP
