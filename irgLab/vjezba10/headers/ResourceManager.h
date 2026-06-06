#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <assimp/Importer.hpp>      
#include <assimp/scene.h>           
#include <assimp/postprocess.h>
#include "Renderable.h"
#include "Shader.h"
#include "TriangleMesh.h"
#include "Material.h"
#include "Texture.h"

#define RESOURCES_PATH "resources"

class ResourceManager {
private:
    std::unordered_map<std::string, std::vector<std::shared_ptr<Renderable>>> scenes;
    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures;

    ResourceManager() = default;

    TriangleMesh parseMesh(aiMesh *mesh, const aiScene *scene, const std::string &directory);
    Material parseMaterial(aiMaterial *material) const;

public:
    // Singleton
    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }
    ResourceManager(const ResourceManager&) = delete;

    ResourceManager& operator=(const ResourceManager&) = delete;

    std::vector<std::shared_ptr<Renderable>> getScene(const std::string &name);
    std::shared_ptr<Shader> getShader(const std::string &name, bool useGeometryShader = true);
    std::shared_ptr<Texture> getTexture(const std::string &name);
};