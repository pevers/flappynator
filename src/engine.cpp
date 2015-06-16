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
    //terrain->generateTerrain(50, 20);
    terrain->generateTerrain(50, 6);

    // load test object
    player = std::unique_ptr<Player>(new Player());
    //wobjs.push_back(std::unique_ptr<StaticObject>(new StaticObject(glm::vec3(4.0, 4.0, 2.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, 0.0, 0.0), "resources/monkey.obj")));
    //wobjs.push_back(std::unique_ptr<Explosion>(new Explosion(glm::vec3(3.0, 2.0, 2.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0))));
    //wobjs.push_back(std::unique_ptr<AnimatedObject>(new AnimatedObject(glm::vec3(4.0, 4.0, 2.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, 0.0, 0.0))));

    // Initialize enemies
    initEnemies();

    // Initialize boss
    initBoss();

    //TEST
    sf::Image img;
    img.LoadFromFile("yourimage.bmp");

    sf::Sprite spr(img);

    yourwindow.Draw(spr);

    return true;
}

bool Engine::initEnemies() {
    glm::vec3 enemyScale = Settings::enemyScale;
    glm::vec3 enemyRotation = Settings::enemyRotation;
    std::string enemyModel = Settings::enemyModel1;

    srand(time(NULL));
    for(int i = 0; i < Settings::numEnemies; i++) {
        glm::vec3 enemyStart = player->getPos() + glm::vec3(15 + 10.0*i, 0.0, 0.0);
        glm::vec3 enemyAcc = Settings::enemyAcc;

        float speedX = (((rand()%200) / 100.0) + 2.0) * -1.0; // Value between 2.0 and 4.0
        glm::vec3 enemySpeed = Settings::playerSpeed * glm::vec3(speedX, 0.0, 0.0);

        std::unique_ptr<Enemy> enemy = std::unique_ptr<Enemy>(new Enemy(enemyStart, enemyScale, enemyRotation, enemyAcc, enemySpeed, enemyModel));
        enemy->setHealth(1);

        enemies.push_back(std::move(enemy));
    }

    return true;
}

bool Engine::initBoss() {
    boss = std::unique_ptr<Boss>(new Boss(Settings::bossStart, Settings::bossScale, Settings::bossRotation));
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
    drawEnemies();
    drawBoss();
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
    drawEnemyShadow();
    drawBossShadow();
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

    GLuint isTerrainID = glGetUniformLocation(shaderProgram, "isTerrain");
    glUniform1f(isTerrainID, 1);

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
    GLuint modelID = glGetUniformLocation(shaderProgram, "model");
    GLuint viewID = glGetUniformLocation(shaderProgram, "view");
    GLuint projID = glGetUniformLocation(shaderProgram, "proj");

    glm::mat4 model = w.getModel();
    glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(projID, 1, GL_FALSE, &projMatrix[0][0]);

    // TODO: differen shaders for terrain is maybe a better idea than this!
    GLuint isTerrainID = glGetUniformLocation(shaderProgram, "isTerrain");
    glUniform1f(isTerrainID, 0);

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

void Engine::drawEnemyShadow()
{
    for(auto &e : enemies)
    {
        // Only draw shadow when the object is in the scene
        if(e->getPos().x < slide.getEye().x + 10.0 && e->getPos().x > slide.getEye().x - 10.0)
        {
            glm::mat4 model = e->getModel();

            depthMVP = projMatrix * viewMatrix * model;
            GLuint depthMatrixID = glGetUniformLocation(depthShaderProgram, "depthMVP");
            glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);

            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, e->getVertexBuffer());
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, e->getElementBuffer());

            glDrawElements(GL_TRIANGLES, e->getObjectSize(), GL_UNSIGNED_INT, (void*)0);
            glDisableVertexAttribArray(0);
        }
    }

}

