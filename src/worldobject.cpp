#include "worldobject.h"

WorldObject::WorldObject()
{
    //ctor
}

WorldObject::WorldObject(glm::vec3 start, glm::vec3 scl, glm::vec3 rot, const std::string &path) : pos(start), scale(scl), rotation(rot)
{
    if (!loadObject(path)) {
        std::cerr << "warning, could not add object " << path << std::endl;
    }
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

bool WorldObject::loadObject(const std::string &path)
{
    std::string err = tinyobj::LoadObj(shapes, material, path.c_str());
    if (err.length()) {
        std::cerr << err << std::endl;
        return false;
    }

    // succesfully loaded, load data into VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.positions.size() * sizeof(float), &shapes[0].mesh.positions[0], GL_STATIC_DRAW);

    // create index buffer
    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[0].mesh.indices.size() * sizeof(unsigned int), &shapes[0].mesh.indices[0], GL_STATIC_DRAW);

    if (shapes[0].mesh.normals.size() > 0) {
        // create normal buffer
        glGenBuffers(1, &normalBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.normals.size() * sizeof(float), &shapes[0].mesh.normals[0], GL_STATIC_DRAW);
    } else {
        std::cout << "warning, no normals for object " << path << std::endl;
    }

    if (!elementBuffer || !vbo || !normalBuffer)
    {
        std::cerr << "error generating buffers" << std::endl;
        return false;
    }

    return true;
}

unsigned int WorldObject::getObjectSize()
{
    return shapes[0].mesh.indices.size();
}



