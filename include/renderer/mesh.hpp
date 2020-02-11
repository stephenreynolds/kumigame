#ifndef KUMIGAME_RENDERER_MESH_HPP
#define KUMIGAME_RENDERER_MESH_HPP

#include "shader.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitTangent;
};

struct Texture
{
    GLuint id;
    std::string type;
    std::string path;
};

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, std::vector<Texture> &textures);

    void render(const std::shared_ptr<Shader>& shader);

private:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;

    void setupMesh();
};

#endif //KUMIGAME_RENDERER_MESH_HPP
