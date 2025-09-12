#pragma once
#include<stdbool.h>

typedef struct {
    float x;
    float y;
    float z;
} vec3;

typedef struct {
    float m[4][4];
} Matrix;

Matrix *mat_create();
Matrix *mat_create_projectionmatrix(float near, float far, float fFov, float fAspectRatio);
Matrix *mat_create_projectionmatrix_sample();
vec3 MultiplyMatrixVector(vec3 i, Matrix m);


vec3 Vec_Add(vec3 a, vec3 b);
vec3 Vec_Subtract(vec3 a, vec3 b);
float DotProduct(vec3 a, vec3 b);
vec3 CrossProduct(vec3 a, vec3 b);
vec3 Vec_Perpendicular(vec3 v);
bool IsPointOnRightSideOfLine(vec3 a, vec3 b, vec3 p);
bool IsPointInTriangle(vec3 a, vec3 b, vec3 c, vec3 p);


int compareMyStructs(const void *a, const void *b);
