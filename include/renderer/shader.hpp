#ifndef KUMIGAME_RENDERER_SHADER_HPP
#define KUMIGAME_RENDERER_SHADER_HPP

#include <glm/glm.hpp>
#include <glad/glad.h>

class Shader
{
public:
    GLuint id;

    Shader();
    Shader(const GLchar *vertexShaderFile, const GLchar *fragmentShaderFile);
    Shader(const GLchar *vertexShaderFile, const GLchar *fragmentShaderFile, const GLchar *geometryShaderFile);

    int getUniformLocation(const GLchar *name);

    void setFloat(const GLchar *name, GLfloat value);
    void setInteger(const GLchar *name, GLint value);
    void setVector2f(const GLchar *name, GLfloat x, GLfloat y);
    void setVector2f(const GLchar *name, const glm::vec2 &value);
    void setVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z);
    void setVector3f(const GLchar *name, const glm::vec3 &value);
    void setVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    void setVector4f(const GLchar *name, const glm::vec4 &value);
    void setMatrix4(const GLchar *name, const glm::mat4 &matrix);

    Shader &use();
    void stop();
    void loadFromFile(const GLchar *vertexShaderFile, const GLchar *fragmentShaderFile, const GLchar *geometryShaderFile);
    void compile(const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource = nullptr);
};

#endif //KUMIGAME_RENDERER_SHADER_HPP
