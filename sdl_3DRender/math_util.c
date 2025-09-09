#include "math_util.h"
#include <math.h>
#include <stdlib.h>

Matrix *mat_create(){
    Matrix *m = malloc(sizeof(Matrix));
    return m;
}

Matrix *mat_create_projectionmatrix_sample(){
    Matrix *matProj = malloc(sizeof(Matrix));
    // initialize matrix values to 0
    // can also use memset to initialize matrix to 0
    for(int i=0; i<=4; i++){
        for(int j=0; j<=4; j++){
            matProj->m[i][j] = 0.0f;
        }
    }

    float fNear = 0.1f;
    float fFar = 1000.0f;
    float fFov = 90.0f;
    float fAspectRatio = (float)600 / (float)800;   // height/width
    float fFovRad = 1.0f / tanf(fFov * 0.5f/180.0f * 3.14159f);

    matProj->m[0][0] = fAspectRatio * fFovRad;
    matProj->m[1][1] = fFovRad;
    matProj->m[2][2] = fFar / (fFar- fNear);
    matProj->m[3][2] = (-fFar * fNear) / (fFar - fNear);
    matProj->m[2][3] = 1.0f;
    matProj->m[3][3] = 0.0f;
    return matProj;
}

Matrix *mat_create_projectionmatrix(float near, 
                                    float far, 
                                    float fFov, 
                                    float fAspectRatio)
{
    Matrix *m = malloc(sizeof(Matrix));
    return m;
}

vec3 MultiplyMatrixVector(vec3 i, Matrix m){
    vec3 o;
    o.x     = i.x*m.m[0][0] + i.y*m.m[1][0] + i.z*m.m[2][0] + m.m[3][0];
    o.y     = i.x*m.m[0][1] + i.y*m.m[1][1] + i.z*m.m[2][1] + m.m[3][1];
    o.z     = i.x*m.m[0][2] + i.y*m.m[1][2] + i.z*m.m[2][2] + m.m[3][2];
    float w = i.x*m.m[0][3] + i.y*m.m[1][3] + i.z*m.m[2][3] + m.m[3][3];

    if(w != 0.0f){
        o.x /= w;
        o.y /= w;
        o.z /= w;
    }
    return o;
}
