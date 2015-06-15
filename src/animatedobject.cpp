#include "animatedobject.h"

AnimatedObject::AnimatedObject(glm::vec3 start, glm::vec3 scl, glm::vec3 rot, bool loop, unsigned int startFrame) : WorldObject(start, scl, rot), frameRate(100), startFrame(startFrame), frame(startFrame), loop(loop)
{
    if (!load()) {
        std::cerr << "could not load animations" << std::endl;
    }

    clock.restart();
}

AnimatedObject::~AnimatedObject()
{

}

bool AnimatedObject::load()
{
    std::string basePath = "resources/animation/";

    frames.resize(45);
    materials.resize(45);

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &elementBuffer);
    glGenBuffers(1, &normalBuffer);

    // debug, load all objects
    setNormalsExist(true);
    for (int i = 0; i < 45; i++) {
        std::string path = basePath + std::to_string(i+1) + ".obj";
        std::string err = tinyobj::LoadObj(frames[i], materials[i], path.c_str());
        if (err.length()) {
            std::cerr << err << std::endl;
            return false;
        }

        if (!frames[i][0].mesh.normals.size()) {
            std::cout << "warning, no normals in object " << std::endl;
            setNormalsExist(false);
        }
    }

    // set active frame
    setFrame(0);

    return true;
}

void AnimatedObject::setFrame(int frame)
{
    this->frame = frame;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, frames[frame][0].mesh.positions.size() * sizeof(float), &frames[frame][0].mesh.positions[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, frames[frame][0].mesh.indices.size() * sizeof(unsigned int), &frames[frame][0].mesh.indices[0], GL_STATIC_DRAW);

    if (frames[frame][0].mesh.normals.size() > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glBufferData(GL_ARRAY_BUFFER, frames[frame][0].mesh.normals.size() * sizeof(float), &frames[frame][0].mesh.normals[0], GL_STATIC_DRAW);
    }
}

void AnimatedObject::setStartFrame(unsigned int frame)
{
    startFrame = frame;
}

unsigned int AnimatedObject::getStartFrame()
{
    return startFrame;
}

void AnimatedObject::setLoop(bool loop)
{
    this->loop = loop;
}

bool AnimatedObject::isLooped()
{
    return loop;
}

void AnimatedObject::startAnimation()
{
    // reset animation
    frame = startFrame;
    frame = nextFrame(frame);
}

void AnimatedObject::free()
{
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &elementBuffer);
    glDeleteBuffers(1, &normalBuffer);
}

int AnimatedObject::getObjectSize()
{
    return frames[frame][0].mesh.indices.size();
}

unsigned int AnimatedObject::nextFrame(unsigned int f)
{
    f++;
    if (f > (frames.size() - 1))
        return 0;
    return f;
}

void AnimatedObject::destroyObject()
{
    // empty
}

void AnimatedObject::update()
{
    // interpolate frames if animation is still busy
    if (frame != startFrame || loop) {
        sf::Time elapsed = clock.getElapsedTime();
        unsigned int newFrame = frame;
        if (elapsed.asMilliseconds() > (1000.0 / frameRate)) {
            // update frame
            newFrame = nextFrame(newFrame);
            clock.restart();
        }

        if (newFrame != frame) {
            setFrame(newFrame);
        }
    }
}
