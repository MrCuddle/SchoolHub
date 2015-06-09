/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL, SDL OpenGL, GLEW, standard IO, and strings
#include <SDL.h>
#include <gl\glew.h>
#include <SDL_opengl.h>
#include <gl\glu.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "ShaderProgram.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SOIL/SOIL.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Starts up SDL, creates window, and initializes OpenGL
bool init();

//Initializes rendering program and clear color
bool initGL();

//Input handler
void handleKeys( unsigned char key, int x, int y );

//Per frame update
void update();

//Renders quad to the screen
void render();

//Frees media and shuts down SDL
void close();

//Shader loading utility programs
void printProgramLog( GLuint program );
void printShaderLog( GLuint shader );

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//OpenGL context
SDL_GLContext gContext;
bool init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Use OpenGL 3.1 core
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

        //Create window
        gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            //Create context
            gContext = SDL_GL_CreateContext( gWindow );
            if( gContext == NULL )
            {
                printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
                success = false;
            }
            else
            {
                //Initialize GLEW
                glewExperimental = GL_TRUE;
                GLenum glewError = glewInit();
                if( glewError != GLEW_OK )
                {
                    printf( "Error initializing GLEW! %s\n", glewGetErrorString( glewError ) );
                }

                //Use Vsync
                if( SDL_GL_SetSwapInterval( 1 ) < 0 )
                {
                    printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
                }

                //Initialize OpenGL
                if( !initGL() )
                {
                    printf( "Unable to initialize OpenGL!\n" );
                    success = false;
                }
            }
        }
    }

    return success;
}

GLuint boxVAO, textureID;
ShaderProgram program;

bool initGL()
{
    glClearColor( 0.2f, 0.3f, 0.3f, 1.0f );
    program.loadShader( "plainVs.glsl", "plainFs.glsl" );

    int texWidth, texHeight;
    unsigned char* image = SOIL_load_image( "pic.png", &texWidth, &texHeight, 0, SOIL_LOAD_RGB );

    glGenTextures( 1, &textureID );
    glBindTexture( GL_TEXTURE_2D, textureID );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glGenerateMipmap( GL_TEXTURE_2D );
    SOIL_free_image_data( image );

    std::vector<GLfloat> vertices = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays( 1, &boxVAO );
    GLuint VBO;
    glGenBuffers( 1, &VBO );
    glBindVertexArray( boxVAO );
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( GLfloat ), &vertices[0], GL_STATIC_DRAW );

    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof( GLfloat ), (GLvoid*)0 );
    glEnableVertexAttribArray( 0 );

    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof( GLfloat ), (GLvoid*)( 3 * sizeof( GLfloat ) ) );
    glEnableVertexAttribArray( 1 );

    glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof( GLfloat ), (GLvoid*)( 5 * sizeof( GLfloat ) ) );
    glEnableVertexAttribArray( 2 );

    glBindVertexArray( 0 );
    //Success flag
    bool success = true;
    glEnable(GL_DEPTH_TEST);
    return success;
}

void handleKeys( unsigned char key, int x, int y )
{
}

glm::vec3 cameraPos = glm::vec3( 0.0f, 0.0f, 5.0f );
glm::vec3 cameraFront = glm::vec3( 0.0f, 0.0f, -1.0f );
glm::vec3 cameraUp = glm::vec3( 0.0f, 1.0f, 0.0f );


void update()
{
    float camSpeed = 0.02f;
    const Uint8* state = SDL_GetKeyboardState( NULL );
    if( state[SDL_SCANCODE_W] )
        cameraPos += glm::vec3( 0, 0, -camSpeed );
    if( state[SDL_SCANCODE_S] )
        cameraPos += glm::vec3( 0, 0, camSpeed );
    if( state[SDL_SCANCODE_A] )
        cameraPos += glm::vec3( -camSpeed, 0, 0 );
    if( state[SDL_SCANCODE_D] )
        cameraPos += glm::vec3( camSpeed, 0, 0 );
    if( state[SDL_SCANCODE_SPACE] )
        cameraPos += glm::vec3( 0, camSpeed, 0 );
    if( state[SDL_SCANCODE_LCTRL] )
        cameraPos += glm::vec3( 0, -camSpeed, 0 );
    cameraFront = glm::normalize(-cameraPos);
}


