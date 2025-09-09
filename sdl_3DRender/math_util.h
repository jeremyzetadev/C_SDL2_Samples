#pragma once

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
