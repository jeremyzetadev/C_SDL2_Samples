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
    Uint32 colorOrange = SDL_MapRGB(global.g_screenSurface->format, 255, 165, 0); 
    float v_x = v_end.x - v_start.x;
    float v_y = v_end.y - v_start.y;
    float hypothenuse = sqrt( (v_x*v_x)+(v_y*v_y) );
    float step_x = v_x/hypothenuse;
    float step_y = v_y/hypothenuse;
    int steps = (int)hypothenuse;
    for(int i=0; i<steps; i++){
        int v_x_trail = v_start.x + (i * step_x);
        int v_y_trail = v_start.y + (i * step_y);
        pixels[v_y_trail*pitch +v_x_trail] = colorOrange;
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
    pixels[y*pitch + x] = col;
}


void Render_Triangle(Triangle t){
    Render_LineScreenSurface(t.p[0], t.p[1]);
    Render_LineScreenSurface(t.p[1], t.p[2]);
    Render_LineScreenSurface(t.p[2], t.p[0]);
}
