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

    // Initialize the menu of the game
    initMenu();

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

    terrain->generateTerrain(140, 8);

    // load test object
    player = std::unique_ptr<Player>(new Player());
    //wobjs.push_back(std::unique_ptr<StaticObject>(new StaticObject(glm::vec3(4.0, 4.0, 2.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, 0.0, 0.0), "resources/monkey.obj")));
    //wobjs.push_back(std::unique_ptr<Explosion>(new Explosion(glm::vec3(3.0, 2.0, 2.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0))));
    //wobjs.push_back(std::unique_ptr<AnimatedObject>(new AnimatedObject(glm::vec3(4.0, 4.0, 2.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, 0.0, 0.0))));

    // Initialize enemies
    initEnemies();

    // Initialize boss
    initBoss();

    return true;
}

void Engine::initMenu(){
    textureBackground.loadFromFile("resources/background.png");
    spriteBackground.setTexture(textureBackground);
    spriteBackground.setTextureRect(sf::IntRect(0, 0, 1024, 764));
    spriteBackground.setColor(sf::Color(255, 255, 255, 200));
    spriteBackground.setPosition(0, 0);

    sf::Texture textureName;
    textureName.loadFromFile("resources/flappy.png");
    sf::Sprite spriteName;
    spriteName.setTexture(textureName);
    spriteName.setTextureRect(sf::IntRect(0, 0, 1024, 764));
    spriteName.setPosition(Settings::screenWidth*0.28, Settings::screenHeight*0.1);

    sf::Texture textureControl;
    textureControl.loadFromFile("resources/control.png");
    sf::Sprite spriteControl;
    spriteControl.setTexture(textureControl);
    spriteControl.setTextureRect(sf::IntRect(0, 0, 1024, 764));
    spriteControl.setPosition(Settings::screenWidth*0.33, Settings::screenHeight*0.25);
    while (window.isOpen() && windowOn)
    {

        // poll window events
        sf::Event windowEvent;
        while (window.pollEvent(windowEvent))
        {
                if (windowEvent.type == sf::Event::Closed)
                    window.close();
                if ((windowEvent.type == sf::Event::KeyPressed) && (windowEvent.key.code == sf::Keyboard::Space)) {
                    windowOn = false;
                    std::cerr << "Initialize game" << std::endl;
                }
        }
        window.draw(spriteBackground);
        window.draw(spriteName);
        window.draw(spriteControl);
        window.display();
        window.clear();
    }
}
void Engine::initEndMenu(bool win){
    window2.create(sf::VideoMode(Settings::screenWidth, Settings::screenHeight, Settings::context.depthBits), Settings::windowTitle.c_str(), sf::Style::Close, Settings::context);
    window.close();

    sf::Texture textureEnd;
    sf::Sprite spriteEnd;
    if (win) {
        textureEnd.loadFromFile("resources/win.png");
        spriteEnd.setPosition(Settings::screenWidth*0.01, Settings::screenHeight*0.25);
    }
    else {
        textureEnd.loadFromFile("resources/gameover.png");
        spriteEnd.setPosition(Settings::screenWidth*0.01, Settings::screenHeight*0.25);
    }
    spriteEnd.setTexture(textureEnd);
    spriteEnd.setTextureRect(sf::IntRect(0, 0, 1024, 550));


    while (window2.isOpen() )
    {
        // poll window events
        sf::Event windowEvent;
        while (window2.pollEvent(windowEvent))
        {
                if (windowEvent.type == sf::Event::Closed)
                    window2.close();
                if ((windowEvent.type == sf::Event::KeyPressed) && (windowEvent.key.code == sf::Keyboard::Space)) {
                    window2.close();
                }
        }
        window2.draw(spriteBackground);
        window2.draw(spriteEnd);
        window2.display();
        window2.clear();
    }
}

