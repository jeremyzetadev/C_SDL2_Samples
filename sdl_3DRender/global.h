#pragma once

#include <SDL2/SDL.h>
#include "math_util.h"

typedef struct{
    SDL_Window *g_window;
    SDL_Surface *g_screenSurface;
    SDL_Renderer *g_renderer;
    Matrix matRotX;
    Matrix matRotZ;
    vec3 g_camera;
    vec3 vLookDir;
    char *objFile_extrudebox;
    char *objFile_mountains;
    int max_distance_sight;

    float fYaw;
} Global;

extern Global global;
// extern SDL_Window *g_window;
// extern SDL_Surface *g_screenSurface;

