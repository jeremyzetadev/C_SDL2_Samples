#pragma once
#include<stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>

typedef struct {
    float x;
    float y;
    float z;
    float w;
} vec3;

vec3 vec3_init(vec3 v);

typedef struct {
    float m[4][4];
} Matrix;

Matrix *mat_create();
// Matrix *mat_create_projectionmatrix_sample();
vec3 MultiplyMatrixVector_old(vec3 i, Matrix m);
vec3 Matrix_MultiplyVector(Matrix *m, vec3 *i);
Matrix Matrix_MakeIdentity();
Matrix Matrix_MakeRotationX(float fAngleRad);
Matrix Matrix_MakeRotationY(float fAngleRad);
Matrix Matrix_MakeRotationZ(float fAngleRad);
Matrix Matrix_MakeTranslation(float x, float y, float z);
Matrix Matrix_MakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar);
Matrix Matrix_MultiplyMatrix(Matrix *m1, Matrix *m2);

Matrix Matrix_PointAt(vec3 *pos, vec3 *target, vec3 *up);
Matrix Matrix_QuickInverse(Matrix *m);

vec3 Vec_Add(vec3 a, vec3 b);
vec3 Vec_Subtract(vec3 a, vec3 b);
vec3 Vec_Multiply(vec3 a, float multiplier);
vec3 Vec_Div(vec3 a, float divisor);
float Vec_Length(vec3 v);
vec3 Vec_Normalise(vec3 v);
float Vec_DotProduct(vec3 a, vec3 b);
vec3 Vec_CrossProduct(vec3 a, vec3 b);
vec3 Vec_Perpendicular(vec3 v);

bool IsPointOnRightSideOfLine(vec3 a, vec3 b, vec3 p);
bool IsPointInTriangle(vec3 a, vec3 b, vec3 c, vec3 p);

Uint32 GetColour(float lum);
int compareMyStructs(const void *a, const void *b);

float static Min(float x, float y){
    return x < y ? x : y;
}

float static Max(float x, float y){
    return x > y ? x : y;
}

float static Clamp(float high, float low, float value){
    if(value > high){
        return high;
    } else if(value < low){
        return low;
    }
    return value;
}
