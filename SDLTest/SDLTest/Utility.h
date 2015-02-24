#pragma once
#include <gl/glew.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

GLuint LoadShaders(const char* vertexFilePath, const char* fragmentFilePath);