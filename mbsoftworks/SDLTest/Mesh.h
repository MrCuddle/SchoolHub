#pragma once
#include <gl\glew.h>
#include <vector>
#include <glm/glm.hpp>
#include "ObjLoader.h"
#include <string>

using namespace std;
using namespace glm;
class Mesh
{
public:
    void init(string filename);
    void draw();


private:
    vector<vec4> vertices;
    vector<vec3> normals;
    vector<GLushort> elements;
    GLuint VBO[2], VAO;
};

void Mesh::init(string filename)
{
    ObjLoader::loadObj(filename.c_str(), vertices, normals, elements);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(2, VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(vec4), &vertices.front(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(GLushort), &elements.front(), GL_STATIC_DRAW);

}

void Mesh::draw()
{
    //glBindVertexArray(VAO);
    //glDrawElements(GL_TRIANGLES, elements.size(), GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
    glDrawElements(GL_TRIANGLES, elements.size(), GL_UNSIGNED_SHORT, 0);
}
