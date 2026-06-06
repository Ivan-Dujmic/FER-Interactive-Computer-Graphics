#include "ResourceManager.h"
#include <glm/glm.hpp>
#include <filesystem>
#include <iostream>

std::vector<std::shared_ptr<Renderable>> ResourceManager::getScene(const std::string &name) {
    auto it = scenes.find(name);
    if (it != scenes.end()) {
        return it->second;
    }
        
    Assimp::Importer importer;

    std::filesystem::path directory = std::filesystem::current_path() / RESOURCES_PATH / name;
    std::filesystem::path objPath = directory / (name + ".obj");

	const aiScene* scene = importer.ReadFile(objPath.string().c_str(),
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_FlipUVs |
		aiProcess_GenSmoothNormals
	);

    if (!scene) {
		std::cerr << importer.GetErrorString();
		exit(EXIT_FAILURE);
	}

    // Will have to do a check once we start supporting more than just the TriangleMesh
    // And perhaps split this loading into a separate function or class

    if (!scene->HasMeshes()) {
        std::cerr << "Scene has no meshes\n";
        exit(EXIT_FAILURE);
    }

    std::vector<std::shared_ptr<Renderable>> renderables;
    renderables.reserve(scene->mNumMeshes);

    for (std::size_t i = 0 ; i < scene->mNumMeshes ; i++) {
        renderables.emplace_back(std::make_shared<TriangleMesh>(parseMesh(scene->mMeshes[i], scene, directory.string())));
    }

    scenes[name] = renderables;
    return renderables;
}

TriangleMesh ResourceManager::parseMesh(aiMesh *mesh, const aiScene *scene, const std::string &directory) {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<GLuint> indices;
    std::vector<glm::vec2> uvCoords;
    vertices.reserve(mesh->mNumVertices);
    normals.reserve(mesh->mNumVertices);
    uvCoords.reserve(mesh->mNumVertices);

    for (unsigned int i = 0 ; i < mesh->mNumVertices ; i++) {
        const aiVector3D& v = mesh->mVertices[i];
        vertices.emplace_back(v.x, v.y, v.z);
    }

    if (mesh->HasNormals()) {
        for (unsigned int i = 0 ; i < mesh->mNumVertices ; i++) {
            const aiVector3D& n = mesh->mNormals[i];
            normals.emplace_back(n.x, n.y, n.z);
        }
    }

    if (mesh->HasTextureCoords(0)) {
        for (unsigned int i = 0 ; i < mesh->mNumVertices ; i++) {
            const aiVector3D& t = mesh->mTextureCoords[0][i];
            uvCoords.emplace_back(t.x, t.y);
        }
    } else {
        uvCoords.resize(mesh->mNumVertices, glm::vec2(0.0f));
    }

    if (mesh->HasFaces()) {
        // This pays off if faces can include any number of indices as it avoids reallocating vector data
        unsigned int indicesSize = 0;
        for (unsigned int i = 0 ; i < mesh->mNumFaces ; i++) {
            indicesSize += mesh->mFaces[i].mNumIndices;
        }
        indices.reserve(indicesSize);

        for (unsigned int i = 0 ; i < mesh->mNumFaces ; i++) {
            aiFace f = mesh->mFaces[i];
            for (unsigned int j = 0 ; j < f.mNumIndices ; j++) {
                indices.emplace_back(f.mIndices[j]);
            }
        }
    }

    Material material;
    std::shared_ptr<Texture> diffuseTexture = nullptr;

    if (scene && scene->HasMaterials() && mesh->mMaterialIndex < scene->mNumMaterials) {
        aiMaterial *aiMat = scene->mMaterials[mesh->mMaterialIndex];
        material = parseMaterial(aiMat);

        if (aiMat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString texturePath;
            if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
                std::filesystem::path texturePathFull = std::filesystem::path(directory) / texturePath.C_Str();
                diffuseTexture = getTexture(texturePathFull.string());
            }
        }
    }

    return {
        std::move(vertices),
        std::move(normals),
        std::move(uvCoords),
        std::move(indices),
        std::move(material),
        diffuseTexture
    };
}

static glm::vec3 toGlm(const aiColor3D &c) {
    return glm::vec3(c.r, c.g, c.b);
}

Material ResourceManager::parseMaterial(aiMaterial *material) const {
    aiColor3D ambient(0.1f, 0.1f, 0.1f);
    aiColor3D diffuse(0.8f, 0.8f, 0.8f);
    aiColor3D specular(0.2f, 0.2f, 0.2f);
    float shininess = 32.0f;

    if (material) {
        material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
        material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
        material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
        material->Get(AI_MATKEY_SHININESS, shininess);
    }

    return {toGlm(ambient), toGlm(diffuse), toGlm(specular), shininess};
}

// Very bad to have usesGeometryShader as a parameter here, but it will do for now
std::shared_ptr<Shader> ResourceManager::getShader(const std::string &name, bool useGeometryShader) {
    auto it = shaders.find(name);
    if (it != shaders.end()) {
        return it->second;
    }

    std::shared_ptr<Shader> shader = std::make_shared<Shader>(name, useGeometryShader);
    shaders[name] = shader;
    return shader;
}

std::shared_ptr<Texture> ResourceManager::getTexture(const std::string &name) {
    auto it = textures.find(name);
    if (it != textures.end()) {
        return it->second;
    }

    std::shared_ptr<Texture> texture = std::make_shared<Texture>(name);

    if (!texture->isValid()) {
        std::cerr << "Failed to load texture: " << name << "\n";
        return nullptr;
    }

    textures[name] = texture;
    return texture;
}