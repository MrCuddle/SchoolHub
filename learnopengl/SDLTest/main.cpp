/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL, SDL OpenGL, GLEW, standard IO, and strings
#include <SDL.h>
#include <gl\glew.h>
#include <SDL_opengl.h>
#include <gl\glu.h>
#include <iostream>
#include "ShaderProgram.h"
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <type_traits>
#include <cstring>
#include <vector>
#include "Lights.h"

//Starts up SDL, creates window, and initializes OpenGL
bool init();

//Initializes rendering program and clear color
bool initGL();

//Input handler
void handleKeys(unsigned char key, int x, int y);

//Per frame update
void update();

//Renders quad to the screen
void render();

//Frees media and shuts down SDL
void close();

//Shader loading utility programs
void printProgramLog(GLuint program);
void printShaderLog(GLuint shader);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//OpenGL context
SDL_GLContext gContext;

//Render flag
bool gRenderQuad = true;

//Graphics program

using namespace std;
const int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;
bool init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
        success = false;
    }
    else
    {
        SDL_ShowCursor(SDL_DISABLE);
        //Use OpenGL 4.1 core
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        //Create window
        gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
        if (gWindow == NULL)
        {
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            //Create context
            gContext = SDL_GL_CreateContext(gWindow);
            if (gContext == NULL)
            {
                printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                //Initialize GLEW
                glewExperimental = GL_TRUE;
                GLenum glewError = glewInit();
                if (glewError != GLEW_OK)
                {
                    printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
                }

                //Use Vsync
                if (SDL_GL_SetSwapInterval(1) < 0)
                {
                    printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
                }

                //Initialize OpenGL
                if (!initGL())
                {
                    printf("Unable to initialize OpenGL!\n");
                    success = false;
                }
            }
        }
    }

    return success;
}

GLuint VAO, textureID, normalMapID, lightVAO, depthMapFBO, depthMap, floorVAO;
const int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
ShaderProgram lightingShader, lampShader, firstPassShader, depthOnQuadShader;
std::vector<glm::vec3> cubePositions = {
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(2.0f, 5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f, 3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f, 2.0f, -2.5f),
    glm::vec3(1.5f, 0.2f, -1.5f),
    glm::vec3(-1.3f, 1.0f, -1.5f)
};

