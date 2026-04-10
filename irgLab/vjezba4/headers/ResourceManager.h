#pragma once

#include "Object.h"
#include "Shader.h"
#include "Texture.h"
#include <string>
#include <unordered_map>
#include <memory>

class ResourceManager {
private:
    std::unordered_map<std::string, std::unique_ptr<Object>> objects;
    std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;
    std::unordered_map<std::string, std::unique_ptr<Texture>> textures;

    ResourceManager() = default;

public:
    static ResourceManager& getInstace() {
        static ResourceManager instance;
        return instance;
    }
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    Object getScene(std::string name);
    Shader getShader(std::string name);
    Texture getTexture(std::string name);
};