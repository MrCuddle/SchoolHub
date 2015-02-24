#include "controls.h"
#include <iostream>

glm::mat4 viewMatrix;

glm::mat4 getViewMatrix()
{
    return viewMatrix;
}

glm::vec3 position = glm::vec3(0, 0, 5);
float horizontalAngle = 3.14f;
float verticalAngle = 0.0f;
float FoV = 45.f;
float speed = 0.09f;
float mouseSpeed = 0.0009f;

void UpdateViewMatrix(SDL_Window* win, const Uint8* keyState)
{
    int mousePosX, mousePosY;
    if (SDL_GetWindowFlags(win) & SDL_WINDOW_INPUT_FOCUS)
    {
        SDL_GetMouseState(&mousePosX, &mousePosY);
        SDL_WarpMouseInWindow(win, 640 / 2, 480 / 2);
        horizontalAngle += mouseSpeed * float(640 / 2 - mousePosX);
        verticalAngle += mouseSpeed * float(480 / 2 - mousePosY);
    }

    glm::vec3 direction(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
        );

    glm::vec3 right = glm::vec3(
        sin(horizontalAngle - 3.14f / 2.0f),
        0,
        cos(horizontalAngle - 3.14f / 2.0f)
        );

    glm::vec3 up = glm::cross(right, direction);

    if (keyState[SDL_SCANCODE_W])
        position += direction * speed;
    if (keyState[SDL_SCANCODE_S])
        position -= direction * speed;
    if (keyState[SDL_SCANCODE_D])
        position += right * speed;
    if (keyState[SDL_SCANCODE_A])
        position -= right * speed;
    if (keyState[SDL_SCANCODE_SPACE])
        position += up*speed;
    if (keyState[SDL_SCANCODE_LCTRL])
        position -= up*speed;
    //std::cout << "X:" << position.x << " Y:" << position.y << " Z:" << position.z << std::endl;
    viewMatrix = glm::lookAt(position, position + direction, up);
}