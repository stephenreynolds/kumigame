#include "shader.hpp"
#include "debug/log.hpp"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>

std::map<std::string, Shader> Shader::shaders;

void checkCompileErrors(GLuint object, const std::string &type);

Shader::Shader()
{
}

Shader::Shader(const GLchar *vertexShaderFile, const GLchar *fragmentShaderFile, const std::string& name)
    : Shader(vertexShaderFile, fragmentShaderFile, nullptr, name)
{
}

Shader::Shader(const GLchar *vertexShaderFile, const GLchar *fragmentShaderFile, const GLchar *geometryShaderFile, const std::string& name)
{
    loadFromFile(vertexShaderFile, fragmentShaderFile, geometryShaderFile);
    shaders[name] = *this;
}

int Shader::getUniformLocation(const GLchar *name)
{
    return glGetUniformLocation(id, name);
}

void Shader::setFloat(const GLchar *name, GLfloat value, GLboolean useShader)
{
    if (useShader)
        use();
    glUniform1f(glGetUniformLocation(id, name), value);
}

void Shader::setInteger(const GLchar *name, GLint value, GLboolean useShader)
{
    if (useShader)
        use();
    glUniform1i(glGetUniformLocation(id, name), value);
}

void Shader::setVector2f(const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader)
{
    if (useShader)
        use();
    glUniform2f(glGetUniformLocation(id, name), x, y);
}

void Shader::setVector2f(const GLchar *name, const glm::vec2 &value, GLboolean useShader)
{
    if (useShader)
        use();
    glUniform2f(glGetUniformLocation(id, name), value.x, value.y);
}

void Shader::setVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader)
{
    if (useShader)
        use();
    glUniform3f(glGetUniformLocation(id, name), x, y, z);
}

void Shader::setVector3f(const GLchar *name, const glm::vec3 &value, GLboolean useShader)
{
    if (useShader)
        use();
    glUniform3f(glGetUniformLocation(id, name), value.x, value.y, value.z);
}

void Shader::setVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader)
{
    if (useShader)
        use();
    glUniform4f(glGetUniformLocation(id, name), x, y, z, w);
}

void Shader::setVector4f(const GLchar *name, const glm::vec4 &value, GLboolean useShader)
{
    if (useShader)
        use();
    glUniform4f(glGetUniformLocation(id, name), value.x, value.y, value.z, value.w);
}

void Shader::setMatrix4(const GLchar *name, const glm::mat4 &matrix, GLboolean useShader)
{
    if (useShader)
        use();
    glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

Shader Shader::get(std::string name)
{
    return shaders[name];
}

void Shader::clear()
{
    for (auto iter : shaders)
    {
        glDeleteProgram(iter.second.id);
    }
}

Shader &Shader::use()
{
    glUseProgram(id);

    return *this;
}

void Shader::stop()
{
    glUseProgram(0);
}

void Shader::loadFromFile(
    const GLchar *vertexShaderFile, const GLchar *fragmentShaderFile, const GLchar *geometryShaderFile)
{
    std::stringstream geomLoadText;
    if (geometryShaderFile)
    {
        geomLoadText << "\n\t" << geometryShaderFile;
    }

    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;

    try
    {
        std::ifstream vShaderFile(vertexShaderFile);
        std::ifstream fShaderFile(fragmentShaderFile);
        std::stringstream vShaderStream;
        std::stringstream fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.clear();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        if (geometryShaderFile)
        {
            std::ifstream gShaderFile(geometryShaderFile);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("Failed to read shader files:\n\t{}", e.what());
    }

    const GLchar *vShaderCode = vertexCode.c_str();
    const GLchar *fShaderCode = fragmentCode.c_str();
    const GLchar *gShaderCode = geometryCode.c_str();

    compile(vShaderCode, fShaderCode, geometryShaderFile ? gShaderCode : nullptr);
}

void Shader::compile(const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource)
{
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint geometryShader;

    // Vertex Shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);
    checkCompileErrors(vertexShader, "VERTEX");

    // Fragment Shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);
    checkCompileErrors(fragmentShader, "FRAGMENT");

    // If geometry shader source code is given, also compile geometry shader
    if (geometrySource)
    {
        geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryShader, 1, &geometrySource, nullptr);
        glCompileShader(geometryShader);
        checkCompileErrors(geometryShader, "GEOMETRY");
    }

    // Shader Program
    id = glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    if (geometrySource)
        glAttachShader(id, geometryShader);
    glLinkProgram(id);
    checkCompileErrors(id, "PROGRAM");

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    if (geometrySource)
        glDeleteShader(geometryShader);
}

void checkCompileErrors(GLuint object, const std::string &type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(object, 1024, nullptr, infoLog);
            LOG_ERROR("Shader compile-time error: Type: {}\n{}", type, infoLog);
        }
    }
    else
    {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(object, 1024, nullptr, infoLog);
            LOG_ERROR("Shader link-time error: Type: {}\n{}", type, infoLog);
        }
    }
}
