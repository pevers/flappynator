#define GLEW_STATIC
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
    glDeleteProgram(depthShaderProgram);
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

    // create shader program for the shadow mapping, TODO: move shader creation, attaching, etc. to seperate file
    depthShaderProgram = glCreateProgram();
    fragmentShader = ShaderProgram::loadShaderFromFile("shaders/depthFragment.shader", GL_FRAGMENT_SHADER);
    vertexShader = ShaderProgram::loadShaderFromFile("shaders/depthVertex.shader", GL_VERTEX_SHADER);

    if (!fragmentShader || !vertexShader) {
        return false;
    }

    glAttachShader(depthShaderProgram, vertexShader);
    glAttachShader(depthShaderProgram, fragmentShader);
    glLinkProgram(depthShaderProgram);

    if (!initShadowMap()) {
        std::cerr << "no shadow map initialized " << std::endl;
        return false;
    }

    // load sun
    if (!initSun()) {
        std::cerr << "no sun initialized " << std::endl;
        return false;
    }

    // load terrain
    terrain.reset(new SmoothTerrain());
    terrain->generateTerrain(50, 50);

    // load test object
    player = std::unique_ptr<Player>(new Player());
    //wobjs.push_back();

    return true;
}

bool Engine::initShadowMap() {

    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    glGenFramebuffers(1, &frameBuffer);

    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, Settings::screenWidth, Settings::screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Always check that our framebuffer is ok
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
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

    // draw shadows
    drawShadows();

    // draw rest
    draw();
}

void Engine::draw()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Settings::screenWidth, Settings::screenHeight);

    glUseProgram(shaderProgram);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // calculate matrix
    viewMatrix = slide.getSlideView();
    projMatrix = glm::perspective(45.0f, 1024.0f / 768.0f, 1.0f, 100.0f);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    GLuint shadowMap = glGetUniformLocation(shaderProgram, "shadowMap");
    glUniform1i(shadowMap, 1);

    drawTerrain();
    drawWorldObjects();
    drawPlayer();
}

void Engine::drawShadows()
{
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glViewport(0, 0, Settings::screenWidth, Settings::screenHeight);

    glUseProgram(depthShaderProgram);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    projMatrix = glm::ortho<float>(-100, 100, -100, 100, -100, 200);
    glm::vec3 sunPos = glm::vec3(0, 10, 0);
    viewMatrix = glm::lookAt(sunPos, glm::vec3(15,0,15), glm::vec3(0,1,0));

    drawPlayerShadow();
    drawWorldShadow();
}

void Engine::drawTerrain()
{
    // Render to the screen
    glm::mat4 model;
    glm::mat4 MVP = projMatrix * viewMatrix * model;

    GLuint matrixID = glGetUniformLocation(shaderProgram, "MVP");
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);  // bind matrix to shader

    GLuint depthBias = glGetUniformLocation(shaderProgram, "depthBiasMVP");
    glm::mat4 biasMatrix(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    );

    glm::mat4 depthBiasMVP = biasMatrix * depthMVP;
    glUniformMatrix4fv(depthBias, 1, GL_FALSE, &depthBiasMVP[0][0]);

    // send vertex buffer
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, terrain->getVertexBuffer());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // texture buffer
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, terrain->getTextureBuffer());
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // send normal buffer
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, terrain->getNormalBuffer());
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    GLuint uniColor = glGetUniformLocation(shaderProgram, "overrideColor");
    glUniform3f(uniColor, 1.0f, 1.0f, 1.0f);

    glDrawArrays(GL_TRIANGLES, 0, terrain->getTerrainSize());
    glDisableVertexAttribArray(0);
}

void Engine::drawObject(WorldObject &w)
{
    //glm::mat4 modelMatrix = glm::translate(modelMatrix, wobjs[i].getPos());
    glm::mat4 model = w.getModel();
    //modelMatrix = glm::rotate(modelMatrix, w.getRotation().z, glm::vec3(0.0, 0.0, 1.0));
    glm::mat4 MVP = projMatrix * viewMatrix * model;

    GLuint matrixID = glGetUniformLocation(shaderProgram, "MVP");
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);  // bind matrix to shader

    // get vertex buffer
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, w.getVertexBuffer());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    GLuint depthBias = glGetUniformLocation(shaderProgram, "depthBiasMVP");
    glm::mat4 biasMatrix(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    );

    glm::mat4 depthBiasMVP = biasMatrix * MVP;
    glUniformMatrix4fv(depthBias, 1, GL_FALSE, &depthBiasMVP[0][0]);

    // texture buffer
//    glEnableVertexAttribArray(1);
//    glBindBuffer(GL_ARRAY_BUFFER, w.getTextureBuffer());
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // send normal buffer
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, w.getNormalBuffer());
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, w.getElementBuffer());

    // override texture colors
    GLuint uniColor = glGetUniformLocation(shaderProgram, "overrideColor");
    glUniform3f(uniColor, 1.0f, 0.3f, 0.3f);

    glDrawElements(GL_TRIANGLES, w.getObjectSize(), GL_UNSIGNED_INT, (void*)0);
    glDisableVertexAttribArray(0);
}

void Engine::drawWorldShadow()
{
    glm::mat4 model = glm::mat4(1.0);
    depthMVP = projMatrix * viewMatrix * model;
    GLuint depthMatrixID = glGetUniformLocation(depthShaderProgram, "depthMVP");
    glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);

    // get vertex buffer
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, terrain->getVertexBuffer());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_TRIANGLES, 0, terrain->getTerrainSize());
    glDisableVertexAttribArray(0);
}

void Engine::drawPlayerShadow()
{
    glm::mat4 model = player->getModel();

    depthMVP = projMatrix * viewMatrix * model;
    GLuint depthMatrixID = glGetUniformLocation(depthShaderProgram, "depthMVP");
    glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, player->getVertexBuffer());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, player->getElementBuffer());

    glDrawElements(GL_TRIANGLES, player->getObjectSize(), GL_UNSIGNED_INT, (void*)0);
    glDisableVertexAttribArray(0);
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
    } else if (event.key.code == sf::Keyboard::W) {
        eye.z--;
        center.z--;
    } else if (event.key.code == sf::Keyboard::S) {
        eye.z++;
        center.z++;
    } else if (event.key.code == sf::Keyboard::Space) {
        player->addAcc(glm::vec3(0.0f, 1.0f / 200.0, 0.0f));
    }

    std::cout << "eye (" << eye.x << ", " << eye.y << ", " << eye.z << ")" << std::endl;
    slide.setEye(eye);
    slide.setCenter(center);
}

void Engine::cleanWorldObjectBuffers()
{
    for (unsigned int i = 0; i < wobjs.size(); i++) {
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
    slide.setCenter(player->getPos());
    slide.setEye(glm::vec3(player->getPos().x + 1.0, player->getPos().y, 8.0));
}

