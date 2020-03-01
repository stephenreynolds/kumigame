#include "mesh.hpp"
#include <fmt/format.h>
#include <glad/glad.h>
#include <utility>
#include <vector>

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures, Material material)
    : vertices(std::move(vertices)), indices(std::move(indices)), textures(std::move(textures))
{
    materials.emplace_back(std::move(material));
    setupMesh();
}

void Mesh::render(const std::shared_ptr<Shader>& shader, size_t materialIndex)
{
    shader->use();

    Material material = materials[materialIndex];

    if (material.diffuse)
    {
        glActiveTexture(GL_TEXTURE0);
        shader->setInteger("Material.diffuse", 0);
        glBindTexture(GL_TEXTURE_2D, material.diffuse->id);
    }
    if (material.specular)
    {
        glActiveTexture(GL_TEXTURE1);
        shader->setInteger("Material.specular", 1);
        glBindTexture(GL_TEXTURE_2D, material.specular->id);
    }
    shader->setFloat("Material.shininess", material.shininess);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitTangent));
}
