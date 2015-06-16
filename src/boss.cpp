#include "boss.h"

Boss::Boss(glm::vec3 bossStart, glm::vec3 bossScale, glm::vec3 bossRotation) :
    AnimatedObject(bossStart, bossScale, bossRotation, true, 1, Settings::bossPath, Settings::bossNumFrames), boundingBox(0.0,0.0,0.0,0.0)
{

}

Boss::~Boss()
{
    //dtor
}

void Boss::setBoundingBox(glm::vec4 boundingBox)
{
    this->boundingBox = boundingBox;
}

glm::vec4 Boss::getBoundingBox()
{
    return boundingBox;
}

void Boss::start()
{

}

void Boss::update()
{

}
