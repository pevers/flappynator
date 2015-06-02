#include "smoothterrain.h"

SmoothTerrain::SmoothTerrain()
{
    //ctor
}

SmoothTerrain::~SmoothTerrain()
{
    //dtor
}

/**
 * Generates terrain equals the size height*width.
 */
bool SmoothTerrain::generateTerrain(unsigned int width, unsigned int height)
{
    std::cout << "running smooth terrain generation ... ";

    vertices.clear();
    normals.clear();
    indices.clear();

    vertices.resize(width * height);
    normals.resize(width * height);
    indices.resize(6 * (width - 1) * (height - 1));

    float y = 0;
    srand(time(NULL));
    for (float z = 0; z < height; z++) {
        for (float x = 0; x < width; x++) {
            y = 0.0f;
            if ((int)x % (int)mountainInterval > mountainInterval * 0.5)
                y = scaled_octave_noise_3d(4, 4, 1, -2.0, rand() % 6, x, y, z);

            vertices[z * width + x] = glm::vec3(x/2, y/2, z/2);
        }
    }

    for (unsigned int i = 0; i < height - 1; i++) {
        for (unsigned int j = 0; j < width - 1; j++) {
            // set indices first triangle
            indices[i * (width - 1) * 6 + j * 6] = j + i * width;
            indices[i * (width - 1) * 6 + j * 6 + 1] = j + 1 + i * width;
            indices[i * (width - 1) * 6 + j * 6 + 2] = j + (i + 1) * width;

            // set indices first triangle
            indices[i * (width - 1) * 6 + j * 6 + 3] = j + (i + 1) * width;
            indices[i * (width - 1) * 6 + j * 6 + 4] = j + 1 + i * width;
            indices[i * (width - 1) * 6 + j * 6 + 5] = j + 1 + (i + 1) * width;
        }
    }

    // calculate normals
    for (unsigned int i = 0; i < indices.size(); i = i + 3) {
        // calculate triangle normal
        normals[indices[i]] = glm::cross(vertices[indices[i]], vertices[indices[i+1]]);
        normals[indices[i+1]] = glm::cross(vertices[indices[i]], vertices[indices[i+1]]);
        normals[indices[i+2]] = glm::cross(vertices[indices[i]], vertices[indices[i+1]]);
    }

    // load data into VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // load normals
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), &normals[0], GL_STATIC_DRAW);

    // load indices into VBO
    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

    std::cout << "done" << std::endl;
    return true;
}

unsigned int SmoothTerrain::getTerrainSize()
{
    return indices.size();
}
