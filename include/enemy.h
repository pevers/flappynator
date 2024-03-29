#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include <glm/glm.hpp>
#include <math.h>

#include "settings.h"
#include "animatedobject.h"

class Enemy : public AnimatedObject
{
    public:
        Enemy(glm::vec3 enemyStart,
              glm::vec3 enemyScale,
              glm::vec3 enemyRotation,
              glm::vec3 enemyAcc,
              glm::vec3 enemySpeed);
        virtual ~Enemy();

        void start(glm::vec3 playerPos);
        void update();
        void destroyObject();

        void setSpeed(glm::vec3 speed);
        glm::vec3 getSpeed();

        void setBoundingBox(glm::vec4 boundingBox);
        glm::vec4 getBoundingBox();

    private:
        glm::vec3 acc, initAcc, speed, startPos;
        int hp;
        bool down;
        float angle;
        glm::vec4 boundingBox;

};

#endif // ENEMY_H_INCLUDED
