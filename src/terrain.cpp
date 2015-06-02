#include "terrain.h"

Terrain::Terrain()
{
    //ctor
}

Terrain::~Terrain()
{
    //dtor
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &vboTexCoords);
    glDeleteBuffers(1, &elementBuffer);
    glDeleteBuffers(1, &normalBuffer);
}

GLuint Terrain::getVertexBuffer()
{
    return vbo;
}

GLuint Terrain::getTextureBuffer()
{
    return vboTexCoords;
}

GLuint Terrain::getElementBuffer()
{
    return elementBuffer;
}

GLuint Terrain::getNormalBuffer()
{
    return normalBuffer;
}
