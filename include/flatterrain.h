#ifndef FLATTERRAIN_H
#define FLATTERRAIN_H

// debug purpose
#include <iostream>
#include <vector>
#include <SOIL/SOIL.h>

#include "terrain.h"

class FlatTerrain : public Terrain
{
    protected:
    private:
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> texCoords;

        GLuint textures[1];
        int texWidth;
        int texHeight;
    public:
        FlatTerrain();
        virtual ~FlatTerrain();

        virtual bool generateTerrain(unsigned int width=2, unsigned int height=2);
        virtual unsigned int getTerrainSize();
};

#endif // FLATTERRAIN_H
