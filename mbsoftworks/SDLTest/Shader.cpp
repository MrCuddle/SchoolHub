#include "Shader.h"
#include <fstream>
#include <vector>
#include <iostream>

bool Shader::loadShader(string sFile, int shaderType)
{
    fstream fileIn(sFile);
    string line;
    vector<string> lines;
    while (getline(fileIn, line)){
        lines.push_back(line + '\n');
    }
    const GLchar ** sProgram = new const char*[lines.size()];

    for (size_t i = 0; i < lines.size(); i++){
        sProgram[i] = lines[i].c_str();
    }
    shaderID = glCreateShader(shaderType);

    glShaderSource(shaderID, lines.size(), sProgram, nullptr);
    glCompileShader(shaderID);
    delete[] sProgram;

    int compilationStatus = 5;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compilationStatus);

    if (compilationStatus == GL_FALSE){
        GLint logSize = 0;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logSize);

        vector<GLchar> errorLog(logSize);
        glGetShaderInfoLog(shaderID, logSize, &logSize, &errorLog[0]);
        for(GLchar c : errorLog)
            cout << c;

        return false;
    }
    iType = shaderType;
    bLoaded = true;
    return true;
}

void Shader::deleteShader()
{
    glDeleteShader(shaderID);
}

bool Shader::isLoaded()
{
    return bLoaded;
}

unsigned int Shader::getShaderID()
{
    return shaderID;
}

Shader::Shader()
{

}
