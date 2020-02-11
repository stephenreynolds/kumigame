#ifndef KUMIGAME_RENDER_MODEL_HPP
#define KUMIGAME_RENDER_MODEL_HPP

#include "renderer/mesh.hpp"
#include <assimp/scene.h>
#include <memory>
#include <string>
#include <vector>

class Model
{
public:
    explicit Model(const std::string& path);

    void render(const std::shared_ptr<Shader>& shader);

private:
    std::vector<Mesh> meshes;
    std::vector<Texture> texturesLoaded;
    std::string directory;

    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, const std::string& typeName);
};

#endif //KUMIGAME_RENDER_MODEL_HPP
