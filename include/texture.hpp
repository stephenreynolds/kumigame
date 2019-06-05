#ifndef KUMIGAME_TEXTURE_HPP
#define KUMIGAME_TEXTURE_HPP

#include <glad/glad.h>
#include <map>

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
    GLuint filterMag;

    Texture2D();
    Texture2D(const char* name, const char* path, bool alpha = false);
    ~Texture2D();

    void bind() const;

    static void clear();
    static Texture2D get(const char* name);

private:
    static std::map<const char*, Texture2D> textures;

    void loadFromFile(const char* path, bool alpha);
    void generate(GLuint width, GLuint height, unsigned char* data);
};

#endif
