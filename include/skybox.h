#ifndef CUBEMAPTEXTURE_H
#define CUBEMAPTEXTURE_H

#include <iostream>
#include <SOIL/SOIL.h>
#include <SFML/Window.hpp>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shaderprogram.h"

class Skybox
{
    public:
        Skybox(const std::string& posXFileName,
            const std::string& negXFileName,
            const std::string& posYFileName,
            const std::string& negYFileName,
            const std::string& posZFileName,
            const std::string& negZFileName);

        virtual ~Skybox();

        bool load();
        void bind(GLenum textureUnit);

        GLuint getShaderProgram();
        GLuint getVAO();
        GLuint getTexture();
        int getSize();

        glm::mat4 getModel();

        void update(float sunIntensity);
    private:
        bool loadShaders();
        bool loadVAO();

        void setDarken(float darken);

        static constexpr GLenum types[6] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                                  GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                                  GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                                  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                                  GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                                  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

        std::string fileNames[6];
        GLuint textureObj;

        GLuint skyboxVBO, skyboxVAO;
        GLuint shaderProgram, vertexShader, fragmentShader;

        sf::Clock clock;
};

#endif // CUBEMAPTEXTURE_H
