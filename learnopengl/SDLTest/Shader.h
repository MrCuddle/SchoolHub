#pragma once
#include <gl/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

class Shader
{
public:
    bool loadShader(std::string shaderSourcePath, GLenum shaderType)
    {
        fstream file(shaderSourcePath);
        string line, source;
        while (getline(file, line))
        {
            source += line + '\n';
        }
        shaderID = glCreateShader(shaderType);
        const GLchar * glSource = &source.front();
        glShaderSource(shaderID, 1, &glSource, NULL);
        glCompileShader(shaderID);

        GLint success;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            GLchar infoLog[512];
            glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
            cout << infoLog << "\n";
        }
        return true;
    }
    GLuint inline getID() { return shaderID; }
    void inline deleteShader() { glDeleteShader(shaderID); }
private:
    GLuint shaderID;
};