#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include <glm/glm.hpp>

#include "settings.h"
#include "worldobject.h"

class Enemy : public WorldObject
{
    public:
        Enemy(glm::vec3 enemyStart,
              glm::vec3 enemyScale,
              glm::vec3 enemyRotation,
              glm::vec3 enemyAcc,
              glm::vec3 enemySpeed,
              std::string enemyModel);
        virtual ~Enemy();

        void update();

        void setSpeed(glm::vec3 speed);
        glm::vec3 getSpeed();
        void addAcc(glm::vec3 acc);
        glm::vec3 getAcc();
        void calcPathToPlayer();

    protected:
    private:
        glm::vec3 acc, speed;

};

#endif // ENEMY_H_INCLUDED
