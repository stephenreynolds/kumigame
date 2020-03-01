#include "glDebug.hpp"
#include "log.hpp"
#include <glad/glad.h>
#include <sstream>

void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity,
                            GLsizei length, const GLchar* message, const void* userParam)
{
    // Ignore non-significant error/warning codes.
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
    {
        return;
    }

    std::stringstream output;
    output << "OpenGL: " << message;

    output << "\n\tID: " << id;

    output << "\n\tSource: ";
    switch (source)
    {
        case GL_DEBUG_SOURCE_API:
        {
            output << "API";
            break;
        }
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        {
            output << "Window System";
            break;
        }
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
        {
            output << "Shader Compiler";
            break;
        }
        case GL_DEBUG_SOURCE_THIRD_PARTY:
        {
            output << "Third Party";
            break;
        }
        case GL_DEBUG_SOURCE_APPLICATION:
        {
            output << "Application";
            break;
        }
        case GL_DEBUG_SOURCE_OTHER:
        {
            output << "Other";
            break;
        }
        default:
            break;
    }

    output << "\n\tType: ";
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
        {
            output << "Error";
            break;
        }
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        {
            output << "Deprecated Behavior";
            break;
        }
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        {
            output << "Undefined Behavior";
            break;
        }
        case GL_DEBUG_TYPE_PORTABILITY:
        {
            output << "Portability";
            break;
        }
        case GL_DEBUG_TYPE_PERFORMANCE:
        {
            output << "Performance";
            break;
        }
        case GL_DEBUG_TYPE_MARKER:
        {
            output << "Marker";
            break;
        }
        case GL_DEBUG_TYPE_PUSH_GROUP:
        {
            output << "Push Group";
            break;
        }
        case GL_DEBUG_TYPE_POP_GROUP:
        {
            output << "Pop Group";
            break;
        }
        case GL_DEBUG_TYPE_OTHER:
        {
            output << "Other";
            break;
        }
        default:
            break;
    }

    output << "\n\tSeverity: ";
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
        {
            output << "High";
            break;
        }
        case GL_DEBUG_SEVERITY_MEDIUM:
        {
            output << "Medium";
            break;
        }
        case GL_DEBUG_SEVERITY_LOW:
        {
            output << "Low";
            break;
        }
        case GL_DEBUG_SEVERITY_NOTIFICATION:
        {
            output << "Notification";
            break;
        }
        default:
            break;
    }

    LOG_WARN(output.str());
}