void Engine::drawBossShadow()
{
    glm::mat4 model = boss->getModel();

    depthMVP = projMatrix * viewMatrix * model;
    GLuint depthMatrixID = glGetUniformLocation(depthShaderProgram, "depthMVP");
    glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, boss->getVertexBuffer());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boss->getElementBuffer());

    glDrawElements(GL_TRIANGLES, boss->getObjectSize(), GL_UNSIGNED_INT, (void*)0);
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

void Engine::drawEnemies()
{
    for (auto &e : enemies) {
        drawObject(*e);
    }
}

void Engine::drawBoss()
{
    drawObject(*boss);
}

void Engine::update()
{
    //set bounding box
    boundingBox();

    //check collision
    checkCollision();

    // draw single frame
    drawFrame();

    // swap buffers
    window.display();
}

void Engine::startGame()
{
    // Start game
    glm::vec3 eye = slide.getEye();
    glm::vec3 center = slide.getCenter();

    slide.setEye(eye + glm::vec3((Settings::playerStart.x-Settings::eye.x)/Settings::startCameraTurnSpeed,
                                 (Settings::playerStart.y-Settings::eye.y)/Settings::startCameraTurnSpeed,
                                 ((Settings::playerStart.z+10.0)-Settings::eye.z)/Settings::startCameraTurnSpeed));
    slide.setCenter(player->getPos());

    // draw single frame
    drawFrame();

    // swap buffers
    window.display();

    // New gamestate
    if(eye.x >= 0.5)
        gameState.currentState = GameState::ST_PLAYING;
}

void Engine::playGame()
{
    // Play game

    update();
}

void Engine::bossLvl()
{
    // enter boss lvl

    glm::vec3 eye = slide.getEye();
    glm::vec3 center = slide.getCenter();

    if(eye.x > player->getPos().x - 10)
    {
        slide.setEye(eye + glm::vec3(-0.1, 0.0, -0.1));
        slide.setCenter(player->getPos());
    }

    update();
}

void Engine::endGame()
{
    // End game


}

void Engine::mainLoop() {

    sf::Clock clock;
    sf::Time accumulator = sf::Time::Zero;

    switch(gameState.currentState)
    {
        case GameState::ST_STARTING:
            std::cout << "gameState: " << gameState.currentState << std::endl;
    };

    window.setFramerateLimit(60);

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
                    if(gameState.currentState != GameState::ST_STARTING && gameState.currentState != GameState::ST_END)
                        handleKeyEvent(windowEvent);
                break;
                default:
                break;
            }
        }

        while (accumulator > Settings::ups)
        {
            accumulator -= Settings::ups;

            if(gameState.currentState != GameState::ST_STARTING)
                updateWorldObjects();
        }

        // Check gamestate
        switch(gameState.currentState)
        {
            case GameState::ST_STARTING:
                std::cout << "STARTING" << std::endl;
                startGame();
                break;
            case GameState::ST_PLAYING:
                std::cout << "PLAYING" << std::endl;
                playGame();
                break;
            case GameState::ST_BOSS:
                std::cout << "BOSS" << std::endl;
                bossLvl();
                break;
            case GameState::ST_END:
                std::cout << "GAME ENDED" << std::endl;
                endGame();
                break;
        }

        accumulator += clock.restart();
    }
}

