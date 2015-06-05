#ifndef ENGINE_H
#define ENGINE_H

#define GLEW_STATIC
#define GLM_FORCE_RADIANS

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
#include "worldobject.h"
#include "smoothterrain.h"
#include "player.h"

class Engine
{
    private:
        sf::Window window;
        GLuint shaderProgram;
        GLuint elementBuffer;
        GLuint vertexBuffer;

        GLuint vao;

        // debug object shit
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> material;

        // sliding window, eye, center, speed
        Slide slide;
        std::unique_ptr<Terrain> terrain;

        glm::mat4 projMatrix;
        glm::mat4 viewMatrix;

        std::unique_ptr<Player> player;
        std::vector<std::unique_ptr<WorldObject>> wobjs;

        void cleanWorldObjectBuffers();
    public:
        Engine();
        virtual ~Engine();

        bool init();
        bool initSun();
        void drawFrame();
        void drawTerrain();
        void drawPlayer();
        void drawWorldObjects();
        void drawObject(WorldObject &w);
        void mainLoop();

        void updateWorldObjects();

        void handleKeyEvent(sf::Event event);
};

#endif // ENGINE_H
