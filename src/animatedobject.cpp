#include "animatedobject.h"

AnimatedObject::AnimatedObject(glm::vec3 start, glm::vec3 scl, glm::vec3 rot, bool loop, unsigned int startFrame, std::string basePath, unsigned int frameSize) :
    WorldObject(start, scl, rot), activeAnimation(ALIVE)
{
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &elementBuffer);
    glGenBuffers(1, &normalBuffer);
    glGenBuffers(1, &vboTextureBuffer);

    if (!addAnimation(ALIVE, loop, startFrame, frameSize, basePath)) {
        std::cerr << "could not load animations" << std::endl;
    } else {
        // get the size of the first animation as bounding box
        animations[ALIVE]->bindBuffer(vbo, elementBuffer, normalBuffer, vboTextureBuffer);
        animations[ALIVE]->getDistances(widthMesh, heightMesh);
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
    animations[state] = std::move(std::unique_ptr<Animation>(new Animation(loop, startFrame, frameSize, basePath)));
    return true;
}

void AnimatedObject::setAnimationState(OBJECT_STATE state)
{
    if (state >= OBJECT_STATE_SIZE || state < 0) {
        std::cerr << "warning, could not set animation state to " << state << ", doesn't exist" << std::endl;
    } else activeAnimation = state;
}

void AnimatedObject::startAnimation()
{
    animations[activeAnimation]->startAnimation();
}

void AnimatedObject::free()
{
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &elementBuffer);
    glDeleteBuffers(1, &normalBuffer);
    glDeleteBuffers(1, &vboTextureBuffer);
}

int AnimatedObject::getObjectSize()
{
    return animations[activeAnimation]->getObjectSize();
}

void AnimatedObject::destroyObject()
{
    // empty
}

GLuint AnimatedObject::getTexture() {
    return animations[activeAnimation]->getTexture();
} // HACK

void AnimatedObject::changeTexture() {
    animations[activeAnimation]->changeTexture();
}

void AnimatedObject::update()
{
    animations[activeAnimation]->update(vbo, elementBuffer, normalBuffer, vboTextureBuffer);   // we use activeAnimation as state, could be 1-1 with WorldObject::state
}
