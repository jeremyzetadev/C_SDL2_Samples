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

// float fTheta; //dynamic for rotation

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

void init_global_properties(){
    for(int i=0; i<=4; i++){
        for(int j=0; j<=4; j++){
            global.matRotX.m[i][j] = 0.0f;
        }
    }
    for(int i=0; i<=4; i++){
        for(int j=0; j<=4; j++){
            global.matRotZ.m[i][j] = 0.0f;
        }
    }
    global.g_camera = (vec3){0, 0, 0};
}

void mesh_render(float fElapsedTime){
    float fTheta =0;
    fTheta +=(0.01f * fElapsedTime);

		global.matRotZ.m[0][0] = cosf(fTheta);
		global.matRotZ.m[0][1] = sinf(fTheta);
		global.matRotZ.m[1][0] = -sinf(fTheta);
		global.matRotZ.m[1][1] = cosf(fTheta);
		global.matRotZ.m[2][2] = 1;
		global.matRotZ.m[3][3] = 1;

    global.matRotX.m[0][0] = 1;
		global.matRotX.m[1][1] = cosf(fTheta * 0.5f);
		global.matRotX.m[1][2] = sinf(fTheta * 0.5f);
		global.matRotX.m[2][1] = -sinf(fTheta * 0.5f);
		global.matRotX.m[2][2] = cosf(fTheta * 0.5f);
		global.matRotX.m[3][3] = 1;

    //Mesh triangles
    vec3 normal, line1, line2;
    size_t box_triangle_count = 12;
    Mesh *mesh_box = mesh_create(box_triangle_count);
    mesh_init_tris_SampleBox(mesh_box);
    Matrix *mproj = mat_create_projectionmatrix_sample();
    for(size_t i=0; i<mesh_box->tris_num; i++){
        Triangle t;
        vec3 tri_vec1 = (*(mesh_box->tris[i])).p[0];
        vec3 tri_vec2 = (*(mesh_box->tris[i])).p[1];
        vec3 tri_vec3 = (*(mesh_box->tris[i])).p[2];
        t.p[0] = tri_vec1;
        t.p[1] = tri_vec2;
        t.p[2] = tri_vec3;

            //Rotate in Z-Axis
            t.p[0] = MultiplyMatrixVector(t.p[0], global.matRotZ);
            t.p[1] = MultiplyMatrixVector(t.p[1], global.matRotZ);
            t.p[2] = MultiplyMatrixVector(t.p[2], global.matRotZ);
            
            //Rotate in X-Axis
            t.p[0] = MultiplyMatrixVector(t.p[0], global.matRotX);
            t.p[1] = MultiplyMatrixVector(t.p[1], global.matRotX);
            t.p[2] = MultiplyMatrixVector(t.p[2], global.matRotX);
            
            //Offset into the screen
            t.p[0].z = t.p[0].z + 3.0f;
            t.p[1].z = t.p[1].z + 3.0f;
            t.p[2].z = t.p[2].z + 3.0f;

            //triangle normal compute for show if facing the camera
            //triangle normal compute for show if facing the camera
            line1.x = t.p[1].x - t.p[0].x;
            line1.y = t.p[1].y - t.p[0].y;
            line1.z = t.p[1].z - t.p[0].z;

            line2.x = t.p[2].x - t.p[0].x;
            line2.y = t.p[2].y - t.p[0].y;
            line2.z = t.p[2].z - t.p[0].z;

            normal.x = line1.y*line2.z -line1.z*line2.y;
            normal.y = line1.z*line2.x -line1.x*line2.z;
            normal.z = line1.x*line2.y -line1.y*line2.x;

            float l = sqrtf(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
            normal.x = normal.x/l; normal.y = normal.y/l; normal.z = normal.z/l;

            //dot product to camera the triangle/normals
            float alignment =   normal.x * (t.p[0].x - global.g_camera.x) +
                                normal.y * (t.p[1].y - global.g_camera.y) +
                                normal.z * (t.p[2].z - global.g_camera.z);


            //triangle normal compute for show if facing the camera
            //triangle normal compute for show if facing the camera


            //triangle not facing camera no compute
            // if(normal.z<0)
            if(alignment<0.0f)
            {
                //TODO implement Illumination by distance from camera (near lighter, far darker) 
                //TODO maybe add at function Render_TriangleFill by using its z value of vector

                //project triangles from 3D --> 2D
                t.p[0] = MultiplyMatrixVector(t.p[0], *mproj);
                t.p[1] = MultiplyMatrixVector(t.p[1], *mproj);
                t.p[2] = MultiplyMatrixVector(t.p[2], *mproj);

                //Scale into view
                t.p[0].x += 1.0f;  t.p[0].y += 1.0f;
                t.p[1].x += 1.0f;  t.p[1].y += 1.0f;
                t.p[2].x += 1.0f;  t.p[2].y += 1.0f;
                t.p[0].x *= 0.5f*(float)SCREEN_WIDTH;
                t.p[1].x *= 0.5f*(float)SCREEN_WIDTH;
                t.p[2].x *= 0.5f*(float)SCREEN_WIDTH;
                t.p[0].y *= 0.5f*(float)SCREEN_HEIGHT;
                t.p[1].y *= 0.5f*(float)SCREEN_HEIGHT;
                t.p[2].y *= 0.5f*(float)SCREEN_HEIGHT;
            }

        //triangle not facing camera no compute
        // if(normal.z<0)
        if(alignment<0.0f)
        {
            Render_TriangleFill(t);
            Render_TriangleLines(t);
        }
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
    // vec3 v1 = {.x=100, .y=100};
    // vec3 v2 = {.x=400, .y=400};
    // Render_LineScreenSurface(v1, v2);
    // SDL_UpdateWindowSurface(global.g_window);
    // SDL_Delay(700);

    bool isGameRunning = true;
    clock_t start, diff;
    float fElapsedTime =0;
    start = clock();
    init_global_properties();
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
        //mesh render sample
        mesh_render(fElapsedTime);
        diff = clock() - start;
        fElapsedTime = diff * 1000.0f / CLOCKS_PER_SEC;
        Render_FillScreenBlue();
        UnlockScreenSurface();
    }

    // UnlockScreenSurface();
    SDL_Delay(3000);
    SDL_DestroyWindow(global.g_window);
    SDL_Quit();

    return 0;
}

