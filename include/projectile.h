#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "settings.h"
#include "worldobject.h"
#include "math.h"
#include <SOIL/SOIL.h>

class Projectile : public WorldObject
{
    public:
        Projectile(glm::vec3 start,glm::vec3 rotation);
        virtual ~Projectile();

        GLuint getTextureBuffer();
        std::vector<glm::vec2> texCoords;
        GLuint vboTexCoords;
        GLuint textures[1];

        void setSpeed(glm::vec3 speed);
        glm::vec3 getSpeed();
        bool genProjectile();
        glm::vec3 playerRotation;


        virtual int getObjectSize();
        virtual void update();
        virtual void free();
        virtual bool load();

        virtual void destroyObject();
        virtual GLuint getTexture(); // HACK

        void setBoundingBox(glm::vec4 boundingBox);
        glm::vec4 getBoundingBox();
    protected:
    private:
        glm::vec3 speed;
        glm::vec4 boundingBox;
};

#endif // PROJECTILE_H