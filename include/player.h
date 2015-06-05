#ifndef PLAYER_H
#define PLAYER_H

#include "settings.h"
#include "worldobject.h"

class Player : public WorldObject
{
    public:
        Player();
        virtual ~Player();

        void update();

        void setSpeed(glm::vec3 speed);
        glm::vec3 getSpeed();
        void addAcc(glm::vec3 acc);
        glm::vec3 getAcc();

    protected:
    private:
        glm::vec3 acc, speed;
};

#endif // PLAYER_H
