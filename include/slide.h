#define GLEW_STATIC

#ifndef SLIDE_H
#define SLIDE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
 * Sliding window
 */
class Slide
{
    private:
        glm::vec3 eye;   // start
        glm::vec3 center;
        double speed;
    public:
        Slide(glm::vec3 eye, glm::vec3 direction, double speed);
        virtual ~Slide();

        glm::mat4 getSlideView();

        glm::vec3 getEye();
        glm::vec3 getCenter();
        double getSpeed();

        void setEye(glm::vec3 eye);
        void setCenter(glm::vec3 center);
        void setSpeed(double speed);
};

#endif // SLIDE_H
