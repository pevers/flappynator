#include "boss.h"

Boss::Boss(glm::vec3 bossStart, glm::vec3 bossScale, glm::vec3 bossRotation) :
    AnimatedObject(bossStart, bossScale, bossRotation, true, 0, Settings::bossPath, Settings::bossNumFrames), boundingBox(0.0,0.0,0.0,0.0)
{
    addAnimation(DYING, false, 0, 25, "resources/animation/boss/explode/");
}

Boss::~Boss()
{
    //dtor
}

bool Boss::isAlive()
{
    return alive;
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
    angle = 0.0;
    alive = true;
}

void Boss::update()
{
    AnimatedObject::update();

    angle += 0.04;
    pos.y = (float)cos(angle)*2.5 + Settings::bossStart.y;
}