bool Engine::initEnemies() {
    std::vector<float> mountain;
    mountain = SmoothTerrain::getMaxYValues();

    glm::vec3 enemyScale = Settings::enemyScale;
    glm::vec3 enemyRotation = Settings::enemyRotation;

    srand(time(NULL));
    for(int i = 0; i < Settings::numEnemies; i++) {
        float enemyStartX = player->getPos().x + 8.0*i + 15;
        glm::vec3 enemyStart = glm::vec3(enemyStartX, mountain[enemyStartX * 4] + 4.0, player->getPos().z);
        //   player->getPos() + glm::vec3(15 + 10.0*i, 0.0, 0.0);
        glm::vec3 enemyAcc = Settings::enemyAcc;

        float speedX = (((rand()%200) / 100.0) + 2.0) * -1.0; // Value between 2.0 and 4.0
        glm::vec3 enemySpeed = Settings::playerSpeed * glm::vec3(speedX, 0.0, 0.0);

        std::unique_ptr<Enemy> enemy = std::unique_ptr<Enemy>(new Enemy(enemyStart, enemyScale, enemyRotation, enemyAcc, enemySpeed));

        enemies.push_back(std::move(enemy));
    }

    return true;
}

bool Engine::initBoss() {
    std::vector<float> mountain;
    mountain = SmoothTerrain::getMaxYValues();

    // Keeps the boss from hitting terrain
    glm::vec3 start = glm::vec3(Settings::bossStart.x, mountain[Settings::bossStart.x * 4] + 4.0, Settings::bossStart.z);
    Settings::bossStart = glm::vec3(start);

    boss = std::unique_ptr<Boss>(new Boss(start, Settings::bossScale, Settings::bossRotation));
    boss->setHealth(5);
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
    sun.reset(new Sun(glm::vec3(1.0, 1.0, 1.0), Settings::sunPos, Settings::sunSpot, Settings::ambienIntensity));
    return resetSun();
}

bool Engine::resetSun()
{
    GLuint color = glGetUniformLocation(shaderProgram, "sunLight.color");
    glUniform3f(color, sun->getColor().x, sun->getColor().y, sun->getColor().z);

    GLuint intensity = glGetUniformLocation(shaderProgram, "sunLight.ambientIntensity");
    glUniform1f(intensity, sun->getIntensity());

    GLuint direction = glGetUniformLocation(shaderProgram, "sunLight.direction");
    glUniform3f(direction, sun->getDirection().x, sun->getDirection().y, sun->getDirection().z);

    GLuint pos = glGetUniformLocation(shaderProgram, "sunLight.pos");
    glUniform3f(pos, Settings::sunPos.x, Settings::sunPos.y, Settings::sunPos.z);

    return (color >= 0 && intensity >= 0 && direction >= 0);
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

    resetSun();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    GLuint shadowMap = glGetUniformLocation(shaderProgram, "shadowMap");
    glUniform1i(shadowMap, 1);

    glCullFace(GL_BACK);

    drawProjectiles();

    drawTerrain();
    drawWorldObjects();
    drawPlayer();
    drawEnemies();
    drawBoss();
    drawSkybox();
}

void Engine::drawProjectiles()
{
    for (auto &p : projectiles) {
        if (p->getState() != DEAD)
            drawProjectile(*p);
    }
}

