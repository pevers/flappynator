#ifndef ANIMATEDOBJECT_H
#define ANIMATEDOBJECT_H

#include "worldobject.h"
#include "../src/util/tiny_obj_loader.h"
#include <SFML/Window.hpp>

class AnimatedObject : public WorldObject
{
    public:
        AnimatedObject(glm::vec3 start, glm::vec3 scl, glm::vec3 rot, bool loop, unsigned int startFrame);
        virtual ~AnimatedObject();

        void setStartFrame(unsigned int frame);
        unsigned int getStartFrame();

        void setLoop(bool loop);
        bool isLooped();

        void startAnimation();
        unsigned int nextFrame(unsigned int f);

        virtual int getObjectSize();
        virtual void update();
        virtual void free();
        virtual bool load();
        virtual void destroyObject();
    private:
        void setFrame(int frame);

        float frameRate;
        unsigned int startFrame;
        unsigned int frame;
        bool loop;

        sf::Clock clock;

        std::vector<std::vector<tinyobj::shape_t>> frames;
        std::vector<std::vector<tinyobj::material_t>> materials;
};

#endif // ANIMATEDOBJECT_H
