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
    mountains.clear();

    float miny = 0;
    float maxy = 0.20;
    float detail = 0.5;
    for (float z = -2; z < height; z += detail) {
        for (float x = -2; x < width; x += detail) {
            float y1 = scaled_octave_noise_2d(5, 0.1, 1, miny, maxy, x, z);
            float y2 = scaled_octave_noise_2d(5, 0.1, 1, miny, maxy, x, z+detail);
            float y3 = scaled_octave_noise_2d(5, 0.1, 1, miny, maxy, x+detail, z);
            float y4 = scaled_octave_noise_2d(5, 0.1, 1, miny, maxy, x+detail, z+detail);
            //y1 = 0, y2 = 0, y3 = 0, y4 = 0;

            glm::vec3 t1 = glm::vec3(x, y1, z);
            glm::vec3 t2 = glm::vec3(x, y2, z+detail);
            glm::vec3 t3 = glm::vec3(x+detail, y3, z);
            glm::vec3 t4 = glm::vec3(x+detail, y4, z+detail);

            vertices.push_back(t1); vertices.push_back(t2); vertices.push_back(t3);
            vertices.push_back(t3); vertices.push_back(t2); vertices.push_back(t4);

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

    // generate random hills at random.x and player.z
    srand(time(NULL));
    int msize = width / 2;
    float startx = Settings::playerStart.x + 1.0, startz = Settings::playerStart.z;
    for (int i = 0; i < msize; i++) {
        startx += rand() % 5;
        float starty = rand() % 10;
        mountains.push_back(glm::vec3(startx, starty, startz));
    }

    for (int i = 0; i < vertices.size(); i++) {
        for (int j = 0; j < msize; j++) {
            float distance = glm::distance(glm::vec2(vertices[i].x, vertices[i].z), glm::vec2(mountains[j].x, mountains[j].z));
            if (distance < 4.0) {
                vertices[i].y += 0.2 * (1.0 / (distance + 1.0)) * mountains[j].y;
            }

            // recalculate normal
            /*
            glm::vec3 t1 = vertices[i - (i % 3)];
            glm::vec3 t2 = vertices[i - (i % 3) + 1];
            glm::vec3 t3 = vertices[i - (i % 3) + 2];*/
        }
    }

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

    std::cout << "done" << std::endl;
    return true;
}

unsigned int SmoothTerrain::getTerrainSize()
{
    return vertices.size();
}
