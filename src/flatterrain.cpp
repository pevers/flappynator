#include "flatterrain.h"

FlatTerrain::FlatTerrain()
{
    //ctor
}

FlatTerrain::~FlatTerrain()
{
    //dtor
    glDeleteTextures(1, textures);
}

/**
 * Generates terrain equals the size height*width.
 */
bool FlatTerrain::generateTerrain(int width, int height)
{
    std::cout << "running terrain generation ... ";

    vertices.clear();
    float y = 0;    // this is a really simple terrain generator
    for (float z = 0; z < height; z++) {
        for (float x = 0; x < width; x++) {
            // create triangles
            vertices.push_back(glm::vec3(x + 1, y, z));
            vertices.push_back(glm::vec3(x, y, z));
            vertices.push_back(glm::vec3(x, y, z + 1));

            texCoords.push_back(glm::vec2(1, 0));
            texCoords.push_back(glm::vec2(0, 0));
            texCoords.push_back(glm::vec2(0, 1));

            vertices.push_back(glm::vec3(x + 1, y, z));
            vertices.push_back(glm::vec3(x, y, z + 1));
            vertices.push_back(glm::vec3(x + 1, y, z + 1));

            texCoords.push_back(glm::vec2(1, 0));
            texCoords.push_back(glm::vec2(0, 1));
            texCoords.push_back(glm::vec2(1, 1));
        }
    }

    unsigned char *image = SOIL_load_image("resources/grass.png", &texWidth, &texHeight, 0, SOIL_LOAD_RGB);
    if (!image) {
        std::cerr << "could not load image resources/grass.png" << std::endl;
    } else {
        // load textures
        glGenTextures(1, textures);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        SOIL_free_image_data(image);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // add textures to texture buffer
        glGenBuffers(1, &vboTexCoords);
        glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * texCoords.size(), &texCoords[0], GL_STATIC_DRAW);
    }

    // load data into VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    std::cout << "done" << std::endl;
    return true;
}

unsigned int FlatTerrain::getTerrainSize()
{
    return vertices.size();
}
