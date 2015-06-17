#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <math.h>
#include <glm/glm.hpp>
#include <SFML/Window.hpp>
#include <memory>

#define 	M_PI   3.14159265358979323846	/* pi */

namespace Settings
{
    extern int screenWidth;
    extern int screenHeight;
    extern int depthBits;

    extern sf::ContextSettings context;

    extern std::string windowTitle;

    extern glm::vec3 eye;

    extern float startCameraTurnSpeed;

    extern glm::vec3 playerStart;
    extern glm::vec3 playerScale;
    extern glm::vec3 playerRotation;
    extern glm::vec3 playerAcc;
    extern glm::vec3 playerSpeed;
    extern std::string playerModel;
    extern std::string playerPath;
    extern int playerNumFrames;

    extern glm::vec3 enemyAcc;
    extern glm::vec3 enemyScale;
    extern glm::vec3 enemyRotation;
    extern std::string enemyModel1;
    extern std::string enemyPath;
    extern int enemyNumFrames;

    extern glm::vec3 bossStart;
    extern glm::vec3 bossScale;
    extern glm::vec3 bossRotation;
    extern std::string bossPath;
    extern int bossNumFrames;

    extern int numEnemies;
    extern float startEnemyUpdate;
    extern float startBossUpdate;
    extern float startBossStateRange;

    extern glm::vec3 sunPos;
    extern glm::vec3 sunSpot;
    extern float ambienIntensity;

    extern sf::Time ups;
}

#endif // SETTINGS_H
