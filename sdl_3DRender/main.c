#include <SDL2/SDL_error.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include "geometry_util.h"
#include "math_util.h"
#include "render.h"
#include "global.h"

#define ERROR_EXIT(...) {fprintf(stderr, __VA_ARGS__); exit(1);}
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

void InitializeProgram(){
    if(SDL_Init(SDL_INIT_VIDEO) <0)
        ERROR_EXIT("SDL could not initialized, SDL_Error: %s\n" ,SDL_GetError());

    global.g_window = SDL_CreateWindow("Draw Line in Lock Surface", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    if(!global.g_window)
        ERROR_EXIT("g_window could not be created, SDL_Error: %s\n", SDL_GetError());

    global.g_screenSurface = SDL_GetWindowSurface(global.g_window);
    if(!global.g_screenSurface)
        ERROR_EXIT("Could not get g_window surface, SDL_Error:%s\n", SDL_GetError());

}

void LockScreenSurface(){
    if(SDL_MUSTLOCK(global.g_screenSurface)){
        if(SDL_LockSurface(global.g_screenSurface)<0)
            ERROR_EXIT("Could not lock surface, SDL Error:%s\n", SDL_GetError());
    }
}

void UnlockScreenSurface(){
    if(SDL_MUSTLOCK(global.g_screenSurface)){
        SDL_UnlockSurface(global.g_screenSurface);
    }
}

void mesh_render(){
    //Mesh triangles
    size_t box_triangle_count = 12;
    Mesh *mesh_box = mesh_create(box_triangle_count);
    mesh_init_tris_SampleBox(mesh_box);
    Matrix *mproj = mat_create_projectionmatrix_sample();
    for(size_t i=0; i<mesh_box->tris_num; i++){
        Triangle t;
        for(int j=0; j<3; j++){
            vec3 tri_vec = (*(mesh_box->tris[i])).p[j];
            t.p[j] = MultiplyMatrixVector(tri_vec, *mproj);
        }
        //Scale into view
        t.p[0].x += 1.0f;  t.p[0].y += 1.0f;
        t.p[1].x += 1.0f;  t.p[1].y += 1.0f;
        t.p[2].x += 1.0f;  t.p[2].y += 1.0f;

        t.p[0].x *= 0.5f*(float)SCREEN_WIDTH;
        t.p[0].y *= 0.5f*(float)SCREEN_HEIGHT;
        t.p[1].x *= 0.5f*(float)SCREEN_WIDTH;
        t.p[1].y *= 0.5f*(float)SCREEN_HEIGHT;
        t.p[2].x *= 0.5f*(float)SCREEN_WIDTH;
        t.p[2].y *= 0.5f*(float)SCREEN_HEIGHT;
        //Scale into view
        Render_Triangle(t);
    }
    SDL_UpdateWindowSurface(global.g_window);
    SDL_Delay(700);
    //Mesh triangle
}

int main(){
    InitializeProgram();

    LockScreenSurface();

    // Render_EditScreenSurfacePixels(red);
    // SDL_UpdateWindowSurface(g_window);
    // SDL_Delay(700);
    //
    // Render_EditScreenSurfacePixels(blue);
    // SDL_UpdateWindowSurface(g_window);
    // SDL_Delay(700);
    //
    // Render_EditScreenSurfacePixels(green);
    // SDL_UpdateWindowSurface(g_window);
    // SDL_Delay(700);
    
    Render_FillScreenBlue();
    SDL_UpdateWindowSurface(global.g_window);
    SDL_Delay(700);
    vec3 v1 = {.x=100, .y=100};
    vec3 v2 = {.x=400, .y=400};
    Render_LineScreenSurface(v1, v2);
    SDL_UpdateWindowSurface(global.g_window);
    SDL_Delay(700);

    //mesh render sample
    mesh_render();

    UnlockScreenSurface();
    SDL_Delay(3000);
    SDL_DestroyWindow(global.g_window);
    SDL_Quit();

    return 0;
}

