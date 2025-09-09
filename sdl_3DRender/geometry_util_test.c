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
    return m;
}

void mesh_init_tris(Mesh *m){
    // vec3 v = {0,0,0};      //simple init struct
    // m->tris->p[0].x = 5;   //assign value at specified end member
    // m->tris->p[0] = v;        // can init
    // m->tris->p[0] = {0,0,0};  // cannot init
    
    // TODO implement full init of tris
    vec3 arr_v[] = {{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}}; // v[3]
    for(size_t i=0; i<m->tris_num; i++){
        m->tris->p[i] = arr_v[i];
    }
}

void mesh_init_tris_sample(Mesh *m){
    vec3 arr_v[] = {
        {0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}
    };
    for(size_t i=0; i<m->tris_num; i++){
        m->tris->p[i] = arr_v[i];
    }

}
