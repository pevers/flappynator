#include "sun.h"

Sun::Sun(glm::vec3 color, glm::vec3 pos, glm::vec3 spot, float intensity) : pos(pos), color(color), direction(direction), spot(spot), intensity(intensity)
{
    direction = glm::vec3(glm::normalize(spot - pos).x, glm::normalize(spot - pos).y, glm::normalize(spot - pos).z);
}

Sun::~Sun()
{
    //dtor
}

void Sun::setPos(glm::vec3 pos) {
    this->pos = pos;
}

glm::vec3 Sun::getPos() {
    return pos;
}

void Sun::increaseIntensity(float intensity) {
    static bool skip = false;
    static int ticks = 100;

    // stay dark for some ticks
    if (getIntensity() < 0.15 && !skip) {
        skip = true;
        ticks = 100;
    } else if (getIntensity() > 0.15 && skip) {
        skip = false;
    }

    if (skip && ticks) {
        ticks--;
    }
    else
        this->intensity += intensity;
}

void Sun::setIntensity(float intensity) {
    this->intensity = intensity;
}

float Sun::getIntensity() {
    return std::min(std::abs(sin(intensity)) + 0.1, 0.8);
}

glm::vec3 Sun::getDirection() {
    return direction;
}

glm::vec3 Sun::getColor() {
    return color;
}

void Sun::setColor(glm::vec3 color) {
    this->color = color;
}

glm::vec3 Sun::getSpot() {
    return spot;
}

void Sun::setSpot(glm::vec3 spot) {
    this->spot = spot;
}
