#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <math.h>
#include <glm/glm.hpp>
#include <SFML/Window.hpp>

namespace Settings
{
    extern int screenWidth;
    extern int screenHeight;
    extern int depthBits;

    extern sf::ContextSettings context;

    extern std::string windowTitle;

    extern glm::vec3 eye;

    extern glm::vec3 playerStart;
    extern glm::vec3 playerScale;
    extern float playerRotation;

    extern glm::vec3 sunPos;
}

#endif // SETTINGS_H
