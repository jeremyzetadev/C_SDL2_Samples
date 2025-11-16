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
int mesh_loadfrom_Obj(Mesh *mesh, char *objFile);
Mesh *mesh_create_loadfromObj(char *objFile);
void mesh_transform_translate(Mesh *mesh, vec3 dir);

float dist_point_triangle(vec3 *plane_n, vec3 *plane_p, vec3 p);
int Triangle_ClipAgainstPlane(vec3 *plane_p, vec3 *plane_n, Triangle *in_tri, Triangle *out_tri1, Triangle *out_tri2);
