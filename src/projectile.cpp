#include "projectile.h"

GLfloat projectileVertices[] = {
    -1.0, 0.5, 0.0, // top left corner
    1.0, 0.5, 0.0, // top right corner
    -1.0, -0.05, 0.0, // bot left corner
    1.0,-0.05, 0.0, // bot right corner
    };

GLubyte indices[] = {0,1,2, // first triangle (bottom left - top left - top right)
                     0,2,3};

Projectile::Projectile(glm::vec3 start, glm::vec3 rotation) : WorldObject(start, Settings::playerScale, rotation), speed(Settings::projectileSpeed)
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
    pos += speed;
    pos += rotation;
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



    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(projectileVertices), projectileVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) , indices, GL_STATIC_DRAW);

    return true;
}

