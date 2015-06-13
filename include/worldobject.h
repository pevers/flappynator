#define GLEW_STATIC

#ifndef WORLDOBJECT_H
#define WORLDOBJECT_H

#include <string>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../src/util/tiny_obj_loader.h"

class WorldObject
{
    protected:
        GLuint vbo; // vertex buffer
        GLuint elementBuffer;
        GLuint normalBuffer;

        glm::vec3 pos;
        glm::vec3 scale;
        glm::vec3 rotation;

        bool normalsExist;
    public:
        WorldObject();
        WorldObject(glm::vec3 start, glm::vec3 scl, glm::vec3 rot);
        virtual ~WorldObject();

        GLuint &getVertexBuffer();
        GLuint &getElementBuffer();
        GLuint &getNormalBuffer();

        virtual bool load() = 0;
        virtual void free() = 0;
        virtual int getObjectSize() = 0;
        virtual void update() = 0;

        void setPos(glm::vec3 pos);
        glm::vec3 getPos();

        void setScale(glm::vec3 scale);
        glm::vec3 getScale();

        void setRotation(glm::vec3 rotation);
        glm::vec3 getRotation();

        glm::mat4 getModel();

        bool hasNormals();
        void setNormalsExist(bool exist);
};

#endif // WORLDOBJECT_H
