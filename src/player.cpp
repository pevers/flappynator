#include "player.h"

/**
 * Create new player.
 */

Player::Player(double speedForward)
{
    this->speedForward = speedForward;
}

Player::~Player()
{

}

void Player::setSpeedForward(double speedForward)
{
    this->speedForward = speedForward;
}

double Player::getSpeedForward()
{
    return speedForward;
}
