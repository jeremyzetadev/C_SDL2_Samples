#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include "math_util.h"
#include "render.h"
#include "global.h"
#include <math.h>

void Render_LineScreenSurface(vec3 v_start, vec3 v_end){
    SDL_PixelFormat* format = global.g_screenSurface->format;
    Uint32* pixels = (Uint32*)global.g_screenSurface->pixels;
    int width = global.g_screenSurface->w;
    int height = global.g_screenSurface->h;
    int pitch = global.g_screenSurface->pitch / format->BytesPerPixel; // Pitch in pixels
    Uint32 colorBlack = SDL_MapRGB(global.g_screenSurface->format, 0, 0, 0); 
    float v_x = v_end.x - v_start.x;
    float v_y = v_end.y - v_start.y;
    float hypothenuse = sqrt( (v_x*v_x)+(v_y*v_y) );
    float step_x = v_x/hypothenuse;
    float step_y = v_y/hypothenuse;
    int steps = (int)hypothenuse;
    for(int i=0; i<steps; i++){
        int v_x_trail = v_start.x + (i * step_x);
        int v_y_trail = v_start.y + (i * step_y);
        if((v_y_trail*pitch + v_x_trail) >=0 && (v_y_trail*pitch + v_x_trail < width*height)){
            pixels[v_y_trail*pitch +v_x_trail] = colorBlack;
        }
    }
}

// Fill the entire screen surface with the color
// Pass nullptr for the rectangle to fill the entire surface
void Render_FillScreenBlue(){
    Uint32 blueColor = SDL_MapRGB(global.g_screenSurface->format, 0, 0, 255);
    SDL_FillRect(global.g_screenSurface, NULL, blueColor);
}

void Render_EditScreenSurfacePixels(enum colorType cType){
    SDL_PixelFormat* format = global.g_screenSurface->format;
    Uint32* pixels = (Uint32*)global.g_screenSurface->pixels;
    int width = global.g_screenSurface->w;
    int height = global.g_screenSurface->h;
    int pitch = global.g_screenSurface->pitch / format->BytesPerPixel; // Pitch in pixels
    Uint32 col;
    switch (cType){
        case red:
           col =SDL_MapRGB(global.g_screenSurface->format, 255, 0, 0);
           break;
        case blue:
           col =SDL_MapRGB(global.g_screenSurface->format, 0, 0, 255);
           break;
        case green:
           col =SDL_MapRGB(global.g_screenSurface->format, 0, 255, 0);
           break;
        default:
           break;
    }

    // Loop through pixels
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Uint8 r = (Uint8)(x * 255 / width);
            // Uint8 g = (Uint8)(y * 255 / height);
            // Uint8 b = (Uint8)((x + y) * 255 / (width + height));
            // pixels[y * pitch + x] = SDL_MapRGB(format, r, g, b);
            pixels[y*pitch +x] = col;
        }
    }
}

// Access and modify pixels directly
// This example sets a pixel at (100, 100) to a color
void Render_EditScreenSurfacePixel(enum colorType cType){
    Uint32* pixels = (Uint32*)global.g_screenSurface->pixels;
    int pitch = global.g_screenSurface->pitch / sizeof(Uint32);
    int x = 100;
    int y = 100;
    Uint32 col;
    switch (cType){
        case red:
           col =SDL_MapRGB(global.g_screenSurface->format, 255, 0, 0);
        case blue:
           col =SDL_MapRGB(global.g_screenSurface->format, 0, 0, 255);
        case green:
           col =SDL_MapRGB(global.g_screenSurface->format, 0, 255, 0);
    }
    if((y*pitch + x) >=0){
        pixels[y*pitch + x] = col;
    }
}


void Render_TriangleLines(Triangle t){
    Render_LineScreenSurface(t.p[0], t.p[1]);
    Render_LineScreenSurface(t.p[1], t.p[2]);
    Render_LineScreenSurface(t.p[2], t.p[0]);
}

void Render_TriangleFill_Old(Triangle t){
    SDL_PixelFormat* format = global.g_screenSurface->format;
    Uint32* pixels = (Uint32*)global.g_screenSurface->pixels;
    int width = global.g_screenSurface->w;
    int height = global.g_screenSurface->h;
    int pitch = global.g_screenSurface->pitch / format->BytesPerPixel; // Pitch in pixels
    Uint32 colorOrange = SDL_MapRGB(global.g_screenSurface->format, 255, 165, 0); 
    // Loop through pixels
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            vec3 v_pixel;
            v_pixel.x = x; v_pixel.y = y;
            if(IsPointInTriangle(t.p[0], t.p[1], t.p[2], v_pixel))
                pixels[y*pitch +x] = colorOrange;
        }
    }
}

