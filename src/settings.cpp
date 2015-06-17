#include "settings.h"

namespace Settings
{
    int screenWidth = 1024;
    int screenHeight = 768;
    int bitsPerPixel = 32;

    sf::ContextSettings context(24, 8, 2);
    std::string windowTitle = "FlappyNator";

    glm::vec3 eye(-10.0, 3.0, 2.0);
    //glm::vec3 eye(0.0, 2.0, 25.0);

    float PI = M_PI;

    float startCameraTurnSpeed = 240.0;

    glm::vec3 playerStart(3.0, 3.0, 2.0);
    glm::vec3 playerScale(0.20, 0.20, 0.20);
    glm::vec3 playerAcc(0.0, - 1.0 / 2000, 0.0);
    glm::vec3 playerSpeed(1.0 / 30, 0.0, 0.0);  // should be 100
    glm::vec3 playerRotation(0.0, 0.5 * M_PI, 0.0);
    std::string playerPath = "resources/animation/player/";
    int playerNumFrames = 45;

    glm::vec3 enemyAcc(0.0, - 1.0 / 2000, 0.0);
    glm::vec3 enemyScale(0.25, 0.25, 0.25);
    glm::vec3 enemyRotation(0.0, 0.5 * M_PI, 0.0);
    std::string enemyModel1 = "resources/monkey.obj";
    std::string enemyPath = "resources/animation/enemy/";
    int enemyNumFrames = 41;

    glm::vec3 bossStart(30.0, 4.0, 2.0);
    glm::vec3 bossScale(0.5, 0.5, 0.5);
    glm::vec3 bossRotation(0.0, 0.5 * M_PI, 0.0);
    std::string bossPath = "resources/animation/boss/";
    int bossNumFrames = 1;

    int numEnemies = 2;
    float startEnemyUpdate = 15.0; // When the horizontal distance between the enemy and the player is smaller than this value, update the enemy
    float startBossUpdate = 10.0;
    float startBossStateRange = 8.0;

    glm::vec3 projectileSpeed(1.0 / 10, 0.0, 0.0);
    //glm::vec3 sunPos(-10.0, 80.0, 0.0);
    glm::vec3 sunPos(-10.0, 80.0, 60.0);

    glm::vec3 sunSpot(50, 0, 50);

    sf::Time ups = sf::seconds(1.f / 60.f);
}
