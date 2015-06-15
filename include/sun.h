#ifndef SUN_H
#define SUN_H

#include <cmath>
#include <algorithm>
#include <glm/glm.hpp>

class Sun
{
    public:
        Sun(glm::vec3 color, glm::vec3 pos, glm::vec3 spot, float intensity);
        virtual ~Sun();

        void setPos(glm::vec3 pos);
        glm::vec3 getPos();

        void increaseIntensity(float intensity);
        void setIntensity(float intensity);
        float getIntensity();

        glm::vec3 getDirection();

        glm::vec3 getColor();
        void setColor(glm::vec3 color);

        glm::vec3 getSpot();
        void setSpot(glm::vec3 spot);
    private:
        float intensity;
        glm::vec3 pos, color, direction, spot;
};

#endif // SUN_H
