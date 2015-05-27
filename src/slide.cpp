#include "slide.h"

/**
 * Create new sliding window.
 * @param eye   eye position
 * @param center axis to move the sliding window
 * @param speed amount of movement each frame
 */
Slide::Slide(glm::vec3 eye, glm::vec3 center, double speed)
{
    this->eye = eye;
    this->center = center;
    this->speed = speed;
}

Slide::~Slide()
{
    //dtor
}

glm::mat4 Slide::getSlideView()
{
    return glm::lookAt(
        eye,
        center,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
}

glm::vec3 Slide::getEye()
{
    return eye;
}

void Slide::setEye(glm::vec3 eye)
{
    this->eye = eye;
}

glm::vec3 Slide::getCenter()
{
    return center;
}

void Slide::setCenter(glm::vec3 center)
{
    this->center = center;
}

double Slide::getSpeed()
{
    return speed;
}

void Slide::setSpeed(double speed)
{
    this->speed = speed;
}
