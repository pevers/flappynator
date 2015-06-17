#ifndef ANIMATEDOBJECT_H
#define ANIMATEDOBJECT_H

#include <map>
#include <SFML/Window.hpp>

#include "worldobject.h"
#include "animation.h"

class AnimatedObject : public WorldObject
{
    public:
        AnimatedObject(glm::vec3 start, glm::vec3 scl, glm::vec3 rot, bool loop, unsigned int startFrame, std::string basePath, unsigned int frameSize);
        virtual ~AnimatedObject();

        virtual int getObjectSize();
        virtual void update();
        virtual void free();
        virtual bool load();
        virtual void destroyObject();
        virtual GLuint getTexture();

        bool addAnimation(OBJECT_STATE state, bool loop, unsigned int startFrame, unsigned int frameSize, std::string basePath);
        void setAnimationState(OBJECT_STATE state);
        void startAnimation();
    private:
        Animation *animations[OBJECT_STATE_SIZE];
        OBJECT_STATE activeAnimation;
};

#endif // ANIMATEDOBJECT_H
