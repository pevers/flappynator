#include "animation.h"

Animation::Animation(bool loop, unsigned int startFrame, unsigned int numFrames, std::string basePath) :
    loop(loop), startFrame(startFrame), numFrames(numFrames), basePath(basePath), frameRate(40), m_hasNormals(false), m_hasTexture(false)
{
    if (!load()) {
        std::cerr << "could not load animation " << basePath << std::endl;
    }

    frame = startFrame;
    clock.restart();
}

Animation::~Animation()
{
    std::cout << "watch out, delete called " << std::endl;
    //glDeleteTextures(1, textures);
}

void Animation::getDistances(float &width, float &height) {
    //calculate max and min coordinate
    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::min();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::min();

    for (size_t i = 0; i < 1; i++) {
        for (size_t v = 0; v < frames[frame][0].mesh.positions.size() / 3; v++) {
            if(minX > frames[frame][0].mesh.positions[3*v+2])
                minX = frames[frame][0].mesh.positions[3*v+2];
            if(maxX < frames[frame][0].mesh.positions[3*v+2])
                maxX = frames[frame][0].mesh.positions[3*v+2];
            if(minY > frames[frame][0].mesh.positions[3*v+1])
                minY = frames[frame][0].mesh.positions[3*v+1];
            if(maxY < frames[frame][0].mesh.positions[3*v+1])
                maxY = frames[frame][0].mesh.positions[3*v+1];
        }
    }

    width = std::abs(maxX - minX);
    height = std::abs(maxY - minY);
}

bool Animation::load()
{
    frames.resize(numFrames);
    materials.resize(numFrames);

    // debug, load all objects
    for (int i = 0; i < numFrames; i++) {
        std::string path = basePath + std::to_string(i+1) + ".obj";
        std::string err = tinyobj::LoadObj(frames[i], materials[i], path.c_str());
        if (err.length()) {
            std::cerr << err << std::endl;
            return false;
        }

        if (!frames[i][0].mesh.normals.size()) {
            std::cout << "warning, no normals in object " << std::endl;
        }
    }

    if (frames[0][0].mesh.texcoords.size() > 0) {
        glGenTextures(2, textures);

        std::string imagePath = basePath + "texture.png";
        std::string alt_imagePath = basePath + "alt_texture.png";
        // LOAD debug animation object
        int texWidth, texHeight;
        int alt_texWidth, alt_texHeight;
        unsigned char *image = SOIL_load_image(imagePath.c_str(), &texWidth, &texHeight, 0, SOIL_LOAD_RGBA);
        if (!image) {
            std::cerr << "could not load image " << std::endl;
        } else {
            glBindTexture(GL_TEXTURE_2D, textures[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            SOIL_free_image_data(image);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        unsigned char *image_alt = SOIL_load_image(alt_imagePath.c_str(), &alt_texWidth, &alt_texHeight, 0, SOIL_LOAD_RGBA);
        if (!image_alt) {
            std::cerr << "could not load alternative image " << std::endl;
        } else {
            glBindTexture(GL_TEXTURE_2D, textures[1]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, alt_texWidth, alt_texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_alt);
            SOIL_free_image_data(image_alt);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        selected_texture = 0;
    }

    return true;
}

void Animation::update(GLuint &vbo, GLuint &elementBuffer, GLuint &normalBuffer, GLuint &vboTextureBuffer)
{
    // interpolate frames if animation is still busy
    if (frame != startFrame || loop) {
        sf::Time elapsed = clock.getElapsedTime();
        unsigned int newFrame = frame;
        if (elapsed.asMilliseconds() > (1000.0 / frameRate)) {
            // update frame
            newFrame = nextFrame(newFrame);
            clock.restart();
        }

        if (newFrame != frame) {
            bindBuffer(vbo, elementBuffer, normalBuffer, vboTextureBuffer);

            frame = newFrame;
        }
    }
}

void Animation::bindBuffer(GLuint &vbo, GLuint &elementBuffer, GLuint &normalBuffer, GLuint &vboTextureBuffer)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, frames[frame][0].mesh.positions.size() * sizeof(float), &frames[frame][0].mesh.positions[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, frames[frame][0].mesh.indices.size() * sizeof(unsigned int), &frames[frame][0].mesh.indices[0], GL_STATIC_DRAW);

    if (frames[frame][0].mesh.normals.size() > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glBufferData(GL_ARRAY_BUFFER, frames[frame][0].mesh.normals.size() * sizeof(float), &frames[frame][0].mesh.normals[0], GL_STATIC_DRAW);
    }

    if (frames[frame][0].mesh.texcoords.size() > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, vboTextureBuffer);
        glBufferData(GL_ARRAY_BUFFER, frames[frame][0].mesh.texcoords.size() * sizeof(glm::vec2), &frames[frame][0].mesh.texcoords[0], GL_STATIC_DRAW);
    }
}

void Animation::setStartFrame(unsigned int frame)
{
    startFrame = frame;
}

unsigned int Animation::getStartFrame()
{
    return startFrame;
}

unsigned int Animation::nextFrame(unsigned int f)
{
    f++;
    if (f > (frames.size() - 1))
        return 0;
    return f;
}

void Animation::startAnimation()
{
    // reset animation
    frame = startFrame;
    frame = nextFrame(frame);
}

void Animation::setLoop(bool loop)
{
    this->loop = loop;
}

bool Animation::isLooped()
{
    return loop;
}

unsigned int Animation::getObjectSize()
{
    return frames[frame][0].mesh.indices.size();
}

GLuint Animation::getTexture()
{
    return textures[selected_texture];
}

void Animation::changeTexture()
{
    selected_texture = (selected_texture + 1)%2;
}

bool Animation::hasNormals()
{
    return m_hasNormals;
}

bool Animation::hasTexture()
{
    return m_hasTexture;
}
