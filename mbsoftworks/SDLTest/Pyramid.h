#pragma once
#include <gl\glew.h>
#include <vector>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class Pyramid
{
public:
    void init();
    inline GLuint& getVAO() { return VAO; }
    inline int numOfVertices() { return pyramidVerts.size(); }
    void draw();
    void cleanUp();

private:
    vector<vec3> pyramidVerts;
    vector<vec3> pyramidColors;

    GLuint VBO[2];
    GLuint VAO;
};

void Pyramid::init()
{
    pyramidVerts.emplace_back(0.0f, 5.0f, 0.0f);
    pyramidVerts.emplace_back(-3.f, 0.0f, 3.f);
    pyramidVerts.emplace_back(3.0f, 0.f, 3.0f);

    pyramidVerts.emplace_back(0.0f, 5.0f, 0.0f);
    pyramidVerts.emplace_back(3.f, 0.0f, -3.f);
    pyramidVerts.emplace_back(-3.f, 0.0f, -3.f);

    pyramidVerts.emplace_back(0.0f, 5.0f, 0.0f);
    pyramidVerts.emplace_back(-3.f, 0.0f, -3.f);
    pyramidVerts.emplace_back(-3.f, 0.0f, 3.f);

    pyramidVerts.emplace_back(0.0f, 5.0f, 0.0f);
    pyramidVerts.emplace_back(3.f, 0.0f, 3.f);
    pyramidVerts.emplace_back(3.f, 0.0f, -3.f);

    pyramidVerts.emplace_back(-3.0f, 0.0f, 3.0f);
    pyramidVerts.emplace_back(-3.f, 0.0f, -3.f);
    pyramidVerts.emplace_back(3.f, 0.0f, 3.f);

    pyramidVerts.emplace_back(3.0f, 0.0f, 3.0f);
    pyramidVerts.emplace_back(-3.f, 0.0f, -3.f);
    pyramidVerts.emplace_back(3.f, 0.0f, -3.f);

    pyramidColors.resize(pyramidVerts.size());
    float* data = (float*)pyramidColors.data();
    for (size_t i = 0; i < 4; i++)
    {

        *(data + i * 9) = 1.0f; *(data + i * 9 + 1) = 0.0f; *(data + i * 9 + 2) = 0.0f;
        if (i < 2)
        {
            *(data + i * 9 + 1) = 0.0f; *(data + i * 9 + 4) = 1.0f; *(data + i * 9 + 5) = 0.0f;
            *(data + i * 9 + 2) = 0.0f; *(data + i * 9 + 7) = 0.0f; *(data + i * 9 + 8) = 1.0f;
        }
        else
        {
            *(data + i * 9 + 2) = 0.0f; *(data + i * 9 + 7) = 1.0f; *(data + i * 9 + 8) = 0.0f;
            *(data + i * 9 + 1) = 0.0f; *(data + i * 9 + 4) = 0.0f; *(data + i * 9 + 5) = 1.0f;
        }
    }
    pyramidColors[12] = vec3(1.0f, 0.0f, 0.0f);
    pyramidColors[13] = vec3(0.0f, 1.0f, 0.0f);
    pyramidColors[14] = vec3(0.0f, 0.0f, 1.0f);

    pyramidColors[15] = vec3(1.0f, 0.0f, 0.0f);
    pyramidColors[16] = vec3(0.0f, 1.0f, 0.0f);
    pyramidColors[17] = vec3(0.0f, 0.0f, 1.0f);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(2, VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, pyramidVerts.size() * sizeof(vec3), &pyramidVerts.front(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, pyramidColors.size() * sizeof(vec3), &pyramidColors.front(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void Pyramid::cleanUp()
{
    glDeleteBuffers(2, VBO);
    glDeleteVertexArrays(1, &VAO);
}

void Pyramid::draw()
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, numOfVertices());
}
