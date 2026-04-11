#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "Renderable.h"
#include "Shader.h"
// #include "Texture.h"

#define RESOURCES_PATH "/resources/"

class ResourceManager {
private:
    std::unordered_map<std::string, std::shared_ptr<Renderable>> renderables;
    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;
    // std::unordered_map<std::string, std::shared_ptr<Texture>> textures;

    ResourceManager() = default;

public:
    // Singleton
    static ResourceManager& getInstace() {
        static ResourceManager instance;
        return instance;
    }
    ResourceManager(const ResourceManager&) = delete;

    ResourceManager& operator=(const ResourceManager&) = delete;

    std::shared_ptr<Renderable> getRenderable(const std::string &name);
    std::shared_ptr<Shader> getShader(const std::string &name);
    // std::shared_ptr<Texture> getTexture(std::string name);
};