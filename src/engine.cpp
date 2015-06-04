#include "engine.h"

Engine::Engine(): slide(Settings::eye, glm::vec3(0.0, 0.0, 0.0), 1.0), terrain(NULL)
{
    //ctor
}

Engine::~Engine()
{
    //dtor
    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &vao);

    if (terrain != NULL)
        delete terrain;
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
    glEnable(GL_CULL_FACE);

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
    terrain = new FlatTerrain();
    terrain->generateTerrain(50, 20);

    // load test object
    wobjs.emplace_back(Settings::playerStart, Settings::playerScale, Settings::playerRotation, "resources/monkey.obj");

    return true;
}

bool Engine::initSun()
{
    GLuint color = glGetUniformLocation(shaderProgram, "sunLight.color");
    glUniform3f(color, 1.0f, 1.0f, 1.0f);

    GLuint intensity = glGetUniformLocation(shaderProgram, "sunLight.ambientIntensity");
    glUniform1f(intensity, 0.75f);

    GLuint direction = glGetUniformLocation(shaderProgram, "sunLight.direction");
    glUniform3f(direction, -glm::normalize(Settings::sunPos).x, -glm::normalize(Settings::sunPos).y, -glm::normalize(Settings::sunPos).z);

    return (color && intensity && direction);
}

void Engine::updatePlayer()
{
    if(wobjs.size() > 0)
        wobjs[0].setPos(glm::vec3(wobjs[0].getPos().x+0.0001, wobjs[0].getPos().y, wobjs[0].getPos().z));
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
}

void Engine::drawTerrain()
{
    // no model matrix, defaults  to 0,0
    glm::mat4 modelMatrix;
    glm::mat4 MVP = projMatrix * viewMatrix * modelMatrix;

    GLuint matrixID = glGetUniformLocation(shaderProgram, "MVP");
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);  // bind matrix to shader

    GLuint uniColor = glGetUniformLocation(shaderProgram, "overrideColor");
    glUniform3f(uniColor, 1.0f, 1.0f, 1.0f);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, terrain->getVertexBuffer());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    GLint texAttrib = glGetAttribLocation(shaderProgram, "inCoord");
    glEnableVertexAttribArray(texAttrib);
    glBindBuffer(GL_ARRAY_BUFFER, terrain->getTextureBuffer());
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_TRIANGLES, 0, terrain->getTerrainSize());

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(texAttrib);
}

void Engine::drawWorldObjects()
{
    GLuint uniColor = glGetUniformLocation(shaderProgram, "overrideColor");
    for (unsigned int i = 0; i < wobjs.size(); i++) {
        //glm::mat4 modelMatrix = glm::translate(modelMatrix, wobjs[i].getPos());
        glm::mat4 modelMatrix;
        modelMatrix = glm::rotate(glm::scale(glm::translate(modelMatrix, wobjs[i].getPos()), wobjs[i].getScale()), wobjs[i].getRotation(), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 MVP = projMatrix * viewMatrix * modelMatrix;

        GLuint matrixID = glGetUniformLocation(shaderProgram, "MVP");
        glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);  // bind matrix to shader

        // get vertex buffer
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, wobjs[i].getVertexBuffer());
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // get normal buffer
        GLint normalAttrib = glGetAttribLocation(shaderProgram, "inNormal");
		glEnableVertexAttribArray(normalAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, wobjs[i].getNormalBuffer());
		glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wobjs[i].getElementBuffer());

        // override texture colors
        glUniform3f(uniColor, 1.0f, 0.3f, 0.3f);
        glDrawElements(GL_TRIANGLES, wobjs[i].getObjectSize(), GL_UNSIGNED_INT, (void*)0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(normalAttrib);
    }
}

void Engine::mainLoop() {

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

        // update player object
        updatePlayer();

        // draw single frame
        drawFrame();

        // swap buffers
        window.display();
    }
}

// tmp debug function
void Engine::loadObject(const std::string &path)
{
    std::string err = tinyobj::LoadObj(shapes, material, path.c_str());
    if (err.length()) {
        std::cerr << err << std::endl;
        return;
    }

    // succesfully loaded, load data into VBO
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.positions.size() * sizeof(float), &shapes[0].mesh.positions[0], GL_STATIC_DRAW);

    // create index buffer
    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[0].mesh.indices.size() * sizeof(unsigned int), &shapes[0].mesh.indices[0], GL_STATIC_DRAW);
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
    }

    std::cout << "eye (" << eye.x << ", " << eye.y << ", " << eye.z << ")" << std::endl;
    slide.setEye(eye);
    slide.setCenter(center);
}


