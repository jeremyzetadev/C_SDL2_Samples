#pragma once

#include<stdio.h>
#include "math_util.h"

typedef struct {
    vec3 p[3];
} Triangle;

typedef struct {
    Triangle **tris;
    size_t tris_num;
} Mesh;

Mesh *mesh_create(size_t tris_num);
void mesh_init_tris(Mesh *mesh);
void mesh_init_tris_SampleBox(Mesh *mesh);
void mesh_free(Mesh *mesh);
