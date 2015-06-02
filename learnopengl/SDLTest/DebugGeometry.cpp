#include "DebugGeometry.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include <vector>

DebugGeometry::DebugGeometry()
{
}

void DebugGeometry::init()
{
    GLfloat lineVertices[]
    {
        0.f, 0.f, -0.5f,
        0.f, 0.f, 0.5f
    };

    glGenVertexArrays( 1, &lineVAO );
    glBindVertexArray( lineVAO );

    GLuint VBO;
    glGenBuffers( 1, &VBO );
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( lineVertices ), &lineVertices, GL_STATIC_DRAW );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GLfloat ), (GLvoid*)0 );
    glBindVertexArray( 0 );


    std::vector<GLfloat> boxVertices
    {
        -0.5, -0.5, 0.5,     //near bl 0
        0.5, 0.5, 0.5,       //near tr 1
        -0.5, 0.5, 0.5,      //near tl 2
        0.5, -0.5, 0.5,      //near br 3
        0.5, -0.5, -0.5,     //far br 4
        -0.5, 0.5, -0.5,     //far tl 5
        0.5, 0.5, -0.5,      //far tr 6
        -0.5, -0.5, -0.5     //far bl 7
    };

     std::vector<GLuint> boxIndices{
        0, 1, 2,
        0, 1, 3,
        3, 6, 1,
        3, 6, 4,
        4, 5, 6,
        4, 5, 7,
        7, 2, 5,
        7, 2, 0,
        2, 6, 5,
        2, 6, 1,
        7, 3, 0,
        7, 3, 4
    };

    glGenVertexArrays( 1, &boxVAO );
    glBindVertexArray( boxVAO );
    GLuint boxVBO, boxEBO;
    glGenBuffers( 1, &boxVBO );
    glBindBuffer( GL_ARRAY_BUFFER, boxVBO );
    glBufferData( GL_ARRAY_BUFFER, boxVertices.size() * sizeof( GLfloat ), &boxVertices[0], GL_STATIC_DRAW );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GLfloat ), (GLvoid*)0 );
    glGenBuffers( 1, &boxEBO );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, boxEBO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, boxIndices.size() * sizeof( GLuint ), &boxIndices[0], GL_STATIC_DRAW );
    glBindVertexArray( 0 );

    shader.loadShader( "debugVs.glsl", "debugFs.glsl" );
}

void DebugGeometry::setupViewProjection( glm::mat4& view, glm::mat4& projection)
{
    shader.Use();
    glUniformMatrix4fv( glGetUniformLocation( shader.getID(), "view" ), 1, GL_FALSE, glm::value_ptr( view ) );
    glUniformMatrix4fv( glGetUniformLocation( shader.getID(), "projection" ), 1, GL_FALSE, glm::value_ptr( projection ) );
}

void DebugGeometry::renderLine(const glm::vec3& a, const glm::vec3& b, const glm::vec3& color )
{
    glm::mat4 model = glm::mat4();
    glm::vec3 identity( 0, 0, -1.0 );
    glm::vec3 line( b - a );

    float length = glm::length( line );
    line = glm::normalize( line );
    glm::vec3 center = ( a + b ) * 0.5f;
    float dotProd = glm::dot( identity, line );
    glm::vec3 crossProd = glm::normalize( glm::cross( identity, line ) );
    float theta = glm::acos( dotProd );
    model = glm::translate( center );

    if( fabsf( dotProd ) < 1.0f )
    {
        model *= glm::toMat4( glm::angleAxis( theta, crossProd ) );
    }
    model *= glm::scale( glm::vec3( 1, 1, length ) );

    shader.Use();
    glUniformMatrix4fv( glGetUniformLocation( shader.getID(), "model" ), 1, GL_FALSE, glm::value_ptr( model ) );
    glUniform3f( glGetUniformLocation( shader.getID(), "color" ), color.x, color.y, color.z );
    glBindVertexArray( lineVAO );
    glDrawArrays( GL_LINES, 0, 2 );
}

void DebugGeometry::renderBox(const glm::mat4& model, const glm::vec3& color)
{
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    //model *= glm::toMat4(glm::angleAxis(glm::radians(45.f), glm::vec3(0,1,0)));
    shader.Use();
    glUniformMatrix4fv( glGetUniformLocation( shader.getID(), "model" ), 1, GL_FALSE, glm::value_ptr( model ) );
    glUniform3f( glGetUniformLocation( shader.getID(), "color" ), color.x, color.y, color.z );
    glBindVertexArray( boxVAO );
    glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0 );
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}