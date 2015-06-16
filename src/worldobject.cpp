#include "worldobject.h"

WorldObject::WorldObject() : state(ALIVE), heightMesh(-1), widthMesh(-1)
{
    //ctor
}

WorldObject::WorldObject(glm::vec3 start, glm::vec3 scl, glm::vec3 rot) : pos(start), scale(scl), rotation(rot), state(ALIVE)
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

void WorldObject::setBoundingBox(glm::vec4 boundingBox)
{
    this->boundingBox = boundingBox;
}

glm::vec4 WorldObject::getBoundingBox()
{
    return boundingBox;
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

OBJECT_STATE WorldObject::getState()
{
    return state;
}

void WorldObject::setState(OBJECT_STATE state)
{
    this->state = state;
}

bool WorldObject::isAlive()
{
    return state == ALIVE;
}

void WorldObject::setWidth(float widthMesh)
{
    this->widthMesh = widthMesh;
}

float WorldObject::getWidth()
{
    return widthMesh;
}

void WorldObject::setHeight(float heightMesh)
{
    this->heightMesh = heightMesh;
}

float WorldObject::getHeight()
{
    return heightMesh;
}

glm::mat4 WorldObject::getModel()
{
    glm::mat4 model;
    model = glm::rotate(glm::translate(model, pos), rotation.y, glm::vec3(0.0, 1.0, 0.0));
    model = glm::rotate(model, rotation.z, glm::vec3(1.0, 0.0, 0.0));   // this is because we did a rotation before xD
    model = glm::scale(model, scale);
    return model;
}

