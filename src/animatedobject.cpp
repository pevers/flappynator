#include "animatedobject.h"
#include <sstream>
AnimatedObject::AnimatedObject(glm::vec3 start, glm::vec3 scl, glm::vec3 rot, bool loop, unsigned int startFrame) : WorldObject(start, scl, rot), frameRate(100), startFrame(startFrame), frame(startFrame), loop(loop)
{
    if (!load()) {
        std::cerr << "could not load animations" << std::endl;
    }
    else {
        getDistances();
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

void AnimatedObject::getDistances() {
        //calculate max and min coordinate
    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::min();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::min();

    for (size_t i = 0; i < 1; i++) {
        for (size_t v = 0; v < frames[frame][0].mesh.positions.size() / 3; v++) {
                if(minX > frames[frame][0].mesh.positions[3*v+2])
                    minX = frames[frame][0].mesh.positions[3*v+2];
                if(maxX < frames[frame][0].mesh.positions[3*v+2])
                    maxX = frames[frame][0].mesh.positions[3*v+2];
                if(minY > frames[frame][0].mesh.positions[3*v+1])
                    minY = frames[frame][0].mesh.positions[3*v+1];
                if(maxY < frames[frame][0].mesh.positions[3*v+1])
                    maxY = frames[frame][0].mesh.positions[3*v+1];
                 //std::cerr << "LOADOBJECT" <<  shapes[i].mesh.positions[3*v+0] << "  " << shapes[i].mesh.positions[3*v+1] << "  " <<  shapes[i].mesh.positions[3*v+2]<< std::endl;
        }
    }

    widthMesh = std::abs(maxX - minX);
    heightMesh = std::abs(maxY - minY);
    std::cerr << "minX: " <<  minX << " maxX: " << maxX << " minY: " <<  minY << " maxY: " << maxY << std::endl;
    std::cerr << "widthMesh: " <<  widthMesh << " heightMesh: " << heightMesh << std::endl;
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

void AnimatedObject::setWidth(float widthMesh)
{
    this->widthMesh = widthMesh;
}

float AnimatedObject::getWidth()
{
    return widthMesh;
}

void AnimatedObject::setHeight(float heightMesh)
{
    this->heightMesh = heightMesh;
}

float AnimatedObject::getHeight()
{
    return heightMesh;
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