void render()
{
    glCullFace(GL_BACK);
    glm::mat4 view = glm::lookAt( cameraPos, glm::vec3(0,0,0), cameraUp );
    glm::mat4 model = glm::mat4();
    glm::mat4 projection;
    projection = glm::perspective( 90.f, GLfloat( SCREEN_WIDTH / SCREEN_HEIGHT ), 0.1f, 10.f );
    //Clear color buffer
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    program.Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture( GL_TEXTURE_2D, textureID );
    //
    glUniform3f( glGetUniformLocation( program.getID(), "light.ambient" ), 0.1f, 0.1f, 0.1f );
    // We set the diffuse intensity a bit higher; note that the right lighting conditions differ with each lighting method and environment.
    // Each environment and lighting type requires some tweaking of these variables to get the best out of your environment.
    glUniform3f(glGetUniformLocation(program.getID(), "light.position"), cameraPos.x, cameraPos.y, cameraPos.z);
    glUniform3f(glGetUniformLocation(program.getID(), "light.direction"), cameraFront.x, cameraFront.y, cameraFront.z);
    glUniform1f(glGetUniformLocation(program.getID(), "light.cutOff"), glm::cos(glm::radians(12.5f)));
    glUniform3f(glGetUniformLocation(program.getID(), "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);

    glUniform3f(glGetUniformLocation(program.getID(), "light.ambient"),   0.1f, 0.1f, 0.1f);
    glUniform3f( glGetUniformLocation( program.getID(), "light.diffuse" ), 0.8f, 0.8f, 0.8f );
    glUniform3f( glGetUniformLocation( program.getID(), "light.specular" ), 1.0f, 1.0f, 1.0f );
    glUniform1f( glGetUniformLocation( program.getID(), "light.constant" ), 1.0f );
    glUniform1f( glGetUniformLocation( program.getID(), "light.linear" ), 0.09 );
    glUniform1f( glGetUniformLocation( program.getID(), "light.quadratic" ), 0.032 );
    //
    glUniform1i( glGetUniformLocation( program.getID(), "sampler" ), 0 );
    glUniformMatrix4fv( glGetUniformLocation( program.getID(), "model" ), 1, GL_FALSE, glm::value_ptr( model ) );
    glUniformMatrix4fv( glGetUniformLocation( program.getID(), "view" ), 1, GL_FALSE, glm::value_ptr( view ) );
    glUniformMatrix4fv( glGetUniformLocation( program.getID(), "projection" ), 1, GL_FALSE, glm::value_ptr( projection ) );
    glBindVertexArray( boxVAO );
    glDrawArrays( GL_TRIANGLES, 0, 36 );
}

void close()
{

    //Destroy window	
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}

void printProgramLog( GLuint program )
{
    //Make sure name is shader
    if( glIsProgram( program ) )
    {
        //Program log length
        int infoLogLength = 0;
        int maxLength = infoLogLength;

        //Get info string length
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &maxLength );

        //Allocate string
        char* infoLog = new char[maxLength];

        //Get info log
        glGetProgramInfoLog( program, maxLength, &infoLogLength, infoLog );
        if( infoLogLength > 0 )
        {
            //Print Log
            printf( "%s\n", infoLog );
        }

        //Deallocate string
        delete[] infoLog;
    }
    else
    {
        printf( "Name %d is not a program\n", program );
    }
}

void printShaderLog( GLuint shader )
{
    //Make sure name is shader
    if( glIsShader( shader ) )
    {
        //Shader log length
        int infoLogLength = 0;
        int maxLength = infoLogLength;

        //Get info string length
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );

        //Allocate string
        char* infoLog = new char[maxLength];

        //Get info log
        glGetShaderInfoLog( shader, maxLength, &infoLogLength, infoLog );
        if( infoLogLength > 0 )
        {
            //Print Log
            printf( "%s\n", infoLog );
        }

        //Deallocate string
        delete[] infoLog;
    }
    else
    {
        printf( "Name %d is not a shader\n", shader );
    }
}

int main( int argc, char* args[] )
{
    //Start up SDL and create window
    if( !init() )
    {
        printf( "Failed to initialize!\n" );
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
        while( !quit )
        {
            //Handle events on queue
            while( SDL_PollEvent( &e ) != 0 )
            {
                //User requests quit
                if( e.type == SDL_QUIT )
                {
                    quit = true;
                }
                //Handle keypress with current mouse position
                else if( e.type == SDL_TEXTINPUT )
                {
                    int x = 0, y = 0;
                    SDL_GetMouseState( &x, &y );
                    handleKeys( e.text.text[0], x, y );
                }
            }
            update();
            render();

            //Update screen
            SDL_GL_SwapWindow( gWindow );
        }

        //Disable text input
        SDL_StopTextInput();
    }

    //Free resources and close SDL
    close();

    return 0;
}