#pragma once

#include<stdio.h>
#include "math_util.h"

typedef struct {
    vec3 p[3];
    int num;
} Triangle;

typedef struct {
    Triangle *tris;
    size_t tris_num;
} Mesh;

Mesh *mesh_create(size_t tris_num);
void mesh_init_tris(Mesh *mesh);
