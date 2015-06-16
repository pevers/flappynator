#ifndef ANIMATION_H
#define ANIMATION_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <SFML/Window.hpp>

#include <vector>
#include <iostream>

#include "../src/util/tiny_obj_loader.h"

class Animation
{
    public:
        Animation();
        Animation(bool loop, unsigned int startFrame, unsigned int numFrames, std::string basePath);
        virtual ~Animation();

        void getDistances(float &width, float &height);
        unsigned int getObjectSize();

        void setStartFrame(unsigned int frame);
        unsigned int getStartFrame();

        void setLoop(bool loop);
        bool isLooped();

        void startAnimation();
        unsigned int nextFrame(unsigned int f);

        void bindBuffer(GLuint &vbo, GLuint &elementBuffer, GLuint &normalBuffer);
        void update(GLuint &vbo, GLuint &elementBuffer, GLuint &normalBuffer);
        bool load();

    private:

        std::vector<std::vector<tinyobj::shape_t>> frames;
        std::vector<std::vector<tinyobj::material_t>> materials;

        std::string basePath;
        sf::Clock clock;

        bool loop;
        unsigned int startFrame;
        unsigned int numFrames;

        float frameRate;
        unsigned int frame;

};

#endif // ANIMATION_H
