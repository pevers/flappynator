#include "worldobject.h"

WorldObject::WorldObject()
{
    //ctor
}

WorldObject::WorldObject(glm::vec3 start, glm::vec3 scl, glm::vec3 rot, const std::string &path) : pos(start), scale(scl), rotation(rot), boundingBox(0.0,0.0,0.0,0.0)
{
    if (!loadObject(path)) {
        std::cerr << "warning, could not add object " << path << std::endl;
    }
    //calculate max and min coordinate
    else {
        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::min();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::min();

        for (size_t i = 0; i < 1; i++) {
            for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
                    if(minX > shapes[i].mesh.positions[3*v+2])
                        minX = shapes[i].mesh.positions[3*v+2];
                    if(maxX < shapes[i].mesh.positions[3*v+2])
                        maxX = shapes[i].mesh.positions[3*v+2];
                    if(minY > shapes[i].mesh.positions[3*v+1])
                        minY = shapes[i].mesh.positions[3*v+1];
                    if(maxY < shapes[i].mesh.positions[3*v+1])
                        maxY = shapes[i].mesh.positions[3*v+1];
                     //std::cerr << "LOADOBJECT" <<  shapes[i].mesh.positions[3*v+0] << "  " << shapes[i].mesh.positions[3*v+1] << "  " <<  shapes[i].mesh.positions[3*v+2]<< std::endl;
            }
        }

        widthMesh = std::abs(maxX - minX);
        heightMesh = std::abs(maxY - minY);
        std::cerr << "minX: " <<  minX << " maxX: " << maxX << " minY: " <<  minY << " maxY: " << maxY << std::endl;
        std::cerr << "widthMesh: " <<  widthMesh << " heightMesh: " << heightMesh << std::endl;

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

glm::mat4 WorldObject::getModel()
{
    glm::mat4 model;
    model = glm::rotate(glm::translate(model, pos), rotation.y, glm::vec3(0.0, 1.0, 0.0));
    model = glm::rotate(model, rotation.z, glm::vec3(1.0, 0.0, 0.0));   // this is because we did a rotation before xD
    model = glm::scale(model, scale);
    return model;
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
    //std::cerr << "LOADOBJECT" << shapes[1].mesh.texcoords[1] << std::endl;

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



