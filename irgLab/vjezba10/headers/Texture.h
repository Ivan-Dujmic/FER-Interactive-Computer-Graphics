#pragma once

#include <glad/glad.h>
#include <string>

class Texture {
private:
    GLuint textureID;
    int width;
    int height;
    int channels;

public:
    Texture();
    explicit Texture(const std::string &path);

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture &&other) noexcept;
    Texture& operator=(Texture &&other) noexcept;

    ~Texture();

    GLuint getTextureId() const;
    bool isValid() const;

    void bind(GLuint unit = 0) const;
};