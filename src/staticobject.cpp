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

bool StaticObject::generateBuffers()
{
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

bool StaticObject::load()
{
    std::string err = tinyobj::LoadObj(shapes, material, objPath.c_str());
    if (err.length()) {
        std::cerr << err << std::endl;
        return false;
    }

    return generateBuffers();
}

int StaticObject::getObjectSize()
{
    return shapes[0].mesh.indices.size();
}

// ugly hack
/*void StaticObject::disintegrate()
{
    // test, remove one of the vertices each tick
    srand(time(NULL));

    if (!shapes[0].mesh.indices.size() || !shapes[0].mesh.positions.size()) {
        destroying = false;
        return;
    }

    int vi = rand() % shapes[0].mesh.indices.size();
    int v = shapes[0].mesh.indices[vi];

    // set next triangle's first vertex as the new connection
    int n;
    if (shapes[0].mesh.indices.size() > 3)
        n = shapes[0].mesh.indices[vi - (vi % 3) + 3];
    else
        n = 0;

    // get the three coordinates
    //float x = shapes[0].mesh.positions[v - (v % 3)];
    //float y = shapes[0].mesh.positions[v - (v % 3) + 1];
    //float z = shapes[0].mesh.positions[v - (v % 3) + 2];

    // remove the vertex
    shapes[0].mesh.positions.erase(shapes[0].mesh.positions.begin() + v - (v % 3));
    shapes[0].mesh.positions.erase(shapes[0].mesh.positions.begin() + v - (v % 3));
    shapes[0].mesh.positions.erase(shapes[0].mesh.positions.begin() + v - (v % 3));

    // remove normal
    shapes[0].mesh.normals.erase(shapes[0].mesh.normals.begin() + v - (v % 3));
    shapes[0].mesh.normals.erase(shapes[0].mesh.normals.begin() + v - (v % 3));
    shapes[0].mesh.normals.erase(shapes[0].mesh.normals.begin() + v - (v % 3));

    // remove index
    shapes[0].mesh.indices.erase(shapes[0].mesh.indices.begin() + vi - (vi % 3));
    shapes[0].mesh.indices.erase(shapes[0].mesh.indices.begin() + vi - (vi % 3));
    shapes[0].mesh.indices.erase(shapes[0].mesh.indices.begin() + vi - (vi % 3));

    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        if (shapes[0].mesh.indices[i] == v)
            shapes[0].mesh.indices[i] = n;

    }

    scale = glm::vec3(scale.x - 1.0/100, scale.y - 1.0/100, scale.z - 1.0/100);
    rotation = glm::vec3(rotation.x + 1.0/10, rotation.y + 1.0/10, rotation.z + 1.0/10);

    // generate buffer
    generateBuffers();

}*/

void StaticObject::destroyObject()
{

}

void StaticObject::update()
{
    // no update
}

