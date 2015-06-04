#include "settings.h"
#define 	M_PI   3.14159265358979323846	/* pi */

namespace Settings
{
    int screenWidth = 800;
    int screenHeight = 600;
    int bitsPerPixel = 32;

    sf::ContextSettings context(24, 8, 2);
    std::string windowTitle = "FlappyNator";

    glm::vec3 eye(0.0, 2.0, 5.0);

    glm::vec3 playerStart(0.0, 2.0, 0.0);
    glm::vec3 playerScale(0.5, 0.5, 0.5);
    float playerRotation = 0.5 * M_PI;

    glm::vec3 sunPos(0.0f, -1.0f, 0.0f);
}
