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
#include "arraylist.h"

#define ERROR_EXIT(...) {fprintf(stderr, __VA_ARGS__); exit(1);}
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// float fTheta; //dynamic for rotation

void InitializeProgram(){
    if(SDL_Init(SDL_INIT_VIDEO) <0)
        ERROR_EXIT("SDL could not initialized, SDL_Error: %s\n" ,SDL_GetError());

    global.g_window = SDL_CreateWindow("Moving Camera Through 3D Space with rendered rotating obj", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
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
    global.objFile_extrudebox = "blender_legacyobj.obj";
    global.objFile_mountains = "blender_legacymountain.obj";
    global.max_distance_sight = 100;
}

void mesh_render(Mesh *mesh_box, float fElapsedTime, vec3 dirOffset){
    float fTheta =0;
    fTheta +=(0.002f * fElapsedTime);

    //Mesh triangles
    vec3 normal, line1, line2;
    Triangle **ArrTri_To_Render = malloc(sizeof(Triangle*) * mesh_box->tris_num*2);
    int ArrTri_To_Render_Size = 0;

    // Matrix *mproj = mat_create_projectionmatrix_sample();
    Matrix mproj = Matrix_MakeProjection(90.0f, (float)SCREEN_HEIGHT/(float)SCREEN_WIDTH, 0.1f, 1000.0f);
    //Rotate in Z-Axis
    global.matRotZ = Matrix_MakeRotationZ(fTheta * 0.5f);
    //Rotate in X-Axis
    global.matRotX = Matrix_MakeRotationX(fTheta); 

    //Offset into the screen or movement
    Matrix matTrans;
    matTrans = Matrix_MakeIdentity();
    matTrans = Matrix_MakeTranslation(dirOffset.x, dirOffset.y, dirOffset.z);

    Matrix matWorld;
    matWorld = Matrix_MakeIdentity();
    matWorld = Matrix_MultiplyMatrix(&global.matRotZ, &global.matRotX);
    matWorld = Matrix_MultiplyMatrix(&matWorld, &matTrans);

    global.vLookDir = (vec3){0, 0, 1};
    vec3 vUp = (vec3){0, -1, 0};  // camera is upsidedown if y = 1 (TODO: check orientation of mat and sdl to learn the positioning as well as cross product)
    // vec3 vTarget = Vec_Add(global.g_camera, global.vLookDir);
    vec3 vTarget = (vec3){0, 0, 1};
    Matrix matCameraRot = Matrix_MakeRotationY(global.fYaw);
    global.vLookDir = Matrix_MultiplyVector(&matCameraRot, &vTarget); 
    vTarget = Vec_Add(global.g_camera, global.vLookDir);

    Matrix matCamera = Matrix_PointAt(&global.g_camera, &vTarget, &vUp);
    Matrix matView = Matrix_QuickInverse(&matCamera);

    for(size_t i=0; i<mesh_box->tris_num; i++){
        Triangle t, tViewed;
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
            // only from 0-maxdistance should be seen triangles
            // if(abs((t.p[0].z+t.p[1].z+t.p[2].z)/3.0f - global.g_camera.z) > global.max_distance_sight) break; 

            ////// implement illumination by distance from camera (near lighter, far darker)  /////
            vec3 light_direction = { 0.0f, 0.0f, -1.0f };
            light_direction = Vec_Normalise(light_direction);

				    // How similar is normal to light direction
            float dp = Max(0.1f, Vec_DotProduct(light_direction, normal));
				    t.color = GetColour(dp);
            tViewed.color = t.color;
            ////// implement illumination by distance from camera (near lighter, far darker)  /////
            
            // Convert World Space --> View Space
            tViewed.p[0] = Matrix_MultiplyVector(&matView, &t.p[0]);
            tViewed.p[1] = Matrix_MultiplyVector(&matView, &t.p[1]);
            tViewed.p[2] = Matrix_MultiplyVector(&matView, &t.p[2]);
            
            // Clip Viewed Triangle against near plane, this could form two additioan triangles.
            int nClippedTriangle = 0;
            Triangle clipped[2];
            nClippedTriangle = Triangle_ClipAgainstPlane(&(vec3){0.0f, 0.0f, 0.1f,}, &(vec3){0.0f, 0.0f, 1.0f}, &tViewed, &clipped[0], &clipped[1]);

            for( int n=0; n<nClippedTriangle; n++){
                //project triangles from 3D --> 2D
                t.p[0] = Matrix_MultiplyVector(&mproj, &clipped[n].p[0]);
                t.p[1] = Matrix_MultiplyVector(&mproj, &clipped[n].p[1]);
                t.p[2] = Matrix_MultiplyVector(&mproj, &clipped[n].p[2]);

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
                
                ArrTri_To_Render[ArrTri_To_Render_Size] = (Triangle*)malloc(sizeof(Triangle));
                ArrTri_To_Render[ArrTri_To_Render_Size]->p[0] = t.p[0];
                ArrTri_To_Render[ArrTri_To_Render_Size]->p[1] = t.p[1];
                ArrTri_To_Render[ArrTri_To_Render_Size]->p[2] = t.p[2];
                ArrTri_To_Render[ArrTri_To_Render_Size]->color = t.color;
                ArrTri_To_Render_Size++;
            }
        }
    }
    //Shrink ArrTri_To_Render lengthofarray to ArrTri_To_Render_Size
    Triangle **ArrTri_To_Render_exact = realloc(ArrTri_To_Render, sizeof(Triangle*) * ArrTri_To_Render_Size);
    Arraylist *arrlist_monitorbound_clip = arraylist_create(sizeof(Triangle), 0);

    // sort through z (render from back to front)
    // qsort(ArrTri_To_Render, numElements, sizeof(Triangle), compareMyStructs);  //OLD
    qsort(ArrTri_To_Render_exact, ArrTri_To_Render_Size, sizeof(Triangle*), compareMyPtr); 
    for(int i=0; i<ArrTri_To_Render_Size; i++){
        // Render_TriangleFill(*ArrTri_To_Render_exact[i]);  //SLOW
        // Render_TriangleFill_ScanLine2(*ArrTri_To_Render_exact[i]);
        // Render_TriangleLines(*ArrTri_To_Render_exact[i]);

        Triangle clipped[2];
        int nTrisToAdd = 0;
        arraylist_append(arrlist_monitorbound_clip, ArrTri_To_Render_exact[i]);
        int nNewTriangles = 1;
        for (int p = 0; p < 4; p++)
        {
            while (nNewTriangles > 0)
            {
                // Take triangle from front of queue
                Triangle *test = arraylist_get(arrlist_monitorbound_clip, 0);
                arraylist_remove(arrlist_monitorbound_clip, 0);
                nNewTriangles--;

                // Clip it against a plane. We only need to test each 
                // subsequent plane, against subsequent new triangles
                // as all triangles after a plane clip are guaranteed
                // to lie on the inside of the plane. I like how this
                // comment is almost completely and utterly justified
                switch (p)
                {
                    case 0:	nTrisToAdd = Triangle_ClipAgainstPlane(&(vec3){ 0.0f, 0.0f, 0.0f }, &(vec3){ 0.0f, 1.0f, 0.0f }, test, &clipped[0], &clipped[1]); break;
                    case 1:	nTrisToAdd = Triangle_ClipAgainstPlane(&(vec3){ 0.0f, (float)SCREEN_HEIGHT - 1, 0.0f }, &(vec3){ 0.0f, -1.0f, 0.0f }, test, &clipped[0], &clipped[1]); break;
                    case 2:	nTrisToAdd = Triangle_ClipAgainstPlane(&(vec3){ 0.0f, 0.0f, 0.0f }, &(vec3){ 1.0f, 0.0f, 0.0f }, test, &clipped[0], &clipped[1]); break;
                    case 3:	nTrisToAdd = Triangle_ClipAgainstPlane(&(vec3){ (float)SCREEN_WIDTH - 1, 0.0f, 0.0f }, &(vec3){ -1.0f, 0.0f, 0.0f }, test, &clipped[0], &clipped[1]); break;
                }

                // Clipping may yield a variable number of triangles, so
                // add these new ones to the back of the queue for subsequent
                // clipping against next planes
                for (int w = 0; w < nTrisToAdd; w++)
                    arraylist_append(arrlist_monitorbound_clip, &clipped[w]);
            }
            nNewTriangles = arrlist_monitorbound_clip->len;
        }
        for(int j=0; j<arrlist_monitorbound_clip->len; j++){
            Triangle *t = arraylist_get(arrlist_monitorbound_clip, j);
            Render_TriangleFill_ScanLine2(*t);
            Render_TriangleLines(*t);
        }
    }


    for(int i=0; i<ArrTri_To_Render_Size; i++)
        free(ArrTri_To_Render[i]);
    free(ArrTri_To_Render);
    free(arrlist_monitorbound_clip);
}

