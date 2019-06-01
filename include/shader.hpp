#ifndef KUMIGAME_SHADER_HPP
#define KUMIGAME_SHADER_HPP

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <map>
#include <string>

class Shader
{
public:
    GLuint id;

    Shader();
    Shader(const GLchar *vertexShaderFile, const GLchar *fragmentShaderFile, const std::string& name);
    Shader(const GLchar *vertexShaderFile, const GLchar *fragmentShaderFile, const GLchar *geometryShaderFile, const std::string& name);

    int getUniformLocation(const GLchar *name);

    void setFloat(const GLchar *name, GLfloat value, GLboolean useShader = false);
    void setInteger(const GLchar *name, GLint value, GLboolean useShader = false);
    void setVector2f(const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader = false);
    void setVector2f(const GLchar *name, const glm::vec2 &value, GLboolean useShader = false);
    void setVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader = false);
    void setVector3f(const GLchar *name, const glm::vec3 &value, GLboolean useShader = false);
    void setVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader = false);
    void setVector4f(const GLchar *name, const glm::vec4 &value, GLboolean useShader = false);
    void setMatrix4(const GLchar *name, const glm::mat4 &matrix, GLboolean useShader = false);

    static Shader get(std::string name);
    static void clear();

    Shader &use();
    void stop();
    void loadFromFile(const GLchar *vertexShaderFile, const GLchar *fragmentShaderFile, const GLchar *geometryShaderFile);
    void compile(const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource = nullptr);

private:
    static std::map<std::string, Shader> shaders;
};

#endif
