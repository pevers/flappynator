#include "player.h"

Player::Player() : AnimatedObject(Settings::playerStart, Settings::playerScale, Settings::playerRotation, false, 0), acc(Settings::playerAcc), speed(Settings::playerSpeed)
{
    //ctor
}

Player::~Player()
{
    //dtor
}

void Player::setSpeed(glm::vec3 speed)
{
    this->speed = speed;
}

glm::vec3 Player::getSpeed()
{
    return speed;
}

void Player::addAcc(glm::vec3 acc)
{
    this->acc.y = acc.y;
    this->speed = Settings::playerSpeed;
}

glm::vec3 Player::getAcc()
{
    return acc;
}

/**
 * Update player position, speed and rotation.
 */
void Player::update()
{
    // update animation
    AnimatedObject::update();

    // acceleration
    acc += Settings::playerAcc;
    if (acc.y < Settings::playerAcc.y)
        acc.y = Settings::playerAcc.y;

    speed.y += acc.y;
    pos += speed;

    if (speed.y > 0) {
        rotation.z = -acos(glm::dot(glm::normalize(speed), glm::normalize(glm::vec3(1.0, 0.0, 0.0))));
    } else {
        rotation.z = acos(glm::dot(glm::normalize(speed), glm::normalize(glm::vec3(1.0, 0.0, 0.0))));
    }
}
