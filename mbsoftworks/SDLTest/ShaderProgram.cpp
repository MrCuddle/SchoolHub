#include "ShaderProgram.h"

void ShaderProgram::createProgram()
{
    programID = glCreateProgram();
}

void ShaderProgram::deleteProgram()
{
    glDeleteProgram(programID);
}

bool ShaderProgram::addShader(Shader* shader)
{
    if (shader->isLoaded()){
        glAttachShader(programID, shader->getShaderID());
        return true;
    }
    else return false;
}

bool ShaderProgram::linkProgram()
{
    glLinkProgram(programID);
    GLint linkStatus;
    glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus);
    bLinked = linkStatus == GL_TRUE;
    return bLinked;
}

void ShaderProgram::useProgram()
{
    glUseProgram(programID);
}

unsigned int ShaderProgram::getProgramID()
{
    return programID;
}

ShaderProgram::ShaderProgram()
{

}