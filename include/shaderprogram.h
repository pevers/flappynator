#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <iostream>
#include <string>
#include <fstream>
#include <GL/glew.h>

class ShaderProgram
{
public:
    static GLuint loadShaderFromFile(std::string path, GLenum shaderType)
    {
        GLuint shaderProgram = -1;
        std::string shaderString;
        std::ifstream sourceFile(path.c_str());

        if (sourceFile)
        {
            shaderString.assign((std::istreambuf_iterator<char>(sourceFile)), std::istreambuf_iterator<char>());
            shaderProgram = glCreateShader(shaderType);

            const GLchar *shaderSource = shaderString.c_str();
            glShaderSource(shaderProgram, 1, (const GLchar**)&shaderSource, NULL);

            //Compile shader source
            glCompileShader(shaderProgram);

            //Check shader for errors
            GLint shaderCompiled = GL_FALSE;
            glGetShaderiv(shaderProgram, GL_COMPILE_STATUS, &shaderCompiled);
            if( shaderCompiled != GL_TRUE )
            {
                std::cerr << "unable to compile shader " << shaderProgram << " with source path " << shaderSource << std::endl;
                //printShaderLog(shaderID);
                glDeleteShader(shaderProgram);
                return -1;
            }
        }
        else
        {
            std::cerr << "unable to open file " << path.c_str() << std::endl;;
            return -1;
        }

        return shaderProgram;
    }
};

#endif // SHADERPROGRAM_H
