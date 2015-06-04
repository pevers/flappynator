#include "physics.h"

/**
 * Use physics.
 */

Physics::Physics()
{
    this->g = 0.0000001f;
    this->max_accel = 0.001f;
}

Physics::~Physics()
{

}

float Physics::force(float m1, float m2, float distance)
{
    return g * (m1 * m2) / (distance * distance);
}

float Physics::distance(glm::vec3 p1, glm::vec3 p2)
{
    return sqrt(pow(std::abs(p1.x - p2.x), 2) + pow(std::abs(p1.y - p2.y), 2));
}

float Physics::getMaxAcceleration()
{
    return max_accel;
}
