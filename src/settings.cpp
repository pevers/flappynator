#include "settings.h"

namespace Settings
{
    int screenWidth = 800;
    int screenHeight = 600;
    int bitsPerPixel = 32;

    sf::ContextSettings context(24, 8, 2);
    std::string windowTitle = "FlappyNator";

    glm::vec3 eye(0.0, 2.0, 15.0);
}
