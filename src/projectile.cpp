#include "projectile.h"

GLfloat projectileVertices[] = {
    /*-1.0, 0.5, 0.0, // top left corner
    1.0, 0.5, 0.0, // top right corner
    -1.0, -0.05, 0.0, // bot left corner
    1.0,-0.05, 0.0, // bot right corner
    */
    -1.0f,-0.05f,-1.0f, // triangle 1 : begin
    -1.0f,-0.05f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end
    1.0f, 1.0f,-1.0f, // triangle 2 : begin
    -1.0f,-0.05f,-1.0f,
    -1.0f, 1.0f,-1.0f, // triangle 2 : end
    1.0f,-0.05f, 1.0f,
    -1.0f,-0.05f,-1.0f,
    1.0f,-0.05f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-0.05f,-1.0f,
    -1.0f,-0.05f,-1.0f,
    -1.0f,-0.05f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-0.05f, 1.0f,
    -1.0f,-0.05f, 1.0f,
    -1.0f,-0.05f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-0.05f, 1.0f,
    1.0f,-0.05f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-0.05f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-0.05f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-0.05f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-0.05f, 1.0f

    };

GLubyte indices[] = {0,1,2, // first triangle (bottom left - top left - top right)
                     0,2,3};

Projectile::Projectile(glm::vec3 start, glm::vec3 playerRotation) : WorldObject(start, Settings::playerScale, glm::vec3(0.0f, 0.0f, 0.0f)), speed(Settings::projectileSpeed), playerRotation(playerRotation)
{
    if (!load()) {
        std::cerr << "could not load projectile" << std::endl;
    }
}

Projectile::~Projectile()
{
    //dtor

}

GLuint Projectile::getTextureBuffer()
{
    return vboTexCoords;
}

void Projectile::setSpeed(glm::vec3 speed)
{
    this->speed = speed;
}

glm::vec3 Projectile::getSpeed()
{
    return speed;
}

/**
 * Update projectile speed
 */
void Projectile::update()
{
    rotation = playerRotation;
    pos += speed;
    if(playerRotation.z < 0) {
        pos.y += sqrt(pow(speed.x/cos(playerRotation.z), 2) - pow(speed.x, 2));
    } else {
        pos.y -= sqrt(pow(speed.x/cos(playerRotation.z), 2) - pow(speed.x, 2));
    }
    //pos += glm::vec3(0.0f, tan(glm::normalize(playerRotation).z/*180/M_PI*/)/speed.x, 0.0f);
}

void Projectile::free()
{
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &elementBuffer);
    glDeleteBuffers(1, &normalBuffer);
}

int Projectile::getObjectSize()
{
    return sizeof(indices);
}


bool Projectile::load()
{
    /*texCoords.push_back(glm::vec2(0.0, 0.0));
    texCoords.push_back(glm::vec2(1.0, 0.0));
    texCoords.push_back(glm::vec2(0.0, 1.0));
    texCoords.push_back(glm::vec2(0.0, 1.0));
    texCoords.push_back(glm::vec2(1.0, 0.0));
    texCoords.push_back(glm::vec2(1.0, 1.0));

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
    }*/

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(projectileVertices), projectileVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) , indices, GL_STATIC_DRAW);

    return true;
}

void Projectile::destroyObject()
{
}
GLuint Projectile::getTexture()
{
} // HACK

void Projectile::setBoundingBox(glm::vec4 boundingBox)
{
    this->boundingBox = boundingBox;
}

glm::vec4 Projectile::getBoundingBox()
{
    return boundingBox;
}