void Engine::handleKeyEvent(sf::Event event)
{
    // debug, move the window
    glm::vec3 eye = slide.getEye();
    glm::vec3 center = slide.getCenter();
    if (event.key.code == sf::Keyboard::Up) {
        eye.y += 0.1;
        center.y += 0.1;
        glm::vec3 pos = player->getPos();
        pos.y += 0.1;
        player->setPos(pos);
    } else if (event.key.code == sf::Keyboard::Down) {
        eye.y -= 0.1;
        center.y -= 0.1;
        glm::vec3 pos = player->getPos();
        pos.y -= 0.1;
        player->setPos(pos);
    } else if (event.key.code == sf::Keyboard::Left) {
        eye.x -= 0.1;
        center.x -= 0.1;
        glm::vec3 pos = player->getPos();
        pos.x -= 0.1;
        player->setPos(pos);
    } else if (event.key.code == sf::Keyboard::Right) {
        eye.x += 0.1;
        center.x += 0.1;
        glm::vec3 pos = player->getPos();
        pos.x += 0.1;
        player->setPos(pos);
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
    } else if (event.key.code == sf::Keyboard::F) {
        if (wobjs.size() > 0) {
            StaticObject *obj = dynamic_cast<StaticObject*>(wobjs[0].get());
            obj->destroyObject();
        }
    }
    glm::vec3 pos = player->getPos();
    std::cout << "pos (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
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

    for (auto &e : enemies) {

        // Update the enemy only when the player is in range
        if(e->getPos().x - player->getPos().x < Settings::startEnemyUpdate) {
            if(e->isAlive())
                e->update();
            else {
                e->start(player->getPos());
                e->startAnimation();
            }
        }
    }

    player->update(gameState);

    if(boss->getPos().x - player->getPos().x <= Settings::startBossUpdate)
    {
        if(boss->isAlive())
            boss->update();
        else {
            boss->start();
            boss->startAnimation();
        }
    }

    if(gameState.currentState == GameState::ST_PLAYING)
    {
        slide.setCenter(player->getPos());
        slide.setEye(glm::vec3(player->getPos().x + 1.0, player->getPos().y, Settings::playerStart.z + 10));
    }

    // When in range of the boss, change game state to ST_BOSS
    if(gameState.currentState == GameState::ST_PLAYING && boss->getPos().x - player->getPos().x <= Settings::startBossStateRange)
    {
        gameState.currentState = GameState::ST_BOSS;
    }
}

void Engine::boundingBox() {
    //create/set bounding box for the world objects, such as the enemies
    /*for (auto &w : wobjs) {
        glm::vec3 pos = w->getPos();
        //x,y = coordinate bottom left
        //z = width of object
        //y = height of object
        glm::vec4 boundingBox = glm::vec4(pos.x - (0.5*w->getWidth()*Settings::playerScale.z), pos.y - (0.5*w->getHeight()*Settings::playerScale.y), (w->getWidth()*Settings::playerScale.z), (w->getHeight()*Settings::playerScale.y));
        w->setBoundingBox(boundingBox);
    }*/
    for (auto &e : enemies) {
        glm::vec3 pos = e->getPos();
        glm::vec4 boundingBox = glm::vec4(pos.x - (0.5*e->getWidth()*Settings::enemyScale.z), pos.y - (0.5*e->getHeight()*Settings::enemyScale.y), (e->getWidth()*Settings::enemyScale.z), (e->getHeight()*Settings::enemyScale.y));
        e->setBoundingBox(boundingBox);

        //std::cout << "bounding box: (" << boundingBox.x << ", " << boundingBox.y << ") width: " << boundingBox.z << " height: " << boundingBox.w << std::endl;
    }
    //create/set the bounding box for the bird
    glm::vec3 pos = player->getPos();
    glm::vec4 boundingBox = glm::vec4(pos.x - (0.5*player->getWidth()*Settings::playerScale.z), pos.y - (0.5*player->getHeight()*Settings::playerScale.y), (player->getHeight()*Settings::playerScale.z), (player->getHeight()*Settings::playerScale.y));
    player->setBoundingBox(boundingBox);
}

/**
  * Code from http://stackoverflow.com/questions/15125631/best-way-to-delete-a-stdunique-ptr-from-a-vector-with-a-raw-pointer
  */
