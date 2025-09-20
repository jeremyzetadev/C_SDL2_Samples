#include "math_util.h"
#include <math.h>
#include <stdlib.h>
#include "geometry_util.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include "global.h"
#include <stdint.h>

vec3 vec3_init(vec3 v){
    v.x = 0;
    v.x = 0;
    v.x = 0;
    v.x = 1;
    return v;
}

Matrix *mat_create(){
    Matrix *m = malloc(sizeof(Matrix));
    return m;
}

// Matrix *mat_create_projectionmatrix(float near, 
//                                     float far, 
//                                     float fFov, 
//                                     float fAspectRatio)
// {
//     Matrix *m = malloc(sizeof(Matrix));
//     return m;
// }

vec3 MultiplyMatrixVector_old(vec3 i, Matrix m){
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

vec3 Matrix_MultiplyVector(Matrix *m, vec3 *i){
    vec3 v;
    v.x     = i->x*m->m[0][0] + i->y*m->m[1][0] + i->z*m->m[2][0] + m->m[3][0];
    v.y     = i->x*m->m[0][1] + i->y*m->m[1][1] + i->z*m->m[2][1] + m->m[3][1];
    v.z     = i->x*m->m[0][2] + i->y*m->m[1][2] + i->z*m->m[2][2] + m->m[3][2];
    v.w     = i->x*m->m[0][3] + i->y*m->m[1][3] + i->z*m->m[2][3] + m->m[3][3];
    return v;
}

Matrix Matrix_MakeIdentity(){
    Matrix matrix;
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            matrix.m[i][j] = 0.0f;
        }
    }
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    return matrix;
}

Matrix Matrix_MakeRotationX(float fAngleRad)
{
    Matrix matrix = Matrix_MakeIdentity();
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = cosf(fAngleRad);
    matrix.m[1][2] = sinf(fAngleRad);
    matrix.m[2][1] = -sinf(fAngleRad);
    matrix.m[2][2] = cosf(fAngleRad);
    matrix.m[3][3] = 1.0f;
    return matrix;
}

Matrix Matrix_MakeRotationY(float fAngleRad)
{
    Matrix matrix = Matrix_MakeIdentity();
    matrix.m[0][0] = cosf(fAngleRad);
    matrix.m[0][2] = sinf(fAngleRad);
    matrix.m[2][0] = -sinf(fAngleRad);
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = cosf(fAngleRad);
    matrix.m[3][3] = 1.0f;
    return matrix;
}

Matrix Matrix_MakeRotationZ(float fAngleRad)
{
    Matrix matrix =Matrix_MakeIdentity();
    matrix.m[0][0] = cosf(fAngleRad);
    matrix.m[0][1] = sinf(fAngleRad);
    matrix.m[1][0] = -sinf(fAngleRad);
    matrix.m[1][1] = cosf(fAngleRad);
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    return matrix;
}

Matrix Matrix_MakeTranslation(float x, float y, float z)
{
    Matrix matrix =Matrix_MakeIdentity();
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    matrix.m[3][0] = x;
    matrix.m[3][1] = y;
    matrix.m[3][2] = z;
    return matrix;
}

Matrix Matrix_MakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar)
{
    float fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * 3.14159f);
    Matrix matrix = Matrix_MakeIdentity();
    matrix.m[0][0] = fAspectRatio * fFovRad;
    matrix.m[1][1] = fFovRad;
    matrix.m[2][2] = fFar / (fFar - fNear);
    matrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    matrix.m[2][3] = 1.0f;
    matrix.m[3][3] = 0.0f;
    return matrix;
}

Matrix Matrix_MultiplyMatrix(Matrix *m1, Matrix *m2)
{
    Matrix matrix = Matrix_MakeIdentity();
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            matrix.m[r][c] = m1->m[r][0] * m2->m[0][c] + m1->m[r][1] * m2->m[1][c] + m1->m[r][2] * m2->m[2][c] + m1->m[r][3] * m2->m[3][c];
    return matrix;
}

// in tutorial of javidx, he use reference(c++) so he can pass-byref and calc as object
Matrix Matrix_PointAt(vec3 *pos, vec3 *target, vec3 *up){
    vec3 newForward = Vec_Subtract(*target, *pos);
    newForward = Vec_Normalise(newForward);

    vec3 a = Vec_Multiply(newForward, Vec_DotProduct(*up, newForward));
    vec3 newUp = Vec_Subtract(*up, a);
    newUp = Vec_Normalise(newUp);

    vec3 newRight = Vec_CrossProduct(newUp, newForward);

    Matrix mat = Matrix_MakeIdentity();
    mat.m[0][0] = newRight.x;        mat.m[0][1]=newRight.y;      mat.m[0][2]=newRight.z;
    mat.m[1][0] = newUp.x;           mat.m[1][1]=newUp.y;         mat.m[1][2]=newUp.z;
    mat.m[2][0] = newForward.x;      mat.m[2][1]=newForward.y;    mat.m[2][2]=newForward.z;
    mat.m[3][0] = pos->x;             mat.m[3][1]=pos->y;           mat.m[3][2]=pos->z;

    return mat;
}