void generateFloor()
{
    float lengthWidth = 5.0;
    std::vector<GLfloat> vertices =
    {
        -lengthWidth, -0.5f, -lengthWidth, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        lengthWidth, -0.5f, -lengthWidth, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        lengthWidth, -0.5f, lengthWidth, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        lengthWidth, -0.5f, lengthWidth, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -lengthWidth, -0.5f, lengthWidth, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -lengthWidth, -0.5f, -lengthWidth, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    };

    std::vector<glm::vec3> tangentBuffer;
    int stride = 8;
    for (size_t i = 0; i < vertices.size(); i += stride * 3)
    {
        glm::vec3 a(vertices[i], vertices[i + 1], vertices[i + 2]);
        glm::vec3 b(vertices[i + stride], vertices[i + stride + 1], vertices[i + stride + 2]);
        glm::vec3 c(vertices[i + stride * 2], vertices[i + stride * 2 + 1], vertices[i + stride * 2 + 2]);

        glm::vec3 edge1 = b - a;
        glm::vec3 edge2 = c - a;

        float deltaU1 = vertices[i + stride + 3] - vertices[i + 3];
        float deltaV1 = vertices[i + stride + 4] - vertices[i + 4];
        float deltaU2 = vertices[i + stride * 2 + 3] - vertices[i + 3];
        float deltaV2 = vertices[i + stride * 2 + 4] - vertices[i + 4];

        float f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);

        glm::vec3 tempTangent(
            f * (deltaV2 * edge1.x - deltaV1 * edge2.x),
            f * (deltaV2 * edge1.y - deltaV1 * edge2.y),
            f * (deltaV2 * edge1.z - deltaV1 * edge2.z));
        tangentBuffer.push_back(tempTangent);
        tangentBuffer.push_back(tempTangent);
        tangentBuffer.push_back(tempTangent);
    }
    GLuint VBO, tangetBufferObj;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &tangetBufferObj);
    glGenVertexArrays(1, &floorVAO);

    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, tangetBufferObj);
    glBufferData(GL_ARRAY_BUFFER, tangentBuffer.size() * sizeof(glm::vec3), &tangentBuffer.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
    glEnableVertexAttribArray(3);
    glBindVertexArray(0);

}
bool initGL()
{
    generateFloor();
    //SHADOW MAP
    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //END SHADOW MAP
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    std::vector<GLfloat> vertices = {
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,       0.0f, 0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,     1.0f, 0.0f,       0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f,      1.0f, 1.0f,       0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f,      1.0f, 1.0f,       0.0f, 0.0f, -1.0f,
        -0.5f, 0.5f, -0.5f,     0.0f, 1.0f,       0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,       0.0f, 0.0f, -1.0f,

        -0.5f, -0.5f, 0.5f,     0.0f, 0.0f,       0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f,      1.0f, 0.0f,       0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f,       1.0f, 1.0f,       0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f,       1.0f, 1.0f,       0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f,      0.0f, 1.0f,       0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f,     0.0f, 0.0f,       0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, 0.5f,      1.0f, 0.0f,      -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f,     1.0f, 1.0f,      -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,      -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,      -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f,     0.0f, 0.0f,      -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f,      1.0f, 0.0f,      -1.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.5f,       1.0f, 1.0f,      1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f,      0.0f, 1.0f,      1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,     0.0f, 0.0f,      1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,     0.0f, 0.0f,      1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f,      0.0f, 1.0f,      1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f,       1.0f, 1.0f,      1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,      0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, -0.5f,     1.0f, 1.0f,      0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f,      1.0f, 0.0f,      0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f,      1.0f, 0.0f,      0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f,     0.0f, 0.0f,      0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,      0.0f, -1.0f, 0.0f,

        -0.5f, 0.5f, -0.5f,     0.0f, 1.0f,      0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f,      1.0f, 1.0f,      0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f,       1.0f, 0.0f,      0.0f, 1.0f, 0.0f, 
        0.5f, 0.5f, 0.5f,       1.0f, 0.0f,      0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f,      0.0f, 0.0f,      0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f,     0.0f, 1.0f,      0.0f, 1.0f, 0.0f
    };

    std::vector<glm::vec3> tangentBuffer;
    int stride = 8;
    for (size_t i = 0; i < vertices.size(); i += stride * 3)
    {
        glm::vec3 a(vertices[i], vertices[i + 1], vertices[i + 2]);
        glm::vec3 b(vertices[i + stride], vertices[i + stride + 1], vertices[i + stride + 2]);
        glm::vec3 c(vertices[i + stride*2], vertices[i + stride*2 + 1], vertices[i + stride*2 + 2]);

        glm::vec3 edge1 = b - a;
        glm::vec3 edge2 = c - a;

        float deltaU1 = vertices[i + stride + 3] - vertices[i + 3];
        float deltaV1 = vertices[i + stride + 4] - vertices[i + 4];
        float deltaU2 = vertices[i + stride * 2 + 3] - vertices[i + 3];
        float deltaV2 = vertices[i + stride * 2 + 4] - vertices[i + 4];

        float f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);

        glm::vec3 tempTangent(
            f * (deltaV2 * edge1.x - deltaV1 * edge2.x),
            f * (deltaV2 * edge1.y - deltaV1 * edge2.y),
            f * (deltaV2 * edge1.z - deltaV1 * edge2.z));
        tangentBuffer.push_back(tempTangent);
        tangentBuffer.push_back(tempTangent);
        tangentBuffer.push_back(tempTangent);
    }

    GLuint indices[] = 
    { 
        0, 1, 2,
        1,2,3
    };
    GLuint VBO, EBO, tangetBufferObj;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &tangetBufferObj);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, tangetBufferObj);
    glBufferData(GL_ARRAY_BUFFER, tangentBuffer.size() * sizeof(glm::vec3), &tangentBuffer.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
    glEnableVertexAttribArray(3);


    int texWidth, texHeight;
    unsigned char* image = SOIL_load_image("pic.png", &texWidth, &texHeight, 0, SOIL_LOAD_RGB);
    
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);

    image = SOIL_load_image("picNormalMap.png", &texWidth, &texHeight, 0, SOIL_LOAD_RGB);

    glGenTextures(1, &normalMapID);
    glBindTexture(GL_TEXTURE_2D, normalMapID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);



    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    lightingShader.loadShader("plainVs.glsl", "lightingFs.glsl");
    lampShader.loadShader("plainVs.glsl", "plainFs.glsl");
    firstPassShader.loadShader("firstPassVs.glsl", "firstPassFs.glsl");
    depthOnQuadShader.loadShader("depthQuadVs.glsl", "depthQuadFs.glsl");


    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    return true;
}

