#include "animatedobject.h"

AnimatedObject::AnimatedObject(glm::vec3 start, glm::vec3 scl, glm::vec3 rot, bool loop, unsigned int startFrame, unsigned int frameSize, std::string basePath) :
    WorldObject(start, scl, rot), activeAnimation(ALIVE)
{
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &elementBuffer);
    glGenBuffers(1, &normalBuffer);

    animations.resize(OBJECT_STATE_SIZE);

    if (!addAnimation(ALIVE, loop, startFrame, frameSize, basePath)) {
        std::cerr << "could not load animations" << std::endl;
    } else {
        // get the size of the first animation as bounding box
        animations[ALIVE].bindBuffer(vbo, elementBuffer, normalBuffer);
        animations[ALIVE].getDistances(widthMesh, heightMesh);
    }
}

AnimatedObject::~AnimatedObject()
{

}

bool AnimatedObject::load()
{
    return true;
}

bool AnimatedObject::addAnimation(OBJECT_STATE state, bool loop, unsigned int startFrame, unsigned int frameSize, std::string basePath)
{
    animations[state] = Animation(loop, startFrame, frameSize, basePath);
    return true;
}

void AnimatedObject::setAnimationState(OBJECT_STATE state)
{
    if (state > animations.size() - 1) {
        std::cerr << "warning, could not set animation state to " << state << ", doesn't exist" << std::endl;
    } else activeAnimation = state;
}

void AnimatedObject::startAnimation()
{
    animations[activeAnimation].startAnimation();
}

void AnimatedObject::free()
{
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &elementBuffer);
    glDeleteBuffers(1, &normalBuffer);
}

int AnimatedObject::getObjectSize()
{
    return animations[activeAnimation].getObjectSize();
}

void AnimatedObject::destroyObject()
{
    // empty
}

void AnimatedObject::update()
{
    animations[activeAnimation].update(vbo, elementBuffer, normalBuffer);   // we use activeAnimation as state, could be 1-1 with WorldObject::state
}
