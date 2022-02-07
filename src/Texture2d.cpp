#include "Texture2d.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include <iostream>
using namespace std;

Texture2D::Texture2D() : mTexture(0) {

}

Texture2D::~Texture2D() {

}

bool Texture2D::loadTexture(const string& filename, bool generateMipMaps) {
    int width, height, components;

    unsigned char* imageData = stbi_load(filename.c_str(), &width, &height, &components, STBI_rgb_alpha);
    if (imageData == NULL) {
        cerr << "Error loading texture '" << filename << "'" << endl;
        return false;
    }

    // create "1" texture, and pass it back to our variable
    glGenTextures(1, &mTexture);
    // sets context for future calls
    glBindTexture(GL_TEXTURE_2D, mTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

    if (generateMipMaps) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(imageData);
    // binds to nothing so we dont affect our current texture
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

void Texture2D::bind(GLuint texUnit) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, mTexture);
}