void Engine::drawProjectile(Projectile &p){

    // Render to the screen
    GLuint modelID = glGetUniformLocation(shaderProgram, "model");
    GLuint viewID = glGetUniformLocation(shaderProgram, "view");
    GLuint projID = glGetUniformLocation(shaderProgram, "proj");

    glm::mat4 model = p.getModel();
    glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(projID, 1, GL_FALSE, &projMatrix[0][0]);

    // get vertex buffer
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, p.getVertexBuffer());
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
    /*glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, p.getTextureBuffer());
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p.getElementBuffer());*/

    // override texture colors
    //GLuint uniColor = glGetUniformLocation(shaderProgram, "overrideColor");
    //glUniform3f(uniColor, 1.0f, 0.3f, 0.3f);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 12*3);

    glDisableVertexAttribArray(0);
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

    if (player->getState() != DEAD)
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

    glDisable(GL_TEXTURE_2D);

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


    // necessary but fucking ugly, TODO: FIX FIX FIX
    if (w.getTexture() >= 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, w.getTexture());
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, w.getTextureBuffer());
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

    // send normal buffer
    if (w.getNormalBuffer() >= 0) {
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, w.getNormalBuffer());
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, w.getElementBuffer());

    // override texture colors
    //GLuint uniColor = glGetUniformLocation(shaderProgram, "overrideColor");
    //glUniform3f(uniColor, 1.0f, 0.3f, 0.3f);

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
    if (player->getState() != DEAD)
        drawObject(*player);
}

void Engine::drawEnemies()
{
    for (auto &e : enemies) {
        if (e->getState() != DEAD)
            drawObject(*e);
    }
}

void Engine::drawBoss()
{
    if (boss->getState() != DEAD)
        drawObject(*boss);
}

void Engine::update()
{
    //set bounding box
    boundingBox();

    // draw single frame
    drawFrame();

    //check collision
    checkCollision();

    // swap buffers
    window.display();
}

void Engine::startGame()
{
    // Start game
    glm::vec3 eye = slide.getEye();
    glm::vec3 center = slide.getCenter();

    slide.setEye(eye + glm::vec3((Settings::playerStart.x+2.0-Settings::eye.x)/Settings::startCameraTurnSpeed,
                                 (Settings::playerStart.y-Settings::eye.y)/Settings::startCameraTurnSpeed,
                                 ((Settings::playerStart.z+10.0)-Settings::eye.z)/Settings::startCameraTurnSpeed));

    slide.setCenter(glm::vec3(player->getPos().x+2.0, player->getPos().y, player->getPos().z));

    // draw single frame
    drawFrame();

    // swap buffers
    window.display();

    // New gamestate
    if(eye.x >= 22.5)
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

    if(eye.x > player->getPos().x - 9)
    {
        slide.setEye(eye + glm::vec3(-0.1, 0.01, -0.09));
        slide.setCenter(player->getPos());
    }

    update();
}

void Engine::endGame()
{
    if(boss->getState() == DEAD)
        initEndMenu(true);
    else
        initEndMenu(false);
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
    window.setKeyRepeatEnabled(false);
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
                //std::cout << "STARTING" << std::endl;
                startGame();
                break;
            case GameState::ST_PLAYING:
                //std::cout << "PLAYING" << std::endl;
                playGame();
                break;
            case GameState::ST_BOSS:
                //std::cout << "BOSS" << std::endl;
                bossLvl();
                break;
            case GameState::ST_END:
                //std::cout << "GAME ENDED" << std::endl;
                endGame();
                break;
        }

        //window.display();
        accumulator += clock.restart();

    }
}

