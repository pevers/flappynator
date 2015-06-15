#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include <glm/glm.hpp>
#include <math.h>

#include "settings.h"
#include "staticobject.h"

class Enemy : public StaticObject
{
    public:
        Enemy(glm::vec3 enemyStart,
              glm::vec3 enemyScale,
              glm::vec3 enemyRotation,
              glm::vec3 enemyAcc,
              glm::vec3 enemySpeed,
              std::string enemyModel);
        virtual ~Enemy();

        void start(glm::vec3 playerPos);
        void update();

        void setSpeed(glm::vec3 speed);
        glm::vec3 getSpeed();
        void addAcc(glm::vec3 acc);
        glm::vec3 getAcc();
        void setInitAcc(glm::vec3 acc);
        glm::vec3 getInitAcc();
        void setHealth(int hp);
        int getHealth();
        void setAngle(float angle);
        float getAngle();
        bool isAlive();

    protected:
    private:
        glm::vec3 acc, initAcc, speed, startPos;
        int hp;
        bool alive, down;
        float angle;

};

#endif // ENEMY_H_INCLUDED