Matrix Matrix_QuickInverse(Matrix *m){
    Matrix mat = Matrix_MakeIdentity();
		mat.m[0][0] = m->m[0][0]; mat.m[0][1] = m->m[1][0]; mat.m[0][2] = m->m[2][0]; mat.m[0][3] = 0.0f;
		mat.m[1][0] = m->m[0][1]; mat.m[1][1] = m->m[1][1]; mat.m[1][2] = m->m[2][1]; mat.m[1][3] = 0.0f;
		mat.m[2][0] = m->m[0][2]; mat.m[2][1] = m->m[1][2]; mat.m[2][2] = m->m[2][2]; mat.m[2][3] = 0.0f;
		mat.m[3][0] = -(m->m[3][0] * mat.m[0][0] + m->m[3][1] * mat.m[1][0] + m->m[3][2] * mat.m[2][0]);
		mat.m[3][1] = -(m->m[3][0] * mat.m[0][1] + m->m[3][1] * mat.m[1][1] + m->m[3][2] * mat.m[2][1]);
		mat.m[3][2] = -(m->m[3][0] * mat.m[0][2] + m->m[3][1] * mat.m[1][2] + m->m[3][2] * mat.m[2][2]);
		mat.m[3][3] = 1.0f;
    return mat;
}

// no need to use pointer ([float]4bytes + [float]4bytes = [pointer]8bytes)
vec3 Vec_Add(vec3 a, vec3 b){
    vec3 v_res = (vec3){a.x+b.x, a.y+b.y, a.z+b.z};
    return v_res;
}

vec3 Vec_Subtract(vec3 a, vec3 b){
    vec3 v_res = (vec3){a.x-b.x, a.y-b.y, a.z-b.z};
    return v_res;
}

float Vec_DotProduct(vec3 a, vec3 b){
    return (a.x*b.x) + (a.y*b.y) + (a.z*b.z);
}

vec3 Vec_Multiply(vec3 a, float multiplier){
    vec3 v_multiply = (vec3){a.x*multiplier, a.y*multiplier, a.z*multiplier, a.w*multiplier};
    return v_multiply;
}

vec3 Vec_Div(vec3 a, float divisor){
    vec3 v_div = (vec3){a.x/divisor, a.y/divisor, a.z/divisor};
    return v_div;
}

float Vec_Length(vec3 v){
    return sqrtf(Vec_DotProduct(v,v));
}

vec3 Vec_Normalise(vec3 v){
    float l = Vec_Length(v);
    return (vec3){v.x/l, v.y/l, v.z/l};
}


vec3 Vec_CrossProduct(vec3 a, vec3 b){
    vec3 v_new;
    v_new.x = (a.y*b.z) - (a.z*b.y);
    v_new.y = (a.z*b.x) - (a.x*b.z);
    v_new.z = (a.x*b.y) - (a.y*b.x);
    return v_new;
}

vec3 Vec_Perpendicular(vec3 v){
    vec3 v_perp;
    v_perp.x = v.y;
    v_perp.y = -v.x;
    return v_perp;
}

bool IsPointOnRightSideOfLine(vec3 a, vec3 b, vec3 p){
    vec3 ap = Vec_Subtract(p, a);
    vec3 abPerp = Vec_Perpendicular(Vec_Subtract(b, a));
    return Vec_DotProduct(ap, abPerp) >=0;
}

bool IsPointInTriangle(vec3 a, vec3 b, vec3 c, vec3 p){
    bool sideAB = IsPointOnRightSideOfLine(a, b, p);
    bool sideBC = IsPointOnRightSideOfLine(b, c, p);
    bool sideCA = IsPointOnRightSideOfLine(c, a, p);
    return (sideAB == sideBC) && (sideBC == sideCA);
}

int compareMyStructs(const void *a, const void *b) {
    const Triangle *tA = (const Triangle *)a;
    const Triangle *tB = (const Triangle *)b;

    float z1 = (tA->p[0].z + tA->p[1].z + tA->p[2].z)/3;
    float z2 = (tB->p[0].z + tB->p[1].z + tB->p[2].z)/3;
    
    return z1>z2;
    // if(z1<z2){
    //     return -1;
    // } else if(z1>z2){
    //     return 1;
    // } else {
    //     return 0;
    // }
}

Uint32 GetColour(float lum)
	{
		int pixel_bw = (int)(13.0f*lum);   
    Uint32 colorOrange;
		switch (pixel_bw)
		{
		case 0: colorOrange = SDL_MapRGB(global.g_screenSurface->format, 255, 100, 0); break;

		case 1: colorOrange = SDL_MapRGB(global.g_screenSurface->format, 255, 110, 0); break;
		case 2: colorOrange = SDL_MapRGB(global.g_screenSurface->format, 255, 120, 0); break;
		case 3: colorOrange = SDL_MapRGB(global.g_screenSurface->format, 255, 130, 0); break;
		case 4: colorOrange = SDL_MapRGB(global.g_screenSurface->format, 255, 140, 0); break;

		case 5: colorOrange = SDL_MapRGB(global.g_screenSurface->format, 255, 150, 0); break;
		case 6: colorOrange = SDL_MapRGB(global.g_screenSurface->format, 255, 160, 0); break;
		case 7: colorOrange = SDL_MapRGB(global.g_screenSurface->format, 255, 170, 0); break;
		case 8: colorOrange = SDL_MapRGB(global.g_screenSurface->format, 255, 180, 0); break;

		case 9:  colorOrange = SDL_MapRGB(global.g_screenSurface->format, 255, 190, 0); break;
		case 10: colorOrange = SDL_MapRGB(global.g_screenSurface->format, 255, 200, 0); break;
		case 11: colorOrange = SDL_MapRGB(global.g_screenSurface->format, 255, 210, 0); break;
		case 12: colorOrange = SDL_MapRGB(global.g_screenSurface->format, 255, 220, 0); break;
	}
    return colorOrange;
}



