#ifndef PLAYER_H
#define PLAYER_H

#include "settings.h"
#include "animatedobject.h"

class Player : public AnimatedObject
{
    public:
        Player();
        virtual ~Player();

        void update();

        void setSpeed(glm::vec3 speed);
        glm::vec3 getSpeed();
        void addAcc(glm::vec3 acc);
        glm::vec3 getAcc();

        void setBoundingBox(glm::vec4 boundingBox);
        glm::vec4 getBoundingBox();

    protected:
    private:
        glm::vec3 acc, speed;
        glm::vec4 boundingBox;
};

#endif // PLAYER_H
