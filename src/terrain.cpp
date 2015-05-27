#include "terrain.h"

Terrain::Terrain()
{
    //ctor
}

Terrain::~Terrain()
{
    //dtor
}

GLuint Terrain::getVertexBuffer() {
    return vbo;
}

GLuint Terrain::getTextureBuffer() {
    return vboTexCoords;
}
