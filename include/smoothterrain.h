#ifndef SMOOTHTERRAIN_H
#define SMOOTHTERRAIN_H

#include <iostream>
#include <vector>
#include <typeinfo>
#include <SOIL/SOIL.h>
#include "settings.h"
#include "terrain.h"
#include "simplexnoise.h"

class SmoothTerrain : public Terrain
{
    private:
        std::vector<glm::vec3> vertices;
        std::vector<unsigned int> indices;
        std::vector<std::vector<glm::vec3>> accNormals;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texCoords;

        std::vector<glm::vec3> mountains;


        //static std::vector<float> new_vec;
        GLuint textures[1];

        glm::vec3 calcAverageNormal(std::vector<glm::vec3> normals);

        //float getAcc();
    public:
        SmoothTerrain();
        virtual ~SmoothTerrain();

        virtual bool generateTerrain(unsigned int width, unsigned int height);
        virtual unsigned int getTerrainSize();
        static float maxY[];

        static std::vector<float> getRandom();
};

#endif // SMOOTHTERRAIN_H
