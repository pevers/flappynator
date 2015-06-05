#define GLEW_STATIC

#ifndef TERRAIN_H
#define TERRAIN_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Terrain
{
    protected:
        GLuint vbo; // vertex buffer
        GLuint vboTexCoords;
        GLuint elementBuffer;
        GLuint normalBuffer;
    private:
    public:
        Terrain();
        virtual ~Terrain();

        virtual bool generateTerrain(unsigned int width, unsigned int height) = 0;
        virtual unsigned int getTerrainSize() = 0;

        GLuint getVertexBuffer();
        GLuint getTextureBuffer();
        GLuint getElementBuffer();
        GLuint getNormalBuffer();
};

#endif // TERRAIN_H