using namespace glm;
vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

vec3 lightPos(1.2f, 1.0f, 2.0f);

GLuint lookAtPositionIndex = 0;

DirectionalLight dirLight(vec3(-0.2, -1.0f, -0.3f), vec3(0.2f, 0.2f, 0.2f), vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f));
glm::mat4 lightSpaceMatrix;
void renderFirstPass();
void renderDepthMap();
void renderSecondPass();
void render()
{
    renderFirstPass();
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderDepthMap();
    renderSecondPass();
}

void renderFirstPass()
{
    glm::mat4 lightProjection, lightView;
    lightProjection = glm::ortho(-10.f, 10.0f, -10.0f, 10.0f, 1.0f, 17.5f);
    lightProjection = glm::perspective(30.f, (float)(SCREEN_WIDTH / SCREEN_HEIGHT), 1.f, 30.f);
   
    lightView = glm::lookAt(lightPos, lightPos - glm::vec3(0,1,0), glm::vec3(0.0f, 0.0f,1.0f));
    lightSpaceMatrix = lightProjection * lightView;

    //aktivera shader, ladda upp lightSpaceMatrix

    firstPassShader.Use();
    glUniformMatrix4fv(glGetUniformLocation(firstPassShader.getID(), "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(VAO);
    glm::mat4 model;
    for (GLuint i = 0; i < cubePositions.size(); i++)
    {
        model = glm::mat4();
        model = glm::translate(model, cubePositions[i]);
        GLfloat angle = 20.f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        glUniformMatrix4fv(glGetUniformLocation(firstPassShader.getID(), "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(lightVAO);
    model = glm::mat4();
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2));
    glUniformMatrix4fv(glGetUniformLocation(firstPassShader.getID(), "model"), 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(floorVAO);
    model = glm::mat4();
    model = glm::translate(model, vec3(0, -0.8f, 4));
    glUniformMatrix4fv(glGetUniformLocation(firstPassShader.getID(), "model"), 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(0);
}

void renderDepthMap()
{
    GLuint quadVAO, quadVBO;

    GLfloat vertices[] =
    {
        -1.0f, 1.0f, 0.0f,      0.0f, 1.0f,
        -1.0f, 0.3f, 0.0f,      0.0f, 0.0f,
        -0.3f, 1.0f, 0.0f,      1.0f, 1.0f,
        -0.3f, 0.3f, 0.0f,      1.0f, 0.0f
    };
    depthOnQuadShader.Use();
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glUniform1i(glGetUniformLocation(depthOnQuadShader.getID(), "tex"), 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
float shadowBias = 0.005;
void renderSecondPass()
{
    lightingShader.Use();
    GLuint modelLoc = glGetUniformLocation(lightingShader.getID(), "model");
    GLuint viewLoc = glGetUniformLocation(lightingShader.getID(), "view");
    GLuint projectionLoc = glGetUniformLocation(lightingShader.getID(), "projection");
    GLuint objectColorLoc = glGetUniformLocation(lightingShader.getID(), "objectColor");
    GLuint lightColorLoc = glGetUniformLocation(lightingShader.getID(), "lightColor");
    GLuint lightPosLoc = glGetUniformLocation(lightingShader.getID(), "lightPos");
    GLuint viewPosLoc = glGetUniformLocation(lightingShader.getID(), "viewPos");
    GLuint textureSamplerID = glGetUniformLocation(lightingShader.getID(), "texSampler");
    GLuint normalMapSamplerID = glGetUniformLocation(lightingShader.getID(), "normalSampler");

    glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
    glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
    glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);

    //DirLightSetup
    glUniform3f(glGetUniformLocation(lightingShader.getID(), "dirLight.direction"), dirLight.direction.x, dirLight.direction.y, dirLight.direction.z);
    glUniform3f(glGetUniformLocation(lightingShader.getID(), "dirLight.ambient"), dirLight.ambient.x, dirLight.ambient.y, dirLight.ambient.z);
    glUniform3f(glGetUniformLocation(lightingShader.getID(), "dirLight.diffuse"), dirLight.diffuse.x, dirLight.diffuse.y, dirLight.diffuse.z);
    glUniform3f(glGetUniformLocation(lightingShader.getID(), "dirLight.specular"), dirLight.specular.x, dirLight.specular.y, dirLight.specular.z);
    //End DirLightSetup

    glUniformMatrix4fv(glGetUniformLocation(lightingShader.getID(), "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    glUniform1f(glGetUniformLocation(lightingShader.getID(), "shadowBias"), shadowBias);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(textureSamplerID, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalMapID);
    glUniform1i(normalMapSamplerID, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glUniform1i(glGetUniformLocation(lightingShader.getID(), "shadowSampler"), 2);
    glBindVertexArray(VAO);
    glm::mat4 view;
    view = glm::lookAt(cameraPos, cubePositions[lookAtPositionIndex], cameraUp);


    glm::mat4 projection;
    projection = glm::perspective(45.f, GLfloat(SCREEN_WIDTH / SCREEN_HEIGHT), 0.1f, 100.f);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glm::mat4 model;
    for (GLuint i = 0; i < cubePositions.size(); i++)
    {
        model = glm::mat4();
        model = glm::translate(model, cubePositions[i]);
        GLfloat angle = 20.f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindVertexArray(floorVAO);
    model = glm::mat4();
    model = glm::translate(model, vec3(0, -0.8f, 0));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    //Lamp visualisation
    lampShader.Use();
    modelLoc = glGetUniformLocation(lampShader.getID(), "model");
    viewLoc = glGetUniformLocation(lampShader.getID(), "view");
    projectionLoc = glGetUniformLocation(lampShader.getID(), "projection");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glBindVertexArray(lightVAO);
    model = glm::mat4();
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
}

int main(int argc, char* args[])
{
    //Start up SDL and create window
    if (!init())
    {
        printf("Failed to initialize!\n");
    }
    else
    {
        //Main loop flag
        bool quit = false;

        //Event handler
        SDL_Event e;

        //Enable text input
        SDL_StartTextInput();

        //While application is running
        while (!quit)
        {
            //Handle events on queue
            while (SDL_PollEvent(&e) != 0)
            {
                //User requests quit
                if (e.type == SDL_QUIT)
                {
                    quit = true;
                }
                //Handle keypress with current mouse position
                else if (e.type == SDL_TEXTINPUT)
                {
                    int x = 0, y = 0;
                    SDL_GetMouseState(&x, &y);
                }
            }
            update();
            render();
            SDL_GL_SwapWindow(gWindow);
            SDL_SetWindowTitle(gWindow, std::to_string(shadowBias).c_str());
        }

        //Disable text input
        SDL_StopTextInput();
    }

    //Free resources and close SDL
    close();

    return 0;
}
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
bool tabPressed = false;
void update()
{
    GLfloat currentFrame = SDL_GetTicks();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    cameraFront = normalize(cubePositions[lookAtPositionIndex] - cameraPos);

    GLfloat cameraSpeed = 0.009 * deltaTime;
    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    if (keyState[SDL_SCANCODE_W])
        cameraPos += cameraSpeed * cameraFront;
    if (keyState[SDL_SCANCODE_S])
        cameraPos -= cameraSpeed * cameraFront;
    if (keyState[SDL_SCANCODE_A])
        cameraPos -= normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
    if (keyState[SDL_SCANCODE_D])
        cameraPos += normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
    if (keyState[SDL_SCANCODE_SPACE])
        cameraPos += cameraSpeed * cameraUp;
    if (keyState[SDL_SCANCODE_LCTRL])
        cameraPos -= cameraSpeed * cameraUp;


    float lightSpeed = 0.02f;

    if (keyState[SDL_SCANCODE_KP_8])
        lightPos += vec3(0, 0, -lightSpeed);
    if (keyState[SDL_SCANCODE_KP_5])
        lightPos += vec3(0, 0, lightSpeed);
    if (keyState[SDL_SCANCODE_KP_4])
        lightPos += vec3(-lightSpeed, 0, 0);
    if (keyState[SDL_SCANCODE_KP_6])
        lightPos += vec3(lightSpeed, 0, 0);
    if (keyState[SDL_SCANCODE_KP_1])
        lightPos += vec3(0, -lightSpeed, 0);
    if (keyState[SDL_SCANCODE_KP_7])
        lightPos += vec3(0, lightSpeed, 0);

    if (keyState[SDL_SCANCODE_TAB])
    {
        if (!tabPressed)
        {
            lookAtPositionIndex = (lookAtPositionIndex + 1) % cubePositions.size();
            tabPressed = true;
        }
    }
    else
        tabPressed = false;

    if (keyState[SDL_SCANCODE_KP_PLUS])
        shadowBias += 0.001;
    if (keyState[SDL_SCANCODE_KP_MINUS])
        shadowBias -= 0.001;

}
void close()
{
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}