void Render_TriangleFill(Triangle t){
    SDL_PixelFormat* format = global.g_screenSurface->format;
    Uint32* pixels = (Uint32*)global.g_screenSurface->pixels;
    int width = global.g_screenSurface->w;
    int height = global.g_screenSurface->h;
    int pitch = global.g_screenSurface->pitch / format->BytesPerPixel; // Pitch in pixels
    Uint32 colorOrange = SDL_MapRGB(global.g_screenSurface->format, 255, 165, 0); 
    
    float minX = Min(Min(t.p[0].x, t.p[1].x), t.p[2].x);
    float minY = Min(Min(t.p[0].y, t.p[1].y), t.p[2].y);
    float maxX = Max(Max(t.p[0].x, t.p[1].x), t.p[2].x);
    float maxY = Max(Max(t.p[0].y, t.p[1].y), t.p[2].y);

    //Guard Clause for minmax use screen as bounds
    // if (minX<0) minX=0;
    // if (minY<0) minY=0;
    // if (maxX<0) maxX=width;
    // if (maxY<0) maxY=height;
    // if (minX>width) minX=0;
    // if (minY>height) minY=0;
    // if (maxX>width) maxX=width;
    // if (maxY>height) maxY=height;
    //Guard Clause for minmax use screen as bounds

    int blockStartX = Clamp((int)minX, 0, width-1);
    int blockStartY = Clamp((int)minY, 0, height-1);
    int blockEndX = Clamp((int)maxX, 0, width-1);
    int blockEndY = Clamp((int)maxY, 0, height-1);

    // Loop through bound only of triangle and draw pixel
    // for (int y = blockStartY; y < blockEndY; y++) {
    //     for (int x = blockStartX; x < blockEndX; x++) {
    //         vec3 v_pixel;
    //         v_pixel.x = x; v_pixel.y = y;
    //         if(IsPointInTriangle(t.p[0], t.p[1], t.p[2], v_pixel)){
    //             pixels[y*pitch +x] = colorOrange;
    //             continue;
    //         }
    //     }
    // }

    //Loop through bound only of triangle and draw pixel
    for (int y = minY; y < maxY; ++y) {
        for (int x = minX; x < maxX; ++x) {
            vec3 v_pixel;
            v_pixel.x = x; v_pixel.y = y;
            if((y*pitch +x)>0 && ((y*pitch +x)<width*height)){
                if(IsPointInTriangle(t.p[0], t.p[1], t.p[2], v_pixel)){
                    // pixels[y*pitch +x] = colorOrange;
                    pixels[y*pitch +x] = t.color;
                }
                continue;
            }
        }
    }

    // //OLD
    // for (int y = 0; y < height; ++y) {
    //     for (int x = 0; x < width; ++x) {
    //         vec3 v_pixel;
    //         v_pixel.x = x; v_pixel.y = y;
    //         if(IsPointInTriangle(t.p[0], t.p[1], t.p[2], v_pixel))
    //             if((y*pitch +x)>0){
    //                 pixels[y*pitch +x] = colorOrange;
    //             }
    //     }
    // }
}

void swap_int(int *a, int *b) {
    int temp; 

    temp = *a; 
    *a = *b;  
    *b = temp; 
}

void swap(void *a, void *b) {
    void *temp; 
    temp = a; 
    a = b;  
    b = temp; 
}

void DrawHorizontalLine(int x_start, int x_end, int y, size_t color) {
    Uint32* pixels = (Uint32*)global.g_screenSurface->pixels;
    SDL_PixelFormat* format = global.g_screenSurface->format;
    int pitch = global.g_screenSurface->pitch / format->BytesPerPixel; // Pitch in pixels
    int x;

    // 1. Ensure x_start is less than or equal to x_end.
    // This makes sure the loop always iterates correctly from left to right.
    if (x_start > x_end && x_end < x_start) {
        int temp = x_start;
        x_start = x_end;
        x_end = temp;
    }

    // 2. Iterate and draw each pixel.
    for (x = x_start; x <= x_end; x++) {
        pixels[y*pitch +x] = color;
    }
}

