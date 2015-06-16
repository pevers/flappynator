#include "projectile.h"

GLfloat projectileVertices[] = {
    -3.0, 3.0, 0.0, // bottom left corner
    -3.0, -3.0, 0.0, // top left corner
    3.0,-3.0, 0.0, // top right corner
    3.0,3.0, 0.0}; // bottom right corner

GLubyte indices[] = {0,1,2, // first triangle (bottom left - top left - top right)
                     0,2,3};

Projectile::Projectile(glm::vec3 start) : WorldObject(start, Settings::playerScale, Settings::playerRotation), speed(Settings::projectileSpeed)
{
    if (!load()) {
        std::cerr << "could not load projectile" << std::endl;
    }
}

Projectile::~Projectile()
{
    //dtor

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

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) , indices, GL_STATIC_DRAW);

    return true;
}

