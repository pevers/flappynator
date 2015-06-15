#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <math.h>
#include <glm/glm.hpp>
#include <SFML/Window.hpp>
#include <memory>

namespace Settings
{
    extern int screenWidth;
    extern int screenHeight;
    extern int depthBits;

    extern sf::ContextSettings context;

    extern std::string windowTitle;

    extern glm::vec3 eye;

    extern float PI;

    extern glm::vec3 playerStart;
    extern glm::vec3 playerScale;
    extern glm::vec3 playerRotation;
    extern glm::vec3 playerAcc;
    extern glm::vec3 playerSpeed;
    extern std::string playerModel;

    extern glm::vec3 enemyAcc;
    extern glm::vec3 enemyScale;
    extern std::string enemyModel1;

    extern int numEnemies;
    extern float startEnemyUpdate;

    extern glm::vec3 sunPos;
    extern glm::vec3 sunSpot;
    //extern glm::vec3 sunDirection;

    extern sf::Time ups;
}

#endif // SETTINGS_H
