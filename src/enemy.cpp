#include "enemy.h"

Enemy::Enemy(glm::vec3 enemyStart, glm::vec3 enemyScale, glm::vec3 enemyRotation, glm::vec3 enemyAcc, glm::vec3 enemySpeed, std::string enemyModel) :
    AnimatedObject(enemyStart, enemyScale, enemyRotation, true, 0, Settings::enemyPath, Settings::enemyNumFrames), boundingBox(0.0,0.0,0.0,0.0)
{
    this->acc = enemyAcc;
    this->speed = enemySpeed;
    this->startPos = enemyStart;
}

Enemy::~Enemy()
{
    //dtor
}

void Enemy::setSpeed(glm::vec3 speed)
{
    this->speed = speed;
}

glm::vec3 Enemy::getSpeed()
{
    return speed;
}

void Enemy::addAcc(glm::vec3 acc)
{
    this->acc.y = acc.y;
    this->speed = Settings::playerSpeed;
}

glm::vec3 Enemy::getAcc()
{
    return acc;
}

void Enemy::setInitAcc(glm::vec3 acc)
{
    this->initAcc.y = acc.y;
}

glm::vec3 Enemy::getInitAcc()
{
    return initAcc;
}

void Enemy::setHealth(int hp)
{
    this->hp = hp;
}

int Enemy::getHealth()
{
    return hp;
}

void Enemy::setAngle(float angle)
{
    this->angle = angle;
}

float Enemy::getAngle()
{
    return angle;
}

void Enemy::setBoundingBox(glm::vec4 boundingBox)
{
    this->boundingBox = boundingBox;
}

glm::vec4 Enemy::getBoundingBox()
{
    return boundingBox;
}

/**
  * Calculates enemy's initial acceleration
  */
void Enemy::start(glm::vec3 playerPos)
{
    if(!isAlive())
    {
        setState(ALIVE);
        angle = 0.5 * M_PI;

        // Set a new y position for the enemy, based on the player's position
        pos.y = playerPos.y + (rand()%180 / 100.0);
        startPos.y = pos.y;

        if(startPos.y > playerPos.y)
            down = true;
        else
            down = false;

        startAnimation();
    }
}

/**
 * Update enemy position, speed and rotation (TODO).
 */
void Enemy::update()
{
    if(isAlive())
    {
        //angle += 0.02;
        angle += speed.x / 3.0;

        pos += speed;

        if(down)
            pos.y = (float)cos(angle)*2 + startPos.y;
        else
            pos.y = -(float)cos(angle)*2 + startPos.y;
    }
}

void Enemy::destroyObject()
{
    //delete this;
}
