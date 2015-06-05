#include "engine.h"

Engine::Engine(): slide(Settings::eye, glm::vec3(0.0, 0.0, 0.0), 1.0)
{
    //ctor
}

Engine::~Engine()
{
    //dtor
    cleanWorldObjectBuffers();
    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &vao);

}

bool Engine::init()
{
    window.create(sf::VideoMode(Settings::screenWidth, Settings::screenHeight, Settings::context.depthBits), Settings::windowTitle.c_str(), sf::Style::Close, Settings::context);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "failed to initialize GLEW" << std::endl;
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Create Vertex Array Object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // create shaders and load basic shaders
    shaderProgram = glCreateProgram();
    GLuint fragmentShader = ShaderProgram::loadShaderFromFile("shaders/fragment.shader", GL_FRAGMENT_SHADER);
    GLuint vertexShader = ShaderProgram::loadShaderFromFile("shaders/vertex.shader", GL_VERTEX_SHADER);

    if (!fragmentShader || !vertexShader) {
        return false;
    }

     // Link the vertex and fragment shader into a shader program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    // load sun
    if (!initSun()) {
        std::cerr << "no sun initialized " << std::endl;
        return false;
    }

    // load terrain
    terrain.reset(new SmoothTerrain());
    terrain->generateTerrain(200, 10);

    // load test object
    player = std::unique_ptr<Player>(new Player());
    //wobjs.push_back();

    return true;
}

bool Engine::initSun()
{
    GLuint color = glGetUniformLocation(shaderProgram, "sunLight.color");
    glUniform3f(color, 1.0f, 1.0f, 1.0f);

    GLuint intensity = glGetUniformLocation(shaderProgram, "sunLight.ambientIntensity");
    glUniform1f(intensity, 0.60f);

    GLuint direction = glGetUniformLocation(shaderProgram, "sunLight.direction");
    glUniform3f(direction, glm::normalize(Settings::sunDirection).x, glm::normalize(Settings::sunDirection).y, glm::normalize(Settings::sunDirection).z);

    return (color && intensity && direction);
}

void Engine::drawFrame()
{
    // draw a single frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    viewMatrix = slide.getSlideView();
    projMatrix = glm::perspective(45.0f, 800.0f / 600.0f, 1.0f, 100.0f);

    // draw terrain
    drawTerrain();

    // draw objects
    drawWorldObjects();

    // draw player
    drawPlayer();
}

void Engine::drawTerrain()
{
    // no model matrix, defaults  to 0,0
    glm::mat4 modelMatrix;
    glm::mat4 MVP = projMatrix * viewMatrix * modelMatrix;

    GLuint matrixID = glGetUniformLocation(shaderProgram, "MVP");
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);  // bind matrix to shader

    GLuint uniColor = glGetUniformLocation(shaderProgram, "overrideColor");

    // send vertex buffer
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, terrain->getVertexBuffer());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // send normal buffer
    GLint normalAttrib = glGetAttribLocation(shaderProgram, "inNormal");
    glEnableVertexAttribArray(normalAttrib);
    glBindBuffer(GL_ARRAY_BUFFER, terrain->getNormalBuffer());
    glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain->getElementBuffer());

    glUniform3f(uniColor, 0.129f, 0.698f, 0.02f);
    glDrawElements(GL_TRIANGLES, terrain->getTerrainSize(), GL_UNSIGNED_INT, (void*)0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(normalAttrib);

}

void Engine::drawObject(WorldObject &w)
{
    //glm::mat4 modelMatrix = glm::translate(modelMatrix, wobjs[i].getPos());
    GLuint uniColor = glGetUniformLocation(shaderProgram, "overrideColor");
    glm::mat4 modelMatrix;
    modelMatrix = glm::rotate(glm::scale(glm::translate(modelMatrix, w.getPos()), w.getScale()), w.getRotation().z, glm::vec3(0.0, 0.0, 1.0));
    modelMatrix = glm::rotate(modelMatrix, w.getRotation().y, glm::vec3(0.0, 1.0, 0.0));
    //modelMatrix = glm::rotate(modelMatrix, w.getRotation().z, glm::vec3(0.0, 0.0, 1.0));
    glm::mat4 MVP = projMatrix * viewMatrix * modelMatrix;

    GLuint matrixID = glGetUniformLocation(shaderProgram, "MVP");
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);  // bind matrix to shader

    // get vertex buffer
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, w.getVertexBuffer());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // get normal buffer
    GLint normalAttrib = glGetAttribLocation(shaderProgram, "inNormal");
    glEnableVertexAttribArray(normalAttrib);
    glBindBuffer(GL_ARRAY_BUFFER, w.getNormalBuffer());
    glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, w.getElementBuffer());

    // override texture colors
    glUniform3f(uniColor, 1.0f, 0.3f, 0.3f);
    glDrawElements(GL_TRIANGLES, w.getObjectSize(), GL_UNSIGNED_INT, (void*)0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(normalAttrib);

}

void Engine::drawWorldObjects()
{
    for (auto &w : wobjs) {
        drawObject(*w);
    }
}

void Engine::drawPlayer()
{
    drawObject(*player);
}

void Engine::mainLoop() {

    sf::Clock clock;
    sf::Time accumulator = sf::Time::Zero;

    while (window.isOpen())
    {
        // poll window events
        sf::Event windowEvent;
        while (window.pollEvent(windowEvent))
        {
            switch (windowEvent.type)
            {
                case sf::Event::Closed:
                    window.close();
                break;
                case sf::Event::KeyPressed:
                    handleKeyEvent(windowEvent);
                break;
                default:
                break;
            }
        }

        while (accumulator > Settings::ups)
        {
            accumulator -= Settings::ups;
            updateWorldObjects();
        }

        // draw single frame
        drawFrame();

        // swap buffers
        window.display();

        accumulator += clock.restart();
    }
}

void Engine::handleKeyEvent(sf::Event event)
{
    // debug, move the window
    glm::vec3 eye = slide.getEye();
    glm::vec3 center = slide.getCenter();
    if (event.key.code == sf::Keyboard::Up) {
        eye.y++;
        center.y++;
    } else if (event.key.code == sf::Keyboard::Down) {
        eye.y--;
        center.y--;
    } else if (event.key.code == sf::Keyboard::Left) {
        eye.x--;
        center.x--;
    } else if (event.key.code == sf::Keyboard::Right) {
        eye.x++;
        center.x++;
    } else if (event.key.code == sf::Keyboard::R) {
        eye = Settings::eye;
        center = glm::vec3(0.0, 0.0, 0.0);
    } else if (event.key.code == sf::Keyboard::Space) {
        player->addAcc(glm::vec3(0.0f, 1.0f / 200.0, 0.0f));
    }

    std::cout << "eye (" << eye.x << ", " << eye.y << ", " << eye.z << ")" << std::endl;
    slide.setEye(eye);
    slide.setCenter(center);
}

void Engine::cleanWorldObjectBuffers()
{
    for (int i = 0; i < wobjs.size(); i++) {
        glDeleteBuffers(1, &wobjs[i]->getVertexBuffer());
        glDeleteBuffers(1, &wobjs[i]->getElementBuffer());
    }
}

/**
 * Update world objects at a rate of Settings::ups.
 */
void Engine::updateWorldObjects()
{
    for (auto &w : wobjs) {
        w->update();
    }

    player->update();
}

