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
    // global.objFile = "axis.obj";
    global.objFile = "blender_legacyobj.obj";
}

void mesh_render(Mesh *mesh_box, float fElapsedTime){
    float fTheta =0;
    fTheta +=(0.002f * fElapsedTime);

    // size_t box_triangle_count = 12;
    // Mesh *mesh_box = mesh_create(box_triangle_count);
    // mesh_init_tris_SampleBox(mesh_box);
    // Mesh *mesh_box = mesh_create_loadfromObj();
    // mesh_loadfrom_Obj(mesh_box);

    //Mesh triangles
    vec3 normal, line1, line2;
    Triangle ArrTri[mesh_box->tris_num];
    bool ArrTriBool[mesh_box->tris_num];
    memset(ArrTriBool, false, sizeof(ArrTriBool)); 

    // Matrix *mproj = mat_create_projectionmatrix_sample();
    Matrix mproj = Matrix_MakeProjection(90.0f, (float)SCREEN_HEIGHT/(float)SCREEN_WIDTH, 0.1f, 1000.0f);
    //Rotate in Z-Axis
    global.matRotZ = Matrix_MakeRotationZ(fTheta * 0.5f);
    //Rotate in X-Axis
    global.matRotX = Matrix_MakeRotationX(fTheta); 

    //Offset into the screen
    Matrix matTrans;
    matTrans = Matrix_MakeTranslation(0, 0, 4.0f);

    Matrix matWorld;
    matWorld = Matrix_MakeIdentity();
    matWorld = Matrix_MultiplyMatrix(&global.matRotZ, &global.matRotX);
    matWorld = Matrix_MultiplyMatrix(&matWorld, &matTrans);
    for(size_t i=0; i<mesh_box->tris_num; i++){
        Triangle t;
        vec3 tri_vec1 = (*(mesh_box->tris[i])).p[0];
        vec3 tri_vec2 = (*(mesh_box->tris[i])).p[1];
        vec3 tri_vec3 = (*(mesh_box->tris[i])).p[2];
        t.p[0] = tri_vec1;
        t.p[1] = tri_vec2;
        t.p[2] = tri_vec3;

        t.p[0] = Matrix_MultiplyVector(&matWorld, &t.p[0]);
        t.p[1] = Matrix_MultiplyVector(&matWorld, &t.p[1]);
        t.p[2] = Matrix_MultiplyVector(&matWorld, &t.p[2]);

        //triangle normal compute for show if facing the camera
        //triangle normal compute for show if facing the camera
        line1 = Vec_Subtract(t.p[1], t.p[0]);
        line2 = Vec_Subtract(t.p[2], t.p[0]);

        normal = Vec_CrossProduct(line1, line2);
        normal = Vec_Normalise(normal);
        //triangle normal compute for show if facing the camera
        //triangle normal compute for show if facing the camera


        //triangle not facing camera 
        vec3 vCameraRay = Vec_Subtract(t.p[0], global.g_camera);
        if(Vec_DotProduct(normal, vCameraRay)<0.0f)
        {
            ////// implement illumination by distance from camera (near lighter, far darker)  /////
            vec3 light_direction = { 0.0f, 0.0f, -1.0f };
            light_direction = Vec_Normalise(light_direction);

				    // How similar is normal to light direction
            float dp = Max(0.1f, Vec_DotProduct(light_direction, normal));
				    t.color = GetColour(dp);
            ////// implement illumination by distance from camera (near lighter, far darker)  /////

            //project triangles from 3D --> 2D
            t.p[0] = Matrix_MultiplyVector(&mproj, &t.p[0]);
            t.p[1] = Matrix_MultiplyVector(&mproj, &t.p[1]);
            t.p[2] = Matrix_MultiplyVector(&mproj, &t.p[2]);

            t.p[0] = Vec_Div(t.p[0], t.p[0].w);  // t.p[0].w = 1;
            t.p[1] = Vec_Div(t.p[1], t.p[1].w);  // t.p[0].w = 1;
            t.p[2] = Vec_Div(t.p[2], t.p[2].w);  // t.p[0].w = 1;

            //Scale into view
            vec3 vOffsetView = {1,1,0};
            t.p[0] = Vec_Add(t.p[0], vOffsetView);
            t.p[1] = Vec_Add(t.p[1], vOffsetView);
            t.p[2] = Vec_Add(t.p[2], vOffsetView);
            t.p[0].x *= 0.5f*(float)SCREEN_WIDTH;
            t.p[1].x *= 0.5f*(float)SCREEN_WIDTH;
            t.p[2].x *= 0.5f*(float)SCREEN_WIDTH;
            t.p[0].y *= 0.5f*(float)SCREEN_HEIGHT;
            t.p[1].y *= 0.5f*(float)SCREEN_HEIGHT;
            t.p[2].y *= 0.5f*(float)SCREEN_HEIGHT;

            ArrTri[i] = t;
            ArrTriBool[i] = true;
        } else{
            ArrTri[i] = t;   
            ArrTriBool[i] = false;
        }
    }

    //remove Triangles not rendermesh_box->tris_numand re-align
    int sizeToRenderTri = 0;
    int ArrTriIndexExist[mesh_box->tris_num];
    for(int i=0; i<mesh_box->tris_num; i++){
        if(ArrTriBool[i]==true){  
            sizeToRenderTri++;
        }
    }
    Triangle ArrTri_To_Render[sizeToRenderTri];
    int idx = 0;
    for(int i=0; i<mesh_box->tris_num; i++){
        if(ArrTriBool[i]==true){
            ArrTri_To_Render[idx]= ArrTri[i];
            idx++;
        }
    }

    // sort through z
    int numElements = sizeof(ArrTri_To_Render) / sizeof(Triangle);
    qsort(ArrTri_To_Render, numElements, sizeof(Triangle), compareMyStructs);
    for(int i=0; i<sizeToRenderTri; i++){
        Render_TriangleFill(ArrTri_To_Render[i]);
        Render_TriangleLines(ArrTri_To_Render[i]);
    }

    SDL_UpdateWindowSurface(global.g_window);
    // SDL_Delay(16);
}

