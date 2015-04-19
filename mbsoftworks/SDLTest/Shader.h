#pragma once
#include <string>
#include <gl\glew.h>
using namespace std;

class Shader
{
public:
    bool loadShader(string sFile, int shaderType);
    void deleteShader();

    bool isLoaded();
    unsigned int getShaderID();

    Shader();

private:
    unsigned int shaderID;
    int iType;
    bool bLoaded;
};