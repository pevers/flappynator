#include "smoothterrain.h"

SmoothTerrain::SmoothTerrain()
{
    //ctor
}

SmoothTerrain::~SmoothTerrain()
{
    //dtor
    glDeleteTextures(1, textures);
}

glm::vec3 SmoothTerrain::calcAverageNormal(std::vector<glm::vec3> normals)
{
    glm::vec3 total;
    for (glm::vec3 v : normals) {
        total += v;
    }
    return glm::normalize(total / (float)normals.size());
}

/**
 * Generates terrain equals the size height*width.
 */
bool SmoothTerrain::generateTerrain(unsigned int width, unsigned int height)
{
    std::cout << "running smooth terrain generation ... ";

    vertices.clear();
    texCoords.clear();
    normals.clear();
//    indices.clear();

    //accNormals.resize(width * height);
//    normals.resize(width * height);
//    indices.resize(6 * (width - 1) * (height - 1));
    for (float z = 0; z < height; z += 0.1) {
        for (float x = 0; x < width; x += 0.1) {

            //if ((int)x % (int)mountainInterval > mountainInterval * 0.5)
            //    y = scaled_octave_noise_3d(4, 4, 1, -2.0, rand() % 6, x, y, z);

            float y1 = scaled_octave_noise_2d(5, 0.01, 1, 0.0, 0.3, x, z);
            float y2 = scaled_octave_noise_2d(5, 0.01, 1, 0.0, 0.3, x, z+0.1);
            float y3 = scaled_octave_noise_2d(5, 0.01, 1, 0.0, 0.3, x+0.1, z);
            float y4 = scaled_octave_noise_2d(5, 0.01, 1, 0.0, 0.3, x+0.1, z+0.1);
            //float y1 = 0, y2 = 0, y3 = 0, y4 = 0;

            glm::vec3 t1 = glm::vec3(x, y1, z);
            glm::vec3 t2 = glm::vec3(x, y2, z+0.1);
            glm::vec3 t3 = glm::vec3(x+0.1, y3, z);
            glm::vec3 t4 = glm::vec3(x+0.1, y4, z+0.1);

            vertices.push_back(t1); vertices.push_back(t2); vertices.push_back(t3);
            vertices.push_back(t3); vertices.push_back(t2); vertices.push_back(t4);

            // calculate normals
//            accNormals[z * width + x].push_back(glm::cross(t3 - t1, t2 - t1));
//            accNormals[z * width + x + 1].push_back(glm::cross(t3 - t1, t2 - t1));
//            accNormals[z * width + x + 2].push_back(glm::cross(t3 - t1, t2 - t1));
//
//            accNormals[z * width + x + 1].push_back(glm::cross(t3 - t4, t2 - t4));
//            accNormals[z * width + x + 2].push_back(glm::cross(t3 - t4, t2 - t4));
//            accNormals[z * width + x + 3].push_back(glm::cross(t3 - t4, t2 - t4));

            normals.push_back(glm::cross(t2 - t1, t3 - t1));
            normals.push_back(glm::cross(t2 - t1, t3 - t1));
            normals.push_back(glm::cross(t2 - t1, t3 - t1));
            normals.push_back(glm::cross(t3 - t4, t2 - t4));
            normals.push_back(glm::cross(t3 - t4, t2 - t4));
            normals.push_back(glm::cross(t3 - t4, t2 - t4));

            texCoords.push_back(glm::vec2(0.0, 0.0));
            texCoords.push_back(glm::vec2(1.0, 0.0));
            texCoords.push_back(glm::vec2(0.0, 1.0));
            texCoords.push_back(glm::vec2(0.0, 1.0));
            texCoords.push_back(glm::vec2(1.0, 0.0));
            texCoords.push_back(glm::vec2(1.0, 1.0));
        }
    }

//    for (unsigned int i = 0; i < height - 1; i++) {
//        for (unsigned int j = 0; j < width - 1; j++) {
//            // set indices first triangle
//            indices[i * (width - 1) * 6 + j * 6] = j + i * width;
//            indices[i * (width - 1) * 6 + j * 6 + 1] = j + 1 + i * width;
//            indices[i * (width - 1) * 6 + j * 6 + 2] = j + (i + 1) * width;
//
//            // set indices second triangle
//            indices[i * (width - 1) * 6 + j * 6 + 3] = j + (i + 1) * width;
//            indices[i * (width - 1) * 6 + j * 6 + 4] = j + 1 + i * width;
//            indices[i * (width - 1) * 6 + j * 6 + 5] = j + 1 + (i + 1) * width;
//        }
//    }
//
//    // calculate normals
//    for (int i = 0; i < indices.size(); i = i + 3) {
//
//        accNormals[indices[i]].push_back(glm::cross(vertices[indices[i+2]] - vertices[indices[i]], vertices[indices[i+1]] - vertices[indices[i]]));
//        accNormals[indices[i+1]].push_back(glm::cross(vertices[indices[i+2]] - vertices[indices[i]], vertices[indices[i+1]] - vertices[indices[i]]));
//        accNormals[indices[i+2]].push_back(glm::cross(vertices[indices[i+2]] - vertices[indices[i]], vertices[indices[i+1]] - vertices[indices[i]]));
//
//        normals[indices[i]] = calcAverageNormal(accNormals[indices[i]]);
//        normals[indices[i+1]] = calcAverageNormal(accNormals[indices[i]]);
//        normals[indices[i+2]] = calcAverageNormal(accNormals[indices[i]]);
//    }

    int texWidth, texHeight;
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

    // load normals
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), &normals[0], GL_STATIC_DRAW);

//    // load indices into VBO
//    glGenBuffers(1, &elementBuffer);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

    std::cout << "done" << std::endl;
    return true;
}

unsigned int SmoothTerrain::getTerrainSize()
{
    return vertices.size();
}