void Engine::handleKeyEvent(sf::Event event)
{
    if (event.key.code == sf::Keyboard::S) {
        std::unique_ptr<Projectile> projectile = std::unique_ptr<Projectile>(new Projectile(player->getPos(), player->getRotation()));
        projectiles.push_back(std::move(projectile));
    } else if (event.key.code == sf::Keyboard::Space) {
        if (player->getState() == ALIVE) {
            player->addAcc(glm::vec3(0.0f, 1.0f / 200.0, 0.0f));
            player->startAnimation();
        }
    } else if (event.key.code == sf::Keyboard::F) {
        player->setAnimationState(DYING);
        player->startAnimation();
    } else if (event.key.code == sf::Keyboard::Z) {
        player->changeTexture();
    }

    //std::cout << "pos (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
    //std::cout << "eye (" << eye.x << ", " << eye.y << ", " << eye.z << ")" << std::endl;
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

    for (auto &p : projectiles) {
        p->update();
    }

	sun->increaseIntensity(0.005);
    skybox->update(sun->getIntensity());

    for (auto &e : enemies) {
        // update the enemy only when the player is in range
        if(e->getPos().x - player->getPos().x < Settings::startEnemyUpdate) {
            if(e->getState() == ALIVE || e->getState() == DYING) {
                e->update();
            }
        }
    }

    player->update(gameState);

    if(boss->getPos().x - player->getPos().x <= Settings::startBossUpdate)
    {
        if(boss->getState() == ALIVE || boss->getState() == DYING)
            boss->update();
    }

    if(gameState.currentState == GameState::ST_PLAYING)
    {
        glm::vec3 center = glm::vec3(player->getPos().x + 2.0, player->getPos().y, player->getPos().z);
        glm::vec3 eye = glm::vec3(player->getPos().x + 2.0, player->getPos().y, Settings::playerStart.z + 10);

        center.y = std::max(3.5f, center.y);
        eye.y = std::max(3.5f, eye.y);

        slide.setCenter(center);
        slide.setEye(eye);
        //slide.setCenter(player->getPos());
        //slide.setEye(glm::vec3(player->getPos().x + 1.0, player->getPos().y, Settings::playerStart.z + 10));
    }

    // When in range of the boss, change game state to ST_BOSS
    if(gameState.currentState == GameState::ST_PLAYING && boss->getPos().x - player->getPos().x <= Settings::startBossStateRange)
    {
        gameState.currentState = GameState::ST_BOSS;
    }

	/*glm::vec3 center = glm::vec3(player->getPos().x + 3.0, player->getPos().y, player->getPos().z);
    glm::vec3 eye = glm::vec3(player->getPos().x + 3.0, player->getPos().y, Settings::playerStart.z + 10);
    center.y = std::max(1.5f, center.y);
    eye.y = std::max(1.5f, eye.y);
    slide.setCenter(center);
    slide.setEye(eye);-*/
}

void Engine::boundingBox() {
    //create/set bounding box for the world objects, such as the enemies

    for (auto &e : enemies) {
        glm::vec3 pos = e->getPos();
        glm::vec4 boundingBox = glm::vec4(pos.x - (0.5*e->getWidth()*Settings::enemyScale.z), pos.y - (0.5*e->getHeight()*Settings::enemyScale.y), (e->getWidth()*Settings::enemyScale.z), (e->getHeight()*Settings::enemyScale.y));
        e->setBoundingBox(boundingBox);
    }

    for (auto &p : projectiles) {
        glm::vec3 pos = p->getPos();
        glm::vec4 boundingBox = glm::vec4(pos.x - (0.5), pos.y - (0.25), (1), (0.5));
        p->setBoundingBox(boundingBox);
    }

    //create/set the bounding box for the bird
    glm::vec3 pos = player->getPos();
    glm::vec4 boundingBox = glm::vec4(pos.x - (0.7*player->getWidth()*Settings::playerScale.z), pos.y - (0.7*player->getHeight()*Settings::playerScale.y), (player->getWidth()*Settings::playerScale.x*0.7), (player->getHeight()*Settings::playerScale.y*0.7));
    player->setBoundingBox(boundingBox);

    //create/set the bounding box for the bird
    glm::vec3 posBoss = boss->getPos();
    glm::vec4 boundingBoxBoss = glm::vec4(posBoss.x - (0.7*boss->getWidth()*Settings::bossScale.z), posBoss.y - (0.7*boss->getHeight()*Settings::bossScale.y), (boss->getWidth()*Settings::bossScale.x*0.7), (boss->getHeight()*Settings::bossScale.y*0.7));
    boss->setBoundingBox(boundingBoxBoss);
}

