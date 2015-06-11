#include "skybox.h"

GLfloat skyboxVertices[] = {
    // Positions
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

Skybox::Skybox(const std::string& posXFileName,
            const std::string& negXFileName,
            const std::string& posYFileName,
            const std::string& negYFileName,
            const std::string& posZFileName,
            const std::string& negZFileName)
{
    //ctor
    fileNames[0] = posXFileName;
    fileNames[1] = negXFileName;
    fileNames[2] = posYFileName;
    fileNames[3] = negYFileName;
    fileNames[4] = posZFileName;
    fileNames[5] = negZFileName;
}

Skybox::~Skybox()
{
    //dtor
    glDeleteTextures(1, &textureObj);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
}

constexpr GLenum Skybox::types[6];

bool Skybox::load()
{
    glGenTextures(1, &textureObj);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureObj);

    for (int i = 0; i < 6; i++) {
        int texWidth, texHeight;
        unsigned char *image = SOIL_load_image(fileNames[i].c_str(), &texWidth, &texHeight, 0, SOIL_LOAD_RGB);
        if (!image) {
            std::cerr << "could not load one of the skybox images " << fileNames[i] << std::endl;
            return false;
        }

        glTexImage2D(this->types[i], 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        SOIL_free_image_data(image);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // load shaders
    return loadShaders() && loadVAO();
}

bool Skybox::loadShaders()
{
    shaderProgram = glCreateProgram();
    vertexShader = ShaderProgram::loadShaderFromFile("shaders/skyboxVertex.shader", GL_VERTEX_SHADER);
    fragmentShader = ShaderProgram::loadShaderFromFile("shaders/skyboxFragment.shader", GL_FRAGMENT_SHADER);

    if (!vertexShader || !fragmentShader) return false;

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    return true;
}

bool Skybox::loadVAO()
{
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);

    return true;
}

glm::mat4 Skybox::getModel()
{
    glm::mat4 model;
    return glm::scale(model, glm::vec3(80.0, 80.0, 80.0));
}

GLuint Skybox::getTexture()
{
    return textureObj;
}

int Skybox::getSize()
{
    return 36;  // static :p
}

void Skybox::bind(GLenum textureUnit)
{
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureObj);
}

GLuint Skybox::getShaderProgram()
{
    return shaderProgram;
}

GLuint Skybox::getVAO()
{
    return skyboxVAO;
}


