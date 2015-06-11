#include "enemy.h"

Enemy::Enemy(glm::vec3 enemyStart, glm::vec3 enemyScale, glm::vec3 enemyRotation, glm::vec3 enemyAcc, glm::vec3 enemySpeed, std::string enemyModel) :
    WorldObject(enemyStart, enemyScale, enemyRotation, enemyModel)
{
    this->acc = enemyAcc;
    this->speed = enemySpeed;
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

/**
 * Update enemy position, speed and rotation.
 */
void Enemy::update()
{
    // acceleration
    acc += Settings::enemyAcc;
    if (acc.y < Settings::enemyAcc.y)
        acc.y = Settings::enemyAcc.y;

    speed.y += acc.y;
    pos += speed;

    if (speed.y > 0) {
        rotation.z = -acos(glm::dot(glm::normalize(speed), glm::normalize(glm::vec3(1.0, 0.0, 0.0))));
    } else {
        rotation.z = acos(glm::dot(glm::normalize(speed), glm::normalize(glm::vec3(1.0, 0.0, 0.0))));
    }
}

/**
 * Calculates the acceleration so that the the enemy hits the player on his current position
 */
void Enemy::calcPathToPlayer()
{

}
