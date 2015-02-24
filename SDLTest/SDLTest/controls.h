#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void UpdateViewMatrix(SDL_Window* win, const Uint8* keyState);
glm::mat4 getViewMatrix();
