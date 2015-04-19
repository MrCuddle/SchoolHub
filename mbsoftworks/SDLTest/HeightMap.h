#pragma once
#include <gl\glew.h>
#include <vector>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class HeightMap
{
public:
    void init();
    inline GLuint& getVAO() { return VAO; }
    inline int elementCount(){ return gridSizeX * (gridSizeY - 1) * 2 + gridSizeY - 1; }
    void draw();
    void cleanUp();
private:
    GLuint VAO;
    GLuint heightMapDataVBO;
    GLuint indicesVBO;
    vector<vec3> heightMapData;
    static const int gridSizeX = 4;
    static const int gridSizeY = 4;
};

void HeightMap::init()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &heightMapDataVBO);
    glGenBuffers(1, &indicesVBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, heightMapDataVBO);

    float heights[gridSizeX*gridSizeY] =
    {
        4.0f, 2.0f, 3.0f, 1.0f,
        3.0f, 5.0f, 8.0f, 2.0f,
        7.0f, 10.f, 12.0f, 6.0f,
        4.0f, 6.0f, 8.0f, 3.0f
    };

    heightMapData.resize(gridSizeX*gridSizeY);
    float sizeX = 40.f, sizeZ = 40.f;
    for (int i = 0; i < gridSizeX * gridSizeY; i++)
    {
        float x = float(i%gridSizeX), z = float(i / gridSizeX);
        heightMapData[i] = vec3(
            -sizeX / 2 + sizeX*x / float(gridSizeX - 1),
            heights[i],
            -sizeZ / 2 + sizeZ*z / float(gridSizeY - 1)
            );
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*heightMapData.size(), &heightMapData.front(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVBO);

    int indices[] =
    {
        0, 4, 1, 5, 2, 6, 3, 7, 16,
        4, 8, 5, 9, 6, 10, 7, 11, 16,
        8, 12, 9, 13, 10, 14, 11, 15
    };

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(heightMapData.size());
}

void HeightMap::cleanUp()
{
    glDeleteBuffers(1, &heightMapDataVBO);
    glDeleteBuffers(1, &indicesVBO);
    glDeleteVertexArrays(1, &VAO);
}

void HeightMap::draw()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLE_STRIP, elementCount(), GL_UNSIGNED_INT, 0);
}
