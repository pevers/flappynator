#ifndef STATICOBJECT_H
#define STATICOBJECT_H

#include <iostream>
#include <SFML/Window.hpp>
#include "worldobject.h"

class StaticObject : public WorldObject
{
    public:
        StaticObject(glm::vec3 start, glm::vec3 scl, glm::vec3 rot, const std::string &path);
        virtual ~StaticObject();

        virtual void free();
        virtual bool load();
        virtual int getObjectSize();
        virtual void update();
        virtual void destroyObject();
        virtual GLuint getTexture(); // HACK
    private:
        bool generateBuffers();

        std::string objPath;

        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> material;

        //void disintegrate();
};

#endif // STATICOBJECT_H
