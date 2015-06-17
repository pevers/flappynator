#ifndef BOSS_H_INCLUDED
#define BOSS_H_INCLUDED

#include "settings.h"
#include "animatedobject.h"

class Boss : public AnimatedObject
{
    private:
        glm::vec4 boundingBox;

        double angle;
        float health;
        bool alive; //TODO: Moet weg

    public:
        Boss(glm::vec3 bossStart,
             glm::vec3 bossScale,
             glm::vec3 bossRotation);
        virtual ~Boss();

        void start();
        void update();
        bool isAlive(); //TODO: MOET WEG

        void setBoundingBox(glm::vec4 boundingBox);
        glm::vec4 getBoundingBox();
};

#endif // BOSS_H_INCLUDED
