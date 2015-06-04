#ifndef WORLDOBJECT_H
#define WORLDOBJECT_H

#include <string>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "../src/util/tiny_obj_loader.h"

class WorldObject
{
    protected:
        GLuint vbo; // vertex buffer
        GLuint elementBuffer;
        GLuint normalBuffer;

        glm::vec3 pos;
        glm::vec3 scale;
        float rotation;

        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> material;
    public:
        WorldObject();
        WorldObject(glm::vec3 start, glm::vec3 scl, float rot, const std::string &path);
        virtual ~WorldObject();

        bool loadObject(const std::string &path);
        unsigned int getObjectSize();

        GLuint &getVertexBuffer();
        GLuint &getElementBuffer();
        GLuint getNormalBuffer();

        void setPos(glm::vec3 pos);
        glm::vec3 getPos();

        void setScale(glm::vec3 scale);
        glm::vec3 getScale();

        void setRotation(float rotation);
        float getRotation();
};

#endif // WORLDOBJECT_H
