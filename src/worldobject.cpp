#include "worldobject.h"

WorldObject::WorldObject()
{
    //ctor
}

WorldObject::WorldObject(glm::vec3 start, glm::vec3 scl, glm::vec3 rot) : pos(start), scale(scl), rotation(rot), normalsExist(false)
{
}

WorldObject::~WorldObject()
{
}

GLuint &WorldObject::getVertexBuffer()
{
    return vbo;
}

GLuint &WorldObject::getElementBuffer()
{
    return elementBuffer;
}

GLuint &WorldObject::getNormalBuffer()
{
    return normalBuffer;
}

void WorldObject::setPos(glm::vec3 pos)
{
    this->pos = pos;
}

glm::vec3 WorldObject::getPos()
{
    return pos;
}

void WorldObject::setScale(glm::vec3 scale)
{
    this->scale = scale;
}

glm::vec3 WorldObject::getScale()
{
    return scale;
}

void WorldObject::setRotation(glm::vec3 rotation)
{
    this->rotation = rotation;
}

glm::vec3 WorldObject::getRotation()
{
    return rotation;
}

bool WorldObject::hasNormals()
{
    return normalsExist;
}

void WorldObject::setNormalsExist(bool exist)
{
    normalsExist = exist;
}

glm::mat4 WorldObject::getModel()
{
    glm::mat4 model;
    model = glm::rotate(glm::translate(model, pos), rotation.y, glm::vec3(0.0, 1.0, 0.0));
    model = glm::rotate(model, rotation.z, glm::vec3(1.0, 0.0, 0.0));   // this is because we did a rotation before xD
    model = glm::scale(model, scale);
    return model;
}

