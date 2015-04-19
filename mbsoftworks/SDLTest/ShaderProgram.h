#pragma once
#include "shader.h"
class ShaderProgram
{
public:
    void createProgram();
    void deleteProgram();

    bool addShader(Shader* shader);
    bool linkProgram();

    void useProgram();

    unsigned int getProgramID();

    ShaderProgram();

private:
    unsigned int programID;
    bool bLinked;
};
