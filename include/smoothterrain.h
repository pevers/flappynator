#ifndef SMOOTHTERRAIN_H
#define SMOOTHTERRAIN_H

#include <iostream>
#include <vector>
#include <typeinfo>
#include "terrain.h"
#include "simplexnoise.h"

class SmoothTerrain : public Terrain
{
    private:
        std::vector<glm::vec3> vertices;
        std::vector<unsigned int> indices;
        std::vector<std::vector<glm::vec3>> accNormals;
        std::vector<glm::vec3> normals;

        const float mountainInterval = 10;

        glm::vec3 calcAverageNormal(std::vector<glm::vec3> normals);
    public:
        SmoothTerrain();
        virtual ~SmoothTerrain();

        virtual bool generateTerrain(unsigned int width, unsigned int height);
        virtual unsigned int getTerrainSize();
};

#endif // SMOOTHTERRAIN_H
