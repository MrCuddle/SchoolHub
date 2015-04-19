#pragma once

#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <vector>
#include <gl/glew.h>

using namespace std;
class ObjLoader
{
public:
    static void loadObj(const char* fileName, vector<glm::vec4>& vertices, vector<glm::vec3>& normals, vector<GLushort>& elements);
};
