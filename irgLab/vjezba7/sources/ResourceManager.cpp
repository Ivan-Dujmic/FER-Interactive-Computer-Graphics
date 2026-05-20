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

	std::string path = std::filesystem::current_path().string() + RESOURCES_PATH + name + "/" + name + ".obj";

	const aiScene* scene = importer.ReadFile(path.c_str(),
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_FlipUVs |
		aiProcess_GenNormals
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
        renderables.emplace_back(std::make_shared<TriangleMesh>(parseMesh(scene->mMeshes[i])));
    }

    scenes[name] = renderables;
    return renderables;
}

TriangleMesh ResourceManager::parseMesh(aiMesh* mesh) const {
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

    return {std::move(vertices), std::move(normals), std::move(uvCoords), std::move(indices)};
}

std::shared_ptr<Shader> ResourceManager::getShader(const std::string &name) {
    auto it = shaders.find(name);
    if (it != shaders.end()) {
        return it->second;
    }

    std::shared_ptr<Shader> shader = std::make_shared<Shader>(name, true);
    shaders[name] = shader;
    return shader;
}