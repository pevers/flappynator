#ifndef PHYSICS_H_INCLUDED
#define PHYSICS_H_INCLUDED

#include <math.h>
#include <glm/glm.hpp>

class Physics
{
    private:
        float g;
        float max_accel;

    public:
        Physics();
        virtual ~Physics();

        float force(float m1, float m2, float distance);
        float distance(glm::vec3 p1, glm::vec3 p2);
        float getMaxAcceleration();
};

#endif // PHYSICS_H_INCLUDED
