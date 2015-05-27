#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
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
}

#endif // SETTINGS_H