void Engine::checkCollision() {
    glm::vec4 boundingBoxBird = player->getBoundingBox();
    int counter = 0;

    if (player->getState() == DEAD) {

        gameState.currentState = GameState::ST_END;
    }

    if(boss->getState() == DEAD) {
       gameState.currentState = GameState::ST_END;
    }

    //check for world object collision with the bird
    for (auto &e : enemies) {

        // Check collision with bird
        glm::vec4 boundingBoxWorld = e->getBoundingBox();
        if (e->getState() != DYING &&
            boundingBoxBird.x < boundingBoxWorld.x + boundingBoxWorld.z &&
            boundingBoxBird.x + boundingBoxBird.z > boundingBoxWorld.x &&
            boundingBoxBird.y < boundingBoxWorld.y + boundingBoxWorld.w &&
            boundingBoxBird.y + boundingBoxBird.w > boundingBoxWorld.y)
        {
            player->setState(DYING);
            e->setState(DYING);
        }

        // Check collision with terrain
        float startEnemy = floor(e->getBoundingBox().x); //the min x-value pos of the bird
        float endEnemy = floor(e->getBoundingBox().x + e->getBoundingBox().z); //the max x-value pos of the bird
        float detail = 0.25;
        std::vector<float> mountain;
        mountain = SmoothTerrain::getMaxYValues();
        for(startEnemy; startEnemy < endEnemy; startEnemy += detail) {
            //initiate the counter with some correction
            int counter = floor(startEnemy*4+2);
            //check if the y-value of the mountain at the certain x is higher or equal to the y-value of the bird
            if(mountain[counter] >= e->getBoundingBox().y) {
                //e->setState(DYING);
            }
        }

        for (auto &p : projectiles) {
            glm::vec4 boundingBoxProj = p->getBoundingBox();
            if (p->getState() != DEAD &&
                boundingBoxProj.x < boundingBoxWorld.x + boundingBoxWorld.z &&
                boundingBoxProj.x + boundingBoxProj.z > boundingBoxWorld.x &&
                boundingBoxProj.y < boundingBoxWorld.y + boundingBoxWorld.w &&
                boundingBoxProj.y + boundingBoxProj.w > boundingBoxWorld.y)
            {
                std::cout << "Dead piggy" << std::endl;
                e->setState(DYING);
                p->setState(DEAD);
            }
            if(p->getPos().x - player->getPos().x > 20.0)
                p->setState(DEAD);
        }

        counter++;
    }

    //check for terrain collision
    float startBird = floor(player->getBoundingBox().x); //the min x-value pos of the bird
    float endBird = floor(player->getBoundingBox().x + player->getBoundingBox().z); //the max x-value pos of the bird
    float detail = 0.25;
    std::vector<float> mountain;

    mountain = SmoothTerrain::getMaxYValues();

    for (startBird; startBird < endBird; startBird += detail) {
        //initiate the counter with some correction
        int counter = floor(startBird*4+2);
        //check if the y-value of the mountain at the certain x is higher or equal to the y-value of the bird
        if(mountain[counter] >= player->getBoundingBox().y) {
            player->setState(DYING);
        }
    }

    //check for boss collision
    if(boss->getState() == ALIVE) {
        glm::vec4 boundingBoxBoss = boss->getBoundingBox();
        for (auto &p : projectiles) {
            glm::vec4 boundingBoxProjectile = p->getBoundingBox();
            if (p->getState() != DEAD &&
                boundingBoxProjectile.x < boundingBoxBoss.x + boundingBoxBoss.z &&
                boundingBoxProjectile.x + boundingBoxProjectile.z > boundingBoxBoss.x &&
                boundingBoxProjectile.y < boundingBoxBoss.y + boundingBoxBoss.w &&
                boundingBoxProjectile.y + boundingBoxProjectile.w > boundingBoxBoss.y)
            {
                p->setState(DEAD);
                boss->setHealth(boss->getHealth() - 1);

                std::cout << "Boss is hit!! " << boss->getHealth() << " lives left" << std::endl;

                if(boss->getHealth() <= 0)
                    boss->setState(DYING);
            }
        }
    }
}
