#pragma once

#include<stdio.h>
#include "math_util.h"

typedef struct {
    vec3 p[3];
    size_t color;
} Triangle;

typedef struct {
    Triangle **tris;
    size_t tris_num;
    size_t vert_num;
} Mesh;

Mesh *mesh_create(size_t tris_num);
void mesh_init_tris(Mesh *mesh);
void mesh_init_tris_SampleBox(Mesh *mesh);
void mesh_free(Mesh *mesh);
int mesh_loadfrom_Obj(Mesh *mesh);
Mesh *mesh_create_loadfromObj();
void mesh_transform_translate(Mesh *mesh, vec3 dir);
