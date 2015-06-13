#include "staticobject.h"

StaticObject::StaticObject(glm::vec3 start, glm::vec3 scl, glm::vec3 rot, const std::string &path) : WorldObject(start, scl, rot), objPath(path)
{
    if (!load()) {
        std::cerr << "could not load static object " << path << std::endl;
    }
}

StaticObject::~StaticObject()
{
    //dtor
}

void StaticObject::free()
{
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &elementBuffer);
    glDeleteBuffers(1, &normalBuffer);
}

bool StaticObject::load()
{
    std::string err = tinyobj::LoadObj(shapes, material, objPath.c_str());
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

        setNormalsExist(true);
    } else {
        std::cout << "warning, no normals for object " << objPath << std::endl;
    }

    if (!elementBuffer || !vbo || !normalBuffer)
    {
        std::cerr << "error generating buffers" << std::endl;
        return false;
    }

    return true;
}

int StaticObject::getObjectSize()
{
    return shapes[0].mesh.indices.size();
}

void StaticObject::update()
{
    // do nothing
}

