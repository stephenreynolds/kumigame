#include "model.hpp"
#include "material.hpp"
#include "shader.hpp"
#include "../debug/log.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>

Model::Model(const std::string &path)
{
    loadModel(path);
}

void Model::render(const std::shared_ptr<Shader>& shader, size_t materialIndex)
{
    for (auto& mesh : meshes)
    {
        mesh.render(shader, materialIndex);
    }
}

size_t Model::addMeshMaterial(size_t meshIndex, Material material)
{
    meshes[meshIndex].materials.emplace_back(std::move(material));
    return meshes[meshIndex].materials.size() - 1;
}

void Model::setMeshMaterial(size_t meshIndex, size_t materialIndex, Material material)
{
    meshes[meshIndex].materials[materialIndex] = std::move(material);
}

void Model::loadModel(const std::string &path)
{
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOG_ERROR("Assimp: {}", import.GetErrorString());
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex = {};

        glm::vec3 vector;

        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;

        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.normal = vector;

        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec = {
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            };
            vertex.texCoords = vec;
        }
        else
        {
            vertex.texCoords = glm::vec2(0.0f);
        }

        // Tangents
        if (mesh->mTangents)
        {
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.tangent = vector;
        }

        // Bittangents
        if (mesh->mBitangents)
        {
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.bitTangent = vector;
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "Texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "Texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "Texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "Texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    std::vector<Texture> emissiveMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, "Texture_emissive");
    textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());

    float shininess;
    material->Get(AI_MATKEY_SHININESS, shininess);

    std::shared_ptr<Texture> diffuse;
    if (!diffuseMaps.empty())
    {
        diffuse = std::make_shared<Texture>(diffuseMaps[0]);
    }
    std::shared_ptr<Texture> specular;
    if (!specularMaps.empty())
    {
        specular = std::make_shared<Texture>(specularMaps[0]);
    }

    Material mat = {
        .diffuse = diffuse,
        .specular = specular,
        .shininess = shininess
    };

    return Mesh(vertices, indices, textures, mat);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *material, aiTextureType type, const std::string& typeName)
{
    std::vector<Texture> textures;

    auto textureCount = material->GetTextureCount(type);
    for (unsigned int i = 0; i < textureCount; ++i)
    {
        aiString str;
        material->GetTexture(type, i, &str);

        bool skip = false;

        for (auto& j : texturesLoaded)
        {
            if (std::strcmp(j.path.data(), str.C_Str()) == 0)
            {
                textures.push_back(j);
                skip = true;
                break;
            }
        }

        if (!skip)
        {
            Texture texture = {
                .id = textureFromFile(str.C_Str(), directory),
                .type = typeName,
                .path = str.C_Str()
            };
            textures.push_back(texture);
            texturesLoaded.push_back(texture);
        }
    }

    return textures;
}


