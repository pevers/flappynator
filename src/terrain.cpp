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
}

GLuint Terrain::getVertexBuffer()
{
    return vbo;
}

GLuint Terrain::getTextureBuffer()
{
    return vboTexCoords;
}