void FillFlatBottomTriangle(vec3 pTop, vec3 pL, vec3 pR, size_t color) {
    // Ensure pL is the true left point (smaller X)
    if (pL.x > pR.x) {
        vec3 temp = pL;
        pL = pR;
        pR = temp;
    }

    // 1. Calculate the Y-range
    int y_start = pTop.y;
    int y_end = pL.y; // Same as pR.y

    // Check for division by zero (degenerate triangle)
    if (y_start == y_end) {
        return;
    }

    // 2. Calculate Inverse Slopes (dx/dy)
    // Left edge (pTop to pL)
    float m_inv_L = (float)(pL.x - pTop.x) / (pL.y - pTop.y);

    // Right edge (pTop to pR)
    float m_inv_R = (float)(pR.x - pTop.x) / (pR.y - pTop.y);

    // 3. Initialize current X-coordinates
    // We start at the top vertex X, casting to float for accurate interpolation.
    float current_x_L = (float)pTop.x;
    float current_x_R = (float)pTop.x;

    // 4. Scanline Loop
    // Iterate from the Y-coordinate below the top vertex up to the base Y.
    // We use <= y_end for the loop to include the final flat base.
    for (int y = y_start; y <= y_end; y++) {
        // The current scanline spans from current_x_L to current_x_R.
        // We cast to int/floor to get the pixel index.
        int x_start = (int)(current_x_L);
        int x_end = (int)(current_x_R);
        
        // Always draw from smaller X to larger X
        // We use a helper function that swaps if needed.
        DrawHorizontalLine(x_start, x_end, y, color);

        // Update X-coordinates for the next scanline (y+1)
        current_x_L += m_inv_L;
        current_x_R += m_inv_R;
    }
}

void FillFlatTopTriangle(vec3 pTopL, vec3 pTopR, vec3 pBottom, size_t color) {
    // 1. Ensure pTopL is the true left point (smaller X)
    if (pTopL.x > pTopR.x) {
        vec3 temp = pTopL;
        pTopL = pTopR;
        pTopR = temp;
    }

    // 2. Calculate the Y-range
    int y_start = pTopL.y; // Same as pTopR.y
    int y_end = pBottom.y;

    // Check for division by zero (degenerate triangle)
    if (y_start == y_end) {
        return;
    }

    // 3. Calculate Inverse Slopes (dx/dy)
    // Left edge (pTopL to pBottom)
    float m_inv_L = (float)(pBottom.x - pTopL.x) / (pBottom.y - pTopL.y);

    // Right edge (pTopR to pBottom)
    float m_inv_R = (float)(pBottom.x - pTopR.x) / (pBottom.y - pTopR.y);

    // 4. Initialize current X-coordinates
    // Start at the left and right endpoints of the top flat base.
    float current_x_L = (float)pTopL.x;
    float current_x_R = (float)pTopR.x;

    // 5. Scanline Loop
    // Iterate from the Y-coordinate of the flat base down to the bottom vertex Y.
    for (int y = y_start; y <= y_end; y++) {
        // The current scanline spans from current_x_L to current_x_R.
        int x_start = (int)(current_x_L);
        int x_end = (int)(current_x_R);

        // Draw the horizontal line for the current scanline.
        // DrawHorizontalLine handles drawing from min(x_start, x_end) to max(x_start, x_end).
        DrawHorizontalLine(x_start, x_end, y, color);

        // Update X-coordinates for the next scanline (y+1)
        current_x_L += m_inv_L;
        current_x_R += m_inv_R;
    }
}

void Render_TriangleFill_ScanLine(Triangle t){
    vec3 p1 = t.p[0];
    vec3 p2 = t.p[1];
    vec3 p3 = t.p[2];
    if (p1.y > p2.y) swap(&p1, &p2);
    if (p1.y > p3.y) swap(&p1, &p3);
    if (p2.y > p3.y) swap(&p2, &p3);
    vec3 pTop = p1, pMid = p2, pBottom = p3;
    
    // Handle degenerate cases (e.g., area == 0 or horizontal line)
    if (pTop.y == pBottom.y) return;

    // 2. Find P4 on the long edge (pTop-pBottom) at y = pMid.y
    float x_p4;
    // Avoid division by zero for vertical line, though sorting by Y helps.
    if (pBottom.y - pTop.y == 0) return;

    x_p4 = pTop.x + (pBottom.x - pTop.x) * (pMid.y - pTop.y) / (pBottom.y - pTop.y);
    vec3 p4 = { (int)x_p4, pMid.y };

    // 3. Draw the two sub-triangles

    // Top part: Flat-Bottom Triangle (pTop, pMid, p4)
    FillFlatBottomTriangle(pTop, pMid, p4, t.color);

    // Bottom part: Flat-Top Triangle (pMid, p4, pBottom)
    FillFlatTopTriangle(pMid, p4, pBottom, t.color);
}

