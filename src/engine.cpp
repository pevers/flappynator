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
    glDepthFunc(GL_LEQUAL);

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

    skybox = std::unique_ptr<Skybox>(new Skybox("resources/skybox/right.png",
            "resources/skybox/left.png",
            "resources/skybox/top.png",
            "resources/skybox/bottom.png",
            "resources/skybox/back.png",
            "resources/skybox/front.png"));

    if (!skybox->load()) {
        std::cerr << "could not initalize skybox " << std::endl;
        return false;
    }

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
    terrain->generateTerrain(50, 20);

    // load test object
    player = std::unique_ptr<Player>(new Player());
    //wobjs.push_back(std::unique_ptr<AnimatedObject>(new AnimatedObject(glm::vec3(4.0, 4.0, 2.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, 0.0, 0.0))));

    return true;
}

bool Engine::initShadowMap() {

    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    glGenFramebuffers(1, &frameBuffer);

    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1014, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
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
    glUniform1f(intensity, 0.50f);

    GLuint direction = glGetUniformLocation(shaderProgram, "sunLight.direction");
    glUniform3f(direction, glm::normalize(Settings::sunSpot - Settings::sunPos).x, glm::normalize(Settings::sunSpot - Settings::sunPos).y, glm::normalize(Settings::sunSpot - Settings::sunPos).z);

    GLuint pos = glGetUniformLocation(shaderProgram, "sunLight.pos");
    glUniform3f(pos, Settings::sunPos.x, Settings::sunPos.y, Settings::sunPos.z);

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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // calculate matrix
    viewMatrix = slide.getSlideView();
    projMatrix = glm::perspective(45.0f, (float)Settings::screenWidth / (float)Settings::screenHeight, 1.0f, 200.0f);

    glUseProgram(shaderProgram);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    GLuint shadowMap = glGetUniformLocation(shaderProgram, "shadowMap");
    glUniform1i(shadowMap, 1);

    glCullFace(GL_BACK);

    drawTerrain();
    drawWorldObjects();
    drawPlayer();

    drawSkybox();
}

void Engine::drawSkybox()
{
    glDepthMask(GL_FALSE);
    GLuint shader = skybox->getShaderProgram();
    glUseProgram(shader);

    // set projection and view matrix
    GLuint projID = glGetUniformLocation(shader, "MVP");
    glm::mat4 MVP = projMatrix * viewMatrix * skybox->getModel();

    glUniformMatrix4fv(projID, 1, GL_FALSE, &MVP[0][0]);

    glBindVertexArray(skybox->getVAO());
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->getTexture());
    glDrawArrays(GL_TRIANGLES, 0, skybox->getSize());
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
}

void Engine::drawShadows()
{
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glViewport(0, 0, 1024, 1014);

    glUseProgram(depthShaderProgram);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    projMatrix = glm::ortho<float>(-100, 100, -100, 100, -100, 200);
    viewMatrix = glm::lookAt(Settings::sunPos, Settings::sunSpot, glm::vec3(0, 1, 0));

    glCullFace(GL_FRONT);

    drawPlayerShadow();
    drawWorldShadow();
}

void Engine::drawTerrain()
{
    // Render to the screen
    GLuint modelID = glGetUniformLocation(shaderProgram, "model");
    GLuint viewID = glGetUniformLocation(shaderProgram, "view");
    GLuint projID = glGetUniformLocation(shaderProgram, "proj");

    glm::mat4 model;
    glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(projID, 1, GL_FALSE, &projMatrix[0][0]);

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
    //modelMatrix = glm::rotate(modelMatrix, w.getRotation().z, glm::vec3(0.0, 0.0, 1.0));
    GLuint modelID = glGetUniformLocation(shaderProgram, "model");
    GLuint viewID = glGetUniformLocation(shaderProgram, "view");
    GLuint projID = glGetUniformLocation(shaderProgram, "proj");

    glm::mat4 model = w.getModel();
    glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(projID, 1, GL_FALSE, &projMatrix[0][0]);

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

    glm::mat4 depthBiasMVP = biasMatrix * projMatrix * viewMatrix * model;
    glUniformMatrix4fv(depthBias, 1, GL_FALSE, &depthBiasMVP[0][0]);

    // texture buffer
//    glEnableVertexAttribArray(1);
//    glBindBuffer(GL_ARRAY_BUFFER, w.getTextureBuffer());
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // send normal buffer
    if (w.hasNormals()) {
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, w.getNormalBuffer());
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

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
    } else if (event.key.code == sf::Keyboard::A) {
        eye.x--;
    } else if (event.key.code == sf::Keyboard::D) {
        eye.x++;
    } else if (event.key.code == sf::Keyboard::Space) {
        player->addAcc(glm::vec3(0.0f, 1.0f / 200.0, 0.0f));
        player->startAnimation();
    }

    std::cout << "eye (" << eye.x << ", " << eye.y << ", " << eye.z << ")" << std::endl;
    slide.setEye(eye);
    slide.setCenter(center);
}

void Engine::cleanWorldObjectBuffers()
{
    for (unsigned int i = 0; i < wobjs.size(); i++) {
        wobjs[i]->free();
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
    slide.setEye(glm::vec3(player->getPos().x + 1.0, player->getPos().y, Settings::playerStart.z + 10));
}