void Engine::removeObjectFromVector(std::vector<std::unique_ptr<Enemy>> vec1, std::vector<Enemy*> vec2)
{
    vec1.erase(
        std::remove_if( // Selectively remove elements in the second vector...
            vec1.begin(),
            vec1.end(),
            [&] (std::unique_ptr<Enemy> const& p)
            {   // This predicate checks whether the element is contained
                // in the second vector of pointers to be removed...
                return std::find(
                    vec2.cbegin(),
                    vec2.cend(),
                    p.get()
                    ) != vec2.end();
            }),
        vec1.end()
        );

    vec2.clear();
}

void Engine::checkCollision() {
    glm::vec4 boundingBoxBird = player->getBoundingBox();
    int counter = 0;
    //check for world object collision with the bird
    /*for (auto &w : wobjs) {
        glm::vec4 boundingBoxWorld = w->getBoundingBox();
        if(boundingBoxBird.x < boundingBoxWorld.x + boundingBoxWorld.z &&
            boundingBoxBird.x + boundingBoxBird.z > boundingBoxWorld.x &&
            boundingBoxBird.y < boundingBoxWorld.y + boundingBoxWorld.w &&
            boundingBoxBird.y + boundingBoxBird.w > boundingBoxWorld.y)
        {
            std::cout << "got em boss" << std::endl;
            //delete the bird if the it is hit
            glDeleteBuffers(1, &player->getVertexBuffer());
            glDeleteBuffers(1, &player->getElementBuffer());
            //or RAGE, UNINSTALL SCRUB
            window.close();
        }
        counter++;
    }*/

    for (auto &e : enemies) {

        // Check collision with bird
        glm::vec4 boundingBoxWorld = e->getBoundingBox();
        if(boundingBoxBird.x < boundingBoxWorld.x + boundingBoxWorld.z &&
            boundingBoxBird.x + boundingBoxBird.z > boundingBoxWorld.x &&
            boundingBoxBird.y < boundingBoxWorld.y + boundingBoxWorld.w &&
            boundingBoxBird.y + boundingBoxBird.w > boundingBoxWorld.y)
        {
            //std::cout << "got em boss" << std::endl;
            //delete the bird if the it is hit
            //glDeleteBuffers(1, &player->getVertexBuffer());
            //glDeleteBuffers(1, &player->getElementBuffer());
            //or RAGE, UNINSTALL SCRUB
            //window.close();
            //e->destroyObject();

            gameState.currentState = GameState::ST_END;
        }

        // Check collision with terrain
        float startEnemy = floor(e->getBoundingBox().x); //the min x-value pos of the bird
        float endEnemy = floor(e->getBoundingBox().x + e->getBoundingBox().z); //the max x-value pos of the bird
        float detail = 0.25;
        std::vector<float> mountain;
        mountain = SmoothTerrain::getRandom();
        for(startEnemy; startEnemy < endEnemy; startEnemy += detail) {
            //initiate the counter with some correction
            int counter = floor(startEnemy*4+2);
            //check if the y-value of the mountain at the certain x is higher or equal to the y-value of the bird
            if(mountain[counter] >= e->getBoundingBox().y) {
                //if so then dead
                //std::cout << "enemy DIED" << std::endl;

                //e->destroyObject();

                //std::vector<Enemy*> vec2;
                //vec2.push_back(std::move(e));

                //removeEnemyFromVector(enemies, e);
            }
        }

        counter++;
    }

    //check for terrain collision
    float startBird = floor(player->getBoundingBox().x); //the min x-value pos of the bird
    float endBird = floor(player->getBoundingBox().x + player->getBoundingBox().z); //the max x-value pos of the bird
    float detail = 0.25;
    std::vector<float> mountain;
    mountain = SmoothTerrain::getRandom();
    for(startBird; startBird < endBird; startBird += detail) {
        //initiate the counter with some correction
        int counter = floor(startBird*4+2);
        //check if the y-value of the mountain at the certain x is higher or equal to the y-value of the bird
        if(mountain[counter] >= player->getBoundingBox().y) {
            //if so then dead
            //std::cout << "dead" << std::endl;
            //window.close();

            gameState.currentState = GameState::ST_END;
        }
    }
}