void mesh_render_static(Mesh *mesh_box, vec3 dirOffset){

    //Mesh triangles
    vec3 normal, line1, line2;
    Triangle **ArrTri_To_Render = malloc(sizeof(Triangle*) * mesh_box->tris_num*2);
    int ArrTri_To_Render_Size = 0;

    // Matrix *mproj = mat_create_projectionmatrix_sample();
    Matrix mproj = Matrix_MakeProjection(90.0f, (float)SCREEN_HEIGHT/(float)SCREEN_WIDTH, 0.1f, 1000.0f);

    //Offset into the screen or movement
    Matrix matTrans;
    matTrans = Matrix_MakeIdentity();
    matTrans = Matrix_MakeTranslation(dirOffset.x, dirOffset.y, dirOffset.z);

    Matrix matWorld;
    matWorld = Matrix_MakeIdentity();
    // matWorld = Matrix_MultiplyMatrix(&global.matRotZ, &global.matRotX);
    matWorld = Matrix_MultiplyMatrix(&matWorld, &matTrans);

    global.vLookDir = (vec3){0, 0, 1};
    vec3 vUp = (vec3){0, -1, 0};  // camera is upsidedown if y = 1 (TODO: check orientation of mat and sdl to learn the positioning as well as cross product)
    // vec3 vTarget = Vec_Add(global.g_camera, global.vLookDir);
    vec3 vTarget = (vec3){0, 0, 1};
    Matrix matCameraRot = Matrix_MakeRotationY(global.fYaw);
    global.vLookDir = Matrix_MultiplyVector(&matCameraRot, &vTarget); 
    vTarget = Vec_Add(global.g_camera, global.vLookDir);

    Matrix matCamera = Matrix_PointAt(&global.g_camera, &vTarget, &vUp);
    Matrix matView = Matrix_QuickInverse(&matCamera);

    for(size_t i=0; i<mesh_box->tris_num; i++){
        Triangle t, tViewed;
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
            // only from 0-maxdistance should be seen triangles
            // if(abs((t.p[0].z+t.p[1].z+t.p[2].z)/3.0f - global.g_camera.z) > global.max_distance_sight) break; 

            ////// implement illumination by distance from camera (near lighter, far darker)  /////
            vec3 light_direction = { 0.0f, 0.0f, -1.0f };
            light_direction = Vec_Normalise(light_direction);

				    // How similar is normal to light direction
            float dp = Max(0.1f, Vec_DotProduct(light_direction, normal));
				    t.color = GetColour(dp);
            tViewed.color = t.color;
            ////// implement illumination by distance from camera (near lighter, far darker)  /////
            
            // Convert World Space --> View Space
            tViewed.p[0] = Matrix_MultiplyVector(&matView, &t.p[0]);
            tViewed.p[1] = Matrix_MultiplyVector(&matView, &t.p[1]);
            tViewed.p[2] = Matrix_MultiplyVector(&matView, &t.p[2]);
            
            // Clip Viewed Triangle against near plane, this could form two additioan triangles.
            int nClippedTriangle = 0;
            Triangle clipped[2];
            nClippedTriangle = Triangle_ClipAgainstPlane(&(vec3){0.0f, 0.0f, 0.1f,}, &(vec3){0.0f, 0.0f, 1.0f}, &tViewed, &clipped[0], &clipped[1]);

            for( int n=0; n<nClippedTriangle; n++){
                //project triangles from 3D --> 2D
                t.p[0] = Matrix_MultiplyVector(&mproj, &clipped[n].p[0]);
                t.p[1] = Matrix_MultiplyVector(&mproj, &clipped[n].p[1]);
                t.p[2] = Matrix_MultiplyVector(&mproj, &clipped[n].p[2]);

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
                
                ArrTri_To_Render[ArrTri_To_Render_Size] = (Triangle*)malloc(sizeof(Triangle));
                ArrTri_To_Render[ArrTri_To_Render_Size]->p[0] = t.p[0];
                ArrTri_To_Render[ArrTri_To_Render_Size]->p[1] = t.p[1];
                ArrTri_To_Render[ArrTri_To_Render_Size]->p[2] = t.p[2];
                ArrTri_To_Render[ArrTri_To_Render_Size]->color = t.color;
                ArrTri_To_Render_Size++;
            }
        }
    }
    //Shrink ArrTri_To_Render lengthofarray to ArrTri_To_Render_Size
    Triangle **ArrTri_To_Render_exact = realloc(ArrTri_To_Render, sizeof(Triangle*) * ArrTri_To_Render_Size);
    Arraylist *arrlist_monitorbound_clip = arraylist_create(sizeof(Triangle), 0);

    // sort through z (render from back to front)
    // qsort(ArrTri_To_Render, numElements, sizeof(Triangle), compareMyStructs);  //OLD
    qsort(ArrTri_To_Render_exact, ArrTri_To_Render_Size, sizeof(Triangle*), compareMyPtr); 
    for(int i=0; i<ArrTri_To_Render_Size; i++){
        // Render_TriangleFill(*ArrTri_To_Render_exact[i]);  //SLOW
        // Render_TriangleFill_ScanLine2(*ArrTri_To_Render_exact[i]);
        // Render_TriangleLines(*ArrTri_To_Render_exact[i]);

        Triangle clipped[2];
        int nTrisToAdd = 0;
        arraylist_append(arrlist_monitorbound_clip, ArrTri_To_Render_exact[i]);
        int nNewTriangles = 1;
        for (int p = 0; p < 4; p++)
        {
            while (nNewTriangles > 0)
            {
                // Take triangle from front of queue
                Triangle *test = arraylist_get(arrlist_monitorbound_clip, 0);
                arraylist_remove(arrlist_monitorbound_clip, 0);
                nNewTriangles--;

                // Clip it against a plane. We only need to test each 
                // subsequent plane, against subsequent new triangles
                // as all triangles after a plane clip are guaranteed
                // to lie on the inside of the plane. I like how this
                // comment is almost completely and utterly justified
                switch (p)
                {
                    case 0:	nTrisToAdd = Triangle_ClipAgainstPlane(&(vec3){ 0.0f, 0.0f, 0.0f }, &(vec3){ 0.0f, 1.0f, 0.0f }, test, &clipped[0], &clipped[1]); break;
                    case 1:	nTrisToAdd = Triangle_ClipAgainstPlane(&(vec3){ 0.0f, (float)SCREEN_HEIGHT - 1, 0.0f }, &(vec3){ 0.0f, -1.0f, 0.0f }, test, &clipped[0], &clipped[1]); break;
                    case 2:	nTrisToAdd = Triangle_ClipAgainstPlane(&(vec3){ 0.0f, 0.0f, 0.0f }, &(vec3){ 1.0f, 0.0f, 0.0f }, test, &clipped[0], &clipped[1]); break;
                    case 3:	nTrisToAdd = Triangle_ClipAgainstPlane(&(vec3){ (float)SCREEN_WIDTH - 1, 0.0f, 0.0f }, &(vec3){ -1.0f, 0.0f, 0.0f }, test, &clipped[0], &clipped[1]); break;
                }

                // Clipping may yield a variable number of triangles, so
                // add these new ones to the back of the queue for subsequent
                // clipping against next planes
                for (int w = 0; w < nTrisToAdd; w++)
                    arraylist_append(arrlist_monitorbound_clip, &clipped[w]);
            }
            nNewTriangles = arrlist_monitorbound_clip->len;
        }
        for(int j=0; j<arrlist_monitorbound_clip->len; j++){
            Triangle *t = arraylist_get(arrlist_monitorbound_clip, j);
            // Render_TriangleFill_ScanLine2(*t);
            Render_TriangleLines(*t);
        }
    }
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

    Mesh *mesh_box = mesh_create_loadfromObj(global.objFile_extrudebox);
    mesh_loadfrom_Obj(mesh_box, global.objFile_extrudebox);
    // mesh_transform_translate(mesh_box, (vec3){0.0f, 0.0f, 10.0f});
    Mesh *mesh_box2 = mesh_create_loadfromObj(global.objFile_extrudebox);
    mesh_loadfrom_Obj(mesh_box2, global.objFile_extrudebox);
    // mesh_transform_translate(mesh_box2, (vec3){4.0f, 0.0f, 4.0f});
    Mesh *mesh_box3 = mesh_create_loadfromObj(global.objFile_extrudebox);
    mesh_loadfrom_Obj(mesh_box3, global.objFile_extrudebox);
    // mesh_transform_translate(mesh_box3, (vec3){-4.0f, 0.0f, 4.0f});
    // Mesh *mesh_mountains = mesh_create_loadfromObj(global.objFile_mountains);
    // mesh_loadfrom_Obj(mesh_mountains, global.objFile_mountains);

    while(isGameRunning){
        LockScreenSurface();
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    isGameRunning = false;
                    break;
                case SDL_KEYDOWN:                                        
                    switch(event.key.keysym.scancode){
                        case SDL_SCANCODE_A:
                            global.fYaw += 0.2f;  //*fElapsedTime use time to smooth movement
                            break;
                        case SDL_SCANCODE_D:
                            global.fYaw -= 0.2;
                            break;
                        case SDL_SCANCODE_W:
                            vec3 vForward = Vec_Multiply(global.vLookDir, 2.0f);
                            global.g_camera = Vec_Add(global.g_camera, vForward);
                            break;
                        case SDL_SCANCODE_S:
                            vec3 vBackward = Vec_Multiply(global.vLookDir, 2.0f);
                            global.g_camera = Vec_Subtract(global.g_camera, vBackward);
                            break;
                        case SDL_SCANCODE_UP:
                            global.g_camera = (vec3){global.g_camera.x, global.g_camera.y + 1.0f, global.g_camera.z};
                            break;
                        case SDL_SCANCODE_DOWN:
                            global.g_camera = (vec3){global.g_camera.x, global.g_camera.y - 1.0f, global.g_camera.z};
                            break;
                        default:
                            break;
                    }
                default:
                    break;
            }
        }
        //mesh render sample
        mesh_render(mesh_box, fElapsedTime, (vec3){0.0f, 0.0f, 7.0f});
        mesh_render(mesh_box2, fElapsedTime, (vec3){4.0f, 0.0f, 7.0f});
        mesh_render(mesh_box3, fElapsedTime, (vec3){-4.0f, 0.0f, 7.0f});
        // mesh_render_static(mesh_box, (vec3){0.0f, 0.0f, 7.0f});
        // mesh_render_static(mesh_box2, (vec3){4.0f, 0.0f, 7.0f});
        // mesh_render_static(mesh_box3, (vec3){-4.0f, 0.0f, 7.0f});
        // mesh_render_static(mesh_mountains, (vec3){0.0f, 0.0f, 0.0f});
        SDL_UpdateWindowSurface(global.g_window); 
        // mesh_render_static(mesh_box);
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
            SDL_Log("fEllapsedTime: %f\n", fElapsedTime);
            SDL_Log("cameraPos: %f\t %f\t %f\n", global.g_camera.x, global.g_camera.y, global.g_camera.z);
            SDL_Log("viewDirection: %f\t %f\t %f\n", global.vLookDir.x, global.vLookDir.y, global.vLookDir.z);
            clock_start = clock_end;
            frame_count = 0;
        }
    }

    // UnlockScreenSurface();
    mesh_free(mesh_box);
    mesh_free(mesh_box2);
    mesh_free(mesh_box3);
    SDL_Delay(3000);
    SDL_DestroyWindow(global.g_window);
    SDL_Quit();

    return 0;
}

