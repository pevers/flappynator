#ifndef ENGINE_H
#define ENGINE_H

#define GLM_FORCE_RADIANS

#define GLEW_STATIC
#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include <cmath>

#include "settings.h"
#include "shaderprogram.h"
#include "slide.h"
#include "flatterrain.h"
#include "../src/util/tiny_obj_loader.h"
#include "staticobject.h"
#include "animatedobject.h"
#include "smoothterrain.h"
#include "player.h"
#include "enemy.h"
#include "skybox.h"
#include "gamestate.h";
#include "boss.h";
#include "sun.h"
#include "projectile.h"

class Engine
{
    private:
        sf::RenderWindow window;
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
        std::unique_ptr<Sun> sun;

        glm::mat4 projMatrix;
        glm::mat4 viewMatrix;
        glm::mat4 depthMVP;

        std::unique_ptr<Player> player;
        std::vector<std::unique_ptr<WorldObject>> wobjs;
        std::vector<std::unique_ptr<Enemy>> enemies;

        std::unique_ptr<Boss> boss;

        std::vector<std::unique_ptr<Projectile>> projectiles;
        std::unique_ptr<Projectile> projectile;

        std::unique_ptr<Skybox> skybox;

        void cleanWorldObjectBuffers();
        void draw();

        GameState gameState;
    public:
        Engine();
        virtual ~Engine();

        bool init();
        bool initSun();
        bool resetSun();
        bool initShadowMap();
        bool initEnemies();
        bool initBoss();

        void initMenu();
        void drawShadows();
        void drawFrame();
        void drawTerrain();
        void drawPlayer();
        void drawEnemies();
        void drawBoss();
        void drawProjectiles();
        void drawProjectile(Projectile &p);
        void drawSkybox();
        void drawWorldObjects();
        void drawObject(WorldObject &w);
        void drawWorldShadow();
        void drawPlayerShadow();
        void drawEnemyShadow();
        void drawBossShadow();

        void update();
        void startGame();
        void playGame();
        void bossLvl();
        void endGame();

        void mainLoop();

        void updateWorldObjects();

        void handleKeyEvent(sf::Event event);

        void boundingBox();
        void checkCollision();
};

#endif // ENGINE_H
