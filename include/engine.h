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

#include "settings.h"
#include "shaderprogram.h"
#include "slide.h"
#include "flatterrain.h"
#include "../src/util/tiny_obj_loader.h"
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
        Terrain *terrain;

        glm::mat4 projMatrix;
        glm::mat4 viewMatrix;

        std::vector<WorldObject> wobjs;
    public:
        Engine();
        virtual ~Engine();

        bool init();
        bool initSun();
        void drawFrame();
        void drawTerrain();
        void drawWorldObjects();
        void mainLoop();

        // debug test
        void loadObject(const std::string &path);

        void handleKeyEvent(sf::Event event);
};

#endif // ENGINE_H