void Render_TriangleFill_ScanLine2(Triangle t)
{
    int x1 = t.p[0].x;
    int x2 = t.p[1].x;
    int x3 = t.p[2].x;
    int y1 = t.p[0].y;
    int y2 = t.p[1].y;
    int y3 = t.p[2].y;
    int t1x, t2x, y, minx, maxx, t1xp, t2xp;
    bool changed1 = false;
    bool changed2 = false;
    int signx1, signx2, dx1, dy1, dx2, dy2;
    int e1, e2;
    // Sort vertices
    if (y1>y2) { swap_int(&y1, &y2); swap_int(&x1, &x2); }
    if (y1>y3) { swap_int(&y1, &y3); swap_int(&x1, &x3); }
    if (y2>y3) { swap_int(&y2, &y3); swap_int(&x2, &x3); }

    t1x = t2x = x1; y = y1;   // Starting points
    dx1 = (int)(x2 - x1); 
    if (dx1<0) { dx1 = -dx1; signx1 = -1; }
    else signx1 = 1;
    dy1 = (int)(y2 - y1);

    dx2 = (int)(x3 - x1); 
    if (dx2<0) { dx2 = -dx2; signx2 = -1; }
    else signx2 = 1;
    dy2 = (int)(y3 - y1);

    if (dy1 > dx1) {   // swap_int values
        swap_int(&dx1, &dy1);
        changed1 = true;
    }
    if (dy2 > dx2) {   // swap_int values
        swap_int(&dy2, &dx2);
        changed2 = true;
    }

    e2 = (int)(dx2 >> 1);
    // Flat top, just process the second half
    if (y1 == y2) goto next;
    e1 = (int)(dx1 >> 1);

    for (int i = 0; i < dx1;) {
        t1xp = 0; t2xp = 0;
        if (t1x<t2x) { minx = t1x; maxx = t2x; }
        else { minx = t2x; maxx = t1x; }
        // process first line until y value is about to change
        while (i<dx1) {
            i++;
            e1 += dy1;
            while (e1 >= dx1) {
                e1 -= dx1;
                if (changed1) t1xp = signx1;//t1x += signx1;
                else          goto next1;
            }
            if (changed1) break;
            else t1x += signx1;
        }
        // Move line
next1:
        // process second line until y value is about to change
        while (1) {
            e2 += dy2;
            while (e2 >= dx2) {
                e2 -= dx2;
                if (changed2) t2xp = signx2;//t2x += signx2;
                else          goto next2;
            }
            if (changed2)     break;
            else              t2x += signx2;
        }
next2:
        if (minx>t1x) minx = t1x; if (minx>t2x) minx = t2x;
        if (maxx<t1x) maxx = t1x; if (maxx<t2x) maxx = t2x;
        DrawHorizontalLine(minx, maxx, y, t.color);    // Draw line from min to max points found on the y
                                    // Now increase y
        if (!changed1) t1x += signx1;
        t1x += t1xp;
        if (!changed2) t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y == y2) break;

    }
next:
    // Second half
    dx1 = (int)(x3 - x2); 
    if (dx1<0) { dx1 = -dx1; signx1 = -1; }
    else signx1 = 1;
    dy1 = (int)(y3 - y2);
    t1x = x2;

    if (dy1 > dx1) {   // swap_int values
        swap_int(&dy1, &dx1);
        changed1 = true;
    }
    else changed1 = false;

    e1 = (int)(dx1 >> 1);

    for (int i = 0; i <= dx1; i++) {
        t1xp = 0; t2xp = 0;
        if (t1x<t2x) { minx = t1x; maxx = t2x; }
        else { minx = t2x; maxx = t1x; }
        // process first line until y value is about to change
        while (i<dx1) {
            e1 += dy1;
            while (e1 >= dx1) {
                e1 -= dx1;
                if (changed1) { t1xp = signx1; break; }//t1x += signx1;
                else          goto next3;
            }
            if (changed1) break;
            else   	   	  t1x += signx1;
            if (i<dx1) i++;
        }
next3:
        // process second line until y value is about to change
        while (t2x != x3) {
            e2 += dy2;
            while (e2 >= dx2) {
                e2 -= dx2;
                if (changed2) t2xp = signx2;
                else          goto next4;
            }
            if (changed2)     break;
            else              t2x += signx2;
        }
next4:

        if (minx>t1x) minx = t1x; if (minx>t2x) minx = t2x;
        if (maxx<t1x) maxx = t1x; if (maxx<t2x) maxx = t2x;
        DrawHorizontalLine(minx, maxx, y, t.color);   										
        if (!changed1) t1x += signx1;
        t1x += t1xp;
        if (!changed2) t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y>y3) return;
    }
}
