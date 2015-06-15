#ifndef ENGINE_H
#define ENGINE_H

#define GLM_FORCE_RADIANS

#define GLEW_STATIC
#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>

#include "settings.h"
#include "shaderprogram.h"
#include "slide.h"
#include "flatterrain.h"
#include "../src/util/tiny_obj_loader.h"
#include "staticobject.h"
#include "animatedobject.h"
#include "smoothterrain.h"
#include "player.h"
#include "skybox.h"

class Engine
{
    private:
        sf::Window window;
        GLuint shaderProgram;
        GLuint depthShaderProgram;
        GLuint elementBuffer;
        GLuint vertexBuffer;
        GLuint frameBuffer;

        GLuint vao;

        GLuint depthTexture;

        // sliding window, eye, center, speed
        Slide slide;
        std::unique_ptr<Terrain> terrain;

        glm::mat4 projMatrix;
        glm::mat4 viewMatrix;
        glm::mat4 depthMVP;

        std::unique_ptr<Player> player;
        std::vector<std::unique_ptr<WorldObject>> wobjs;

        std::unique_ptr<Skybox> skybox;

        void cleanWorldObjectBuffers();
        void draw();
    public:
        Engine();
        virtual ~Engine();

        bool init();
        bool initSun();
        bool initShadowMap();

        void drawFrame();
        void drawTerrain();
        void drawPlayer();
        void drawSkybox();
        void drawWorldObjects();
        void drawObject(WorldObject &w);

        void drawShadows();
        void drawWorldShadow();
        void drawPlayerShadow();

        void mainLoop();

        void updateWorldObjects();

        void handleKeyEvent(sf::Event event);

        void boundingBox();
        void checkCollision();
};

#endif // ENGINE_H
