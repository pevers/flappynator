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

/**
  * Calculates enemy's initial acceleration
  */
void Enemy::start(glm::vec3 playerPos)
{
    if(!alive)
    {
        alive = true;

        // Calculate the amount of time it takes the enemy to get to the x position of the player
        float time = Settings::startEnemyUpdate / (abs(speed.x) * 60.0);
        float frames = time * 60.0;
        float enemyAcc = ((pos.y - playerPos.y) / pow(frames, 2.0)) * -1.0;

        initAcc = glm::vec3(0.0, enemyAcc, 0.0);
        acc = initAcc;
    }
}

/**
 * Update enemy position, speed and rotation.
 */
void Enemy::update()
{
    if(alive)
    {
        pos += speed;
        pos.y = -(float)cos(pos.x / 2) + 3;
        // acceleration
        /*acc += initAcc;
        if (acc.y < initAcc.y)
            acc.y = initAcc.y;

        speed.y += acc.y;
        pos += speed;

        if (speed.y > 0) {
            rotation.z = -acos(glm::dot(glm::normalize(speed), glm::normalize(glm::vec3(1.0, 0.0, 0.0))));
        } else {
            rotation.z = acos(glm::dot(glm::normalize(speed), glm::normalize(glm::vec3(1.0, 0.0, 0.0))));
        }*/
    }
}

/**
 * Calculates the acceleration so that the the enemy hits the player on his current position
 */
void Enemy::calcPathToPlayer()
{

}
