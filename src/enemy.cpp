#include "enemy.h"

Enemy::Enemy(glm::vec3 enemyStart, glm::vec3 enemyScale, glm::vec3 enemyRotation, glm::vec3 enemyAcc, glm::vec3 enemySpeed, std::string enemyModel) :
    StaticObject(enemyStart, enemyScale, enemyRotation, enemyModel)
{
    this->acc = enemyAcc;
    this->speed = enemySpeed;
    this->startPos = enemyStart;
    this->alive = false;
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

bool Enemy::isAlive()
{
    return alive;
}

/**
  * Calculates enemy's initial acceleration
  */
void Enemy::start(glm::vec3 playerPos)
{
    if(!alive)
    {
        alive = true;
        angle = 0.0;

        // Set a new y position for the enemy, based on the player's position
        pos.y = playerPos.y + rand()%4;
        startPos.y = pos.y;

        if(startPos.y > playerPos.y)
            down = true;
        else
            down = false;
    }
}

/**
 * Update enemy position, speed and rotation.
 */
void Enemy::update()
{
    if(alive)
    {
        //angle += 0.02;
        angle += speed.x / 5.0;

        pos += speed;

        if(down)
            pos.y = (float)cos(angle)*3 + startPos.y;
        else
            pos.y = -(float)cos(angle)*3 + startPos.y;
    }
}
