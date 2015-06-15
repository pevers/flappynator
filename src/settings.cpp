#include "settings.h"
#define 	M_PI   3.14159265358979323846	/* pi */

namespace Settings
{
    int screenWidth = 1024;
    int screenHeight = 768;
    int bitsPerPixel = 32;

    sf::ContextSettings context(24, 8, 2);
    std::string windowTitle = "FlappyNator";

    glm::vec3 eye(0.0, 2.0, 25.0);

    float PI = M_PI;

    glm::vec3 playerStart(3.0, 1.0, 2.0);
    glm::vec3 playerScale(0.20, 0.20, 0.20);
    glm::vec3 playerAcc(0.0, - 1.0 / 2000, 0.0);
    glm::vec3 playerSpeed(1.0 / 30, 0.0, 0.0);  // should be 100
    glm::vec3 playerRotation(0.0, 0.5 * M_PI, 0.0);
    std::string playerModel = "resources/monkey.obj";

    glm::vec3 enemyAcc(0.0, - 1.0 / 2000, 0.0);
    glm::vec3 enemyScale(0.5, 0.5, 0.5);
    std::string enemyModel1 = "resources/monkey.obj";

    int numEnemies = 10;
    float startEnemyUpdate = 9.0; // When the horizontal distance between the enemy and the player is smaller than this value, update the enemy

    glm::vec3 sunPos(-10.0, 80.0, 0.0);
    glm::vec3 sunSpot(50, 0, 50);
    //glm::vec3 sunDirection(cos(45*3.1415/180.0)*70, -sin(45*3.1415/180.0)*70, 1.0f);

    sf::Time ups = sf::seconds(1.f / 60.f);
}