void mesh_render_static(Mesh *mesh_box){

    //Mesh triangles
    vec3 normal, line1, line2;
    Triangle ArrTri[mesh_box->tris_num];
    bool ArrTriBool[mesh_box->tris_num];
    memset(ArrTriBool, false, sizeof(ArrTriBool)); 

    // Matrix *mproj = mat_create_projectionmatrix_sample();
    Matrix mproj = Matrix_MakeProjection(90.0f, (float)SCREEN_HEIGHT/(float)SCREEN_WIDTH, 0.1f, 1000.0f);

    //Offset into the screen
    Matrix matTrans;
    matTrans = Matrix_MakeTranslation(0, 0, 4.0f);

    Matrix matWorld;
    matWorld = Matrix_MakeIdentity();
    // matWorld = Matrix_MultiplyMatrix(&global.matRotZ, &global.matRotX);
    matWorld = Matrix_MultiplyMatrix(&matWorld, &matTrans);
    for(size_t i=0; i<mesh_box->tris_num; i++){
        Triangle t;
        vec3 tri_vec1 = (*(mesh_box->tris[i])).p[0];
        vec3 tri_vec2 = (*(mesh_box->tris[i])).p[1];
        vec3 tri_vec3 = (*(mesh_box->tris[i])).p[2];
        t.p[0] = tri_vec1;
        t.p[1] = tri_vec2;
        t.p[2] = tri_vec3;

        t.p[0] = Matrix_MultiplyVector(&matWorld, &t.p[0]);
        t.p[1] = Matrix_MultiplyVector(&matWorld, &t.p[1]);
        t.p[2] = Matrix_MultiplyVector(&matWorld, &t.p[2]);

        //triangle normal compute for show if facing the camera
        //triangle normal compute for show if facing the camera
        line1 = Vec_Subtract(t.p[1], t.p[0]);
        line2 = Vec_Subtract(t.p[2], t.p[0]);

        normal = Vec_CrossProduct(line1, line2);
        normal = Vec_Normalise(normal);
        //triangle normal compute for show if facing the camera
        //triangle normal compute for show if facing the camera


        //triangle not facing camera 
        vec3 vCameraRay = Vec_Subtract(t.p[0], global.g_camera);
        if(Vec_DotProduct(normal, vCameraRay)<0.0f)
        {
            ////// implement illumination by distance from camera (near lighter, far darker)  /////
            vec3 light_direction = { 0.0f, 0.0f, -1.0f };
            light_direction = Vec_Normalise(light_direction);

				    // How similar is normal to light direction
            float dp = Max(0.1f, Vec_DotProduct(light_direction, normal));
				    t.color = GetColour(dp);
            ////// implement illumination by distance from camera (near lighter, far darker)  /////

            //project triangles from 3D --> 2D
            t.p[0] = Matrix_MultiplyVector(&mproj, &t.p[0]);
            t.p[1] = Matrix_MultiplyVector(&mproj, &t.p[1]);
            t.p[2] = Matrix_MultiplyVector(&mproj, &t.p[2]);

            t.p[0] = Vec_Div(t.p[0], t.p[0].w);  // t.p[0].w = 1;
            t.p[1] = Vec_Div(t.p[1], t.p[1].w);  // t.p[0].w = 1;
            t.p[2] = Vec_Div(t.p[2], t.p[2].w);  // t.p[0].w = 1;

            //Scale into view
            vec3 vOffsetView = {1,1,0};
            t.p[0] = Vec_Add(t.p[0], vOffsetView);
            t.p[1] = Vec_Add(t.p[1], vOffsetView);
            t.p[2] = Vec_Add(t.p[2], vOffsetView);
            t.p[0].x *= 0.5f*(float)SCREEN_WIDTH;
            t.p[1].x *= 0.5f*(float)SCREEN_WIDTH;
            t.p[2].x *= 0.5f*(float)SCREEN_WIDTH;
            t.p[0].y *= 0.5f*(float)SCREEN_HEIGHT;
            t.p[1].y *= 0.5f*(float)SCREEN_HEIGHT;
            t.p[2].y *= 0.5f*(float)SCREEN_HEIGHT;

            ArrTri[i] = t;
            ArrTriBool[i] = true;
        } else{
            ArrTri[i] = t;   
            ArrTriBool[i] = false;
        }
    }

    //remove Triangles not rendermesh_box->tris_numand re-align
    int sizeToRenderTri = 0;
    int ArrTriIndexExist[mesh_box->tris_num];
    for(int i=0; i<mesh_box->tris_num; i++){
        if(ArrTriBool[i]==true){  
            sizeToRenderTri++;
        }
    }
    Triangle ArrTri_To_Render[sizeToRenderTri];
    int idx = 0;
    for(int i=0; i<mesh_box->tris_num; i++){
        if(ArrTriBool[i]==true){
            ArrTri_To_Render[idx]= ArrTri[i];
            idx++;
        }
    }

    // sort through z
    int numElements = sizeof(ArrTri_To_Render) / sizeof(Triangle);
    qsort(ArrTri_To_Render, numElements, sizeof(Triangle), compareMyStructs);
    for(int i=0; i<sizeToRenderTri; i++){
        Render_TriangleFill(ArrTri_To_Render[i]);
        Render_TriangleLines(ArrTri_To_Render[i]);
    }

    SDL_UpdateWindowSurface(global.g_window);
    // SDL_Delay(16);
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
    clock_t start, diff, clock_start, clock_end;
    double frame_time_seconds, fps;
    int frame_count;
    float fElapsedTime =0;
    start = clock();
    clock_start = clock();
    init_global_properties();
    Mesh *mesh_box = mesh_create_loadfromObj();
    mesh_loadfrom_Obj(mesh_box);
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
        // mesh_render(mesh_box, fElapsedTime);
        mesh_render_static(mesh_box);
        diff = clock() - start;
        fElapsedTime = diff * 1000.0f / CLOCKS_PER_SEC;
        Render_FillScreenBlue();
        UnlockScreenSurface();

        clock_end = clock();
        frame_count++;
        frame_time_seconds = (double)(clock_end - clock_start) / CLOCKS_PER_SEC;
        if(frame_time_seconds>=1.0){
            fps = (double)frame_count/frame_time_seconds;
            SDL_Log("FPS: %f\n", fps);
            clock_start = clock_end;
            frame_count = 0;
        }
    }

    // UnlockScreenSurface();
    mesh_free(mesh_box);
    SDL_Delay(3000);
    SDL_DestroyWindow(global.g_window);
    SDL_Quit();

    return 0;
}

