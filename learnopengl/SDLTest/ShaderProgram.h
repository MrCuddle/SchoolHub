#pragma once
#include <gl/glew.h>
#include "Shader.h"
#include <string>


class ShaderProgram
{
public:
    bool loadShader(std::string vsPath, std::string fsPath)
    {
        Shader vertexShader, fragmentShader;
        vertexShader.loadShader(vsPath, GL_VERTEX_SHADER);
        fragmentShader.loadShader(fsPath, GL_FRAGMENT_SHADER);
        programID = glCreateProgram();
        glAttachShader(programID, vertexShader.getID());
        glAttachShader(programID, fragmentShader.getID());
        glLinkProgram(programID);
        vertexShader.deleteShader();
        fragmentShader.deleteShader();
        return true;
    }
    void Use()
    {
        glUseProgram(programID);
    }

private:
    GLuint programID;

};