#pragma once

#include<stdio.h>
#include"math_util.h"
#include"geometry_util.h"

extern enum colorType{
    red,
    blue,
    green
} colorType_t;

void Render_FillScreenBlue();
void Render_LineScreenSurface(vec3 v_start, vec3 v_end);
void Render_EditScreenSurfacePixel(enum colorType cType);
void Render_EditScreenSurfacePixels(enum colorType cType);

void Render_TriangleLines(const Triangle t);
void Render_TriangleFill(Triangle t);
void Render_TriangleFill_Old(Triangle t);
void Render_TriangleFill_ScanLine(Triangle t);
void Render_TriangleFill_ScanLine2(const Triangle t);
