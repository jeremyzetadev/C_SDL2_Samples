#include <SDL2/SDL_keycode.h>
#include <time.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "geometry_util.h"
#include "math_util.h"
#include "render.h"
#include "global.h"

#define ERROR_EXIT(...) {fprintf(stderr, __VA_ARGS__); exit(1);}
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

float fTheta; //dynamic for rotation

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

void mesh_render(float fElapsedTime){
    // Set up rotation matrices
		Matrix matRotZ, matRotX;

		// Rotation Z
		matRotZ.m[0][0] = cosf(fTheta);
		matRotZ.m[0][1] = sinf(fTheta);
		matRotZ.m[1][0] = -sinf(fTheta);
		matRotZ.m[1][1] = cosf(fTheta);
		matRotZ.m[2][2] = 1;
		matRotZ.m[3][3] = 1;

		// Rotation X
		matRotX.m[0][0] = 1;
		matRotX.m[1][1] = cosf(fTheta * 0.5f);
		matRotX.m[1][2] = sinf(fTheta * 0.5f);
		matRotX.m[2][1] = -sinf(fTheta * 0.5f);
		matRotX.m[2][2] = cosf(fTheta * 0.5f);
		matRotX.m[3][3] = 1;

    //Mesh triangles
    size_t box_triangle_count = 12;
    Mesh *mesh_box = mesh_create(box_triangle_count);
    mesh_init_tris_SampleBox(mesh_box);
    Matrix *mproj = mat_create_projectionmatrix_sample();
    for(size_t i=0; i<mesh_box->tris_num; i++){
        Triangle t;
        for(int j=0; j<3; j++){
            vec3 tri_vec = (*(mesh_box->tris[i])).p[j];

            // //Rotate in Z-Axis
            // t.p[j] = MultiplyMatrixVector(tri_vec, matRotZ);
            //
            // //Rotate in Z-Axis
            // t.p[j] = MultiplyMatrixVector(t.p[j], matRotX);
            //
            // //Offset into the screen
            // t.p[j].z += t.p[j].z + 3.0f;
            
            //project triangles from 3D --> 2D
            // t.p[j] = MultiplyMatrixVector(t.p[j], *mproj);

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
    SDL_Delay(16);
    mesh_free(mesh_box);
    free(mproj);
}

int main(){
    InitializeProgram();

    // LockScreenSurface();

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

    bool isGameRunning = true;
    clock_t start, diff;
    while(isGameRunning){
        LockScreenSurface();
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    isGameRunning = false;
                    break;
                                        
                default:
                    break;
            }
        }
        start = clock();
        float fElapsedTime = diff * 1000.0f / CLOCKS_PER_SEC;
        fTheta += 1.0f * fElapsedTime;
        //mesh render sample
        mesh_render(fTheta);
        diff = clock() - start;
        UnlockScreenSurface();
    }

    // UnlockScreenSurface();
    SDL_Delay(3000);
    SDL_DestroyWindow(global.g_window);
    SDL_Quit();

    return 0;
}

