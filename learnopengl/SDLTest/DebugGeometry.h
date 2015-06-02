#pragma once
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ShaderProgram.h"

class DebugGeometry
{
public:
    DebugGeometry();
    void init();
    void setupViewProjection( glm::mat4& view, glm::mat4& projection);
    void renderLine( const glm::vec3& a, const glm::vec3& b, const glm::vec3& color = glm::vec3(1,1,1));
    void renderBox(const glm::mat4& model = glm::mat4(1), const glm::vec3& color = glm::vec3(1,1,1));

private:
    GLuint          lineVAO, boxVAO;
    ShaderProgram shader;
};