#ifndef KUMIGAME_DEBUG_GL_DEBUG_HPP
#define KUMIGAME_DEBUG_GL_DEBUG_HPP

#include <glad/glad.h>

void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity,
                            GLsizei length, const GLchar* message, const void* userParam);

#endif //KUMIGAME_DEBUG_GL_DEBUG_HPP
