#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <utility>

Texture::Texture() :
    textureID(0),
    width(0),
    height(0),
    channels(0)
{}

Texture::Texture(const std::string &path) :
    textureID(0),
    width(0),
    height(0),
    channels(0)
{
    stbi_set_flip_vertically_on_load(false); // Already flipped in ResourceManager

    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

    if (!data) {
        std::cerr << "Failed to load texture: " << path << "\n";
        std::cerr << "stb_image error: " << stbi_failure_reason() << "\n";
        return;
    }

    GLenum format;
    if (channels == 1) {
        format = GL_RED;
    } else if (channels == 3) {
        format = GL_RGB;
    } else if (channels == 4) {
        format = GL_RGBA;
    } else {
        std::cerr << "Unsupported texture channel count: " << channels << " in " << path << "\n";
        stbi_image_free(data);
        return;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Repeat wrapping for U coordinate
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Repeat wrapping for V coordinate

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Linear interpolation between mipmap levels for minification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Linear interpolation between pixels for magnification
    
    glTexImage2D( // Uploads image to active texture unit on GPU
        GL_TEXTURE_2D,
        0, // Original, full resolution
        format, // Format to store in GPU
        width,
        height,
        0, // No border (legacy OpenGL requirement)
        format, // Format of the input data
        GL_UNSIGNED_BYTE, // Type of each channel
        data // Pointer to pixels
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data); // Free in RAM
}

Texture::Texture(Texture &&other) noexcept :
    textureID(other.textureID),
    width(other.width),
    height(other.height),
    channels(other.channels)
{
    other.textureID = 0;
    other.width = 0;
    other.height = 0;
    other.channels = 0;
}

Texture& Texture::operator=(Texture &&other) noexcept {
    if (this != &other) {
        if (textureID != 0) {
            glDeleteTextures(1, &textureID);
        }

        textureID = other.textureID;
        width = other.width;
        height = other.height;
        channels = other.channels;

        other.textureID = 0;
        other.width = 0;
        other.height = 0;
        other.channels = 0;
    }

    return *this;
}

Texture::~Texture() {
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
    }
}

GLuint Texture::getTextureId() const {
    return textureID;
}

bool Texture::isValid() const {
    return textureID != 0;
}

void Texture::bind(GLuint unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, textureID);
}