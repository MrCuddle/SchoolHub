/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL, SDL OpenGL, GLEW, standard IO, and strings
#include <SDL.h>
#include <gl\glew.h>
#include <SDL_opengl.h>
#include <gl\glu.h>
#include <stdio.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL_image.h>
#include <vector>

#include "Mesh.h"
#include "Pyramid.h"
#include "HeightMap.h"
#include "Shader.h"
#include "ShaderProgram.h"

//Screen dimension constants
const int SCREEN_WIDTH = 1366;
const int SCREEN_HEIGHT = 768;

struct DirectionalLight
{
    glm::vec3 color;
    float ambientIntensity;
};

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

using namespace glm;
bool init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
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
ShaderProgram shaderPlain;

void handleKeys(unsigned char key, int x, int y)
{
}

void update()
{
    const Uint8* state = SDL_GetKeyboardState(nullptr);

    if (state[SDL_SCANCODE_C])
        shaderPlain.useProgram();

    //No per frame update needed
}

void printProgramLog(GLuint program)
{
    //Make sure name is shader
    if (glIsProgram(program))
    {
        //Program log length
        int infoLogLength = 0;
        int maxLength = infoLogLength;

        //Get info string length
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        //Allocate string
        char* infoLog = new char[maxLength];

        //Get info log
        glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
        if (infoLogLength > 0)
        {
            //Print Log
            printf("%s\n", infoLog);
        }

        //Deallocate string
        delete[] infoLog;
    }
    else
    {
        printf("Name %d is not a program\n", program);
    }
}

void printShaderLog(GLuint shader)
{
    //Make sure name is shader
    if (glIsShader(shader))
    {
        //Shader log length
        int infoLogLength = 0;
        int maxLength = infoLogLength;

        //Get info string length
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        //Allocate string
        char* infoLog = new char[maxLength];

        //Get info log
        glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
        if (infoLogLength > 0)
        {
            //Print Log
            printf("%s\n", infoLog);
        }

        //Deallocate string
        delete[] infoLog;
    }
    else
    {
        printf("Name %d is not a shader\n", shader);
    }
}

Pyramid pyramid;
HeightMap hMap;
Mesh mesh;

glm::mat4 projection;

Shader fragmentShader, vertexShader, fragPlainShader;
ShaderProgram shaderMain;

bool initGL()
{
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    
    pyramid.init();
    hMap.init();
    mesh.init("gube.obj");

    vertexShader.loadShader("vertex_shader.glsl", GL_VERTEX_SHADER);
    fragmentShader.loadShader("meshFs.glsl", GL_FRAGMENT_SHADER);
    fragPlainShader.loadShader("plainfrag.glsl", GL_FRAGMENT_SHADER);
    shaderMain.createProgram();
    shaderMain.addShader(&vertexShader);
    shaderMain.addShader(&fragmentShader);
    shaderMain.linkProgram();
    shaderMain.useProgram();
    shaderPlain.createProgram();
    shaderPlain.addShader(&vertexShader);
    shaderPlain.addShader(&fragPlainShader);
    shaderPlain.linkProgram();

    projection = glm::perspective<float>(100, SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.f);

    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0f);
    return true;
}

float PIover180 = 3.1415f / 180.f;
float rotation = 0.f;
void render()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    GLuint modelViewLocation = glGetUniformLocation(shaderMain.getProgramID(), "modelViewMatrix");
    GLuint projectionLocation = glGetUniformLocation(shaderMain.getProgramID(), "projectionMatrix");

    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
    glm::mat4 modelViewMat = glm::lookAt(glm::vec3(0, 0, -1), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));


    glm::mat4 current = glm::rotate(modelViewMat, rotation, vec3(1.f, 1.f, 0.f));
    glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, glm::value_ptr(current));

    //hMap.draw();
    mesh.draw();
    //pyramid.draw();

    rotation += 0.01f;
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
                    handleKeys(e.text.text[0], x, y);
                }
            }
            const Uint8* keyState = SDL_GetKeyboardState(NULL);
            if (keyState[SDL_SCANCODE_ESCAPE])
                quit = true;
            update();
            //Update screen
            render();
            SDL_GL_SwapWindow(gWindow);
        }

        //Disable text input
        SDL_StopTextInput();
    }

    //Free resources and close SDL
    close();

    return 0;
}

void close()
{

    //Destroy window	
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    //Quit SDL subsystem
    SDL_Quit();
}