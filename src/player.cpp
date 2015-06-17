#include "player.h"

Player::Player() :
    AnimatedObject(Settings::playerStart, Settings::playerScale, Settings::playerRotation, false, 14, Settings::playerPath, Settings::playerNumFrames),
                   acc(Settings::playerAcc), speed(Settings::playerSpeed)
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
    this->speed.y = Settings::playerSpeed.y;
}

glm::vec3 Player::getAcc()
{
    return acc;
}

/**
 * Update player position, speed and rotation.
 */
void Player::update(GameState gameState)
{
    // update animation
    AnimatedObject::update();

    // acceleration
    acc += Settings::playerAcc;
    if (acc.y < Settings::playerAcc.y)
        acc.y = Settings::playerAcc.y;

    speed.y += acc.y;

    // Put this here for now, should be done in the engine..
    if(gameState.currentState == GameState::ST_BOSS)
    {
        pos.y += speed.y;
        pos.z += speed.z;
    }
    else
        pos += speed;

    if (speed.y > 0) {
        rotation.z = -acos(glm::dot(glm::normalize(speed), glm::normalize(glm::vec3(1.0, 0.0, 0.0))));
    } else {
        rotation.z = acos(glm::dot(glm::normalize(speed), glm::normalize(glm::vec3(1.0, 0.0, 0.0))));
    }
}
