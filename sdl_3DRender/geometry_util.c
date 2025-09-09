#include"geometry_util.h"
#include"math_util.h"
#include<stdio.h>
#include<stdlib.h>

Mesh *mesh_create(size_t tris_num){
    Mesh *m = malloc(sizeof(Mesh));
    if(!m){
        fprintf(stderr, "Error creating mesh");
        exit(1);
    }
    m->tris = malloc(sizeof(Triangle) * tris_num);
    m->tris_num = tris_num;
    return m;
}

void mesh_init_tris(Mesh *m){
    // TODO implement full init of tris
    vec3 arr_v[] = {{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}}; // v[3]
                                                                         
    Triangle **arrTri = malloc(sizeof(Triangle) * m->tris_num);
    for(size_t i=0; i<m->tris_num; i++){
        Triangle t;
        for(int j=0; j<3; j++){
            t.p[j] = arr_v[i];
        }
        arrTri[i] = &t;
    }
    m->tris= arrTri;
}

void mesh_init_tris_SampleBox(Mesh *m){
    vec3 arr_v[] = {
        // SOUTH
        { 0.0f, 0.0f, 0.0f},    {0.0f, 1.0f, 0.0f},    {1.0f, 1.0f, 0.0f},
        { 0.0f, 0.0f, 0.0f},    {1.0f, 1.0f, 0.0f},    {1.0f, 0.0f, 0.0f},

        // EAST                                                      
        { 1.0f, 0.0f, 0.0f},    {1.0f, 1.0f, 0.0f},    {1.0f, 1.0f, 1.0f},
        { 1.0f, 0.0f, 0.0f},    {1.0f, 1.0f, 1.0f},    {1.0f, 0.0f, 1.0f},

        // NORTH                                                     
        { 1.0f, 0.0f, 1.0f},    {1.0f, 1.0f, 1.0f},    {0.0f, 1.0f, 1.0f},
        { 1.0f, 0.0f, 1.0f},    {0.0f, 1.0f, 1.0f},    {0.0f, 0.0f, 1.0f},

        // WEST                                                      
        { 0.0f, 0.0f, 1.0f},    {0.0f, 1.0f, 1.0f},    {0.0f, 1.0f, 0.0f},
        { 0.0f, 0.0f, 1.0f},    {0.0f, 1.0f, 0.0f},    {0.0f, 0.0f, 0.0f},

        // TOP                                                       
        { 0.0f, 1.0f, 0.0f},    {0.0f, 1.0f, 1.0f},    {1.0f, 1.0f, 1.0f},
        { 0.0f, 1.0f, 0.0f},    {1.0f, 1.0f, 1.0f},    {1.0f, 1.0f, 0.0f},

        // BOTTOM                                                    
        { 1.0f, 0.0f, 1.0f},    {0.0f, 0.0f, 1.0f},    {0.0f, 0.0f, 0.0f},
        { 1.0f, 0.0f, 1.0f},    {0.0f, 0.0f, 0.0f},    {1.0f, 0.0f, 0.0f},    
    }; 
                                                                         
    Triangle **arrTri = malloc(sizeof(Triangle*) * m->tris_num); //sizeof Triangle pointer for double pointer
    for(size_t i=0; i<m->tris_num; i++){
        arrTri[i] = (Triangle*)malloc(sizeof(Triangle));
        for(int j=0; j<3; j++){
            arrTri[i]->p[j] = arr_v[i*3 + j];
        }
    }
    m->tris= arrTri;
}

void mesh_free(Mesh *mesh){
    if(mesh->tris!=NULL){
        free(mesh->tris);
    }
    free(mesh);
}













