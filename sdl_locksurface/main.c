#include <SDL2/SDL_error.h>
#include <SDL2/SDL_video.h>
#include<stdlib.h>
#include<stdio.h>
#include<SDL2/SDL.h>

#define ERROR_EXIT(...) {fprintf(stderr, __VA_ARGS__); exit(1);}
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

SDL_Window *g_window;
SDL_Surface *g_screenSurface;

enum colorType{
    red,
    blue,
    green
} colorType_t;



void InitializeProgram(){
    if(SDL_Init(SDL_INIT_VIDEO) <0)
        ERROR_EXIT("SDL could not initialized, SDL_Error: %s\n" ,SDL_GetError());

    g_window = SDL_CreateWindow("Draw in Lock Surface", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    if(!g_window)
        ERROR_EXIT("g_window could not be created, SDL_Error: %s\n", SDL_GetError());

    g_screenSurface = SDL_GetWindowSurface(g_window);
    if(!g_screenSurface)
        ERROR_EXIT("Could not get g_window surface, SDL_Error:%s\n", SDL_GetError());

}

void LockScreenSurface(){
    if(SDL_MUSTLOCK(g_screenSurface)){
        if(SDL_LockSurface(g_screenSurface)<0)
            ERROR_EXIT("Could not lock surface, SDL Error:%s\n", SDL_GetError());
    }
}

void UnlockScreenSurface(){
    if(SDL_MUSTLOCK(g_screenSurface)){
        SDL_UnlockSurface(g_screenSurface);
    }
}

// Access and modify pixels directly
// This example sets a pixel at (100, 100) to a color
void EditScreenSurfacePixel(enum colorType cType){
    Uint32* pixels = (Uint32*)g_screenSurface->pixels;
    int pitch = g_screenSurface->pitch / sizeof(Uint32);
    int x = 100;
    int y = 100;
    Uint32 col;
    switch (cType){
        case red:
           col =SDL_MapRGB(g_screenSurface->format, 255, 0, 0);
        case blue:
           col =SDL_MapRGB(g_screenSurface->format, 0, 0, 255);
        case green:
           col =SDL_MapRGB(g_screenSurface->format, 0, 255, 0);
    }
    pixels[y*pitch + x] = col;
}

// Fill the entire screen surface with the color
// Pass nullptr for the rectangle to fill the entire surface
void FillScreen(){
    Uint32 redColor = SDL_MapRGB(g_screenSurface->format, 255, 0, 0);
    SDL_FillRect(g_screenSurface, NULL, redColor);
}

void EditScreenSurfacePixels(enum colorType cType){
    SDL_PixelFormat* format = g_screenSurface->format;
    Uint32* pixels = (Uint32*)g_screenSurface->pixels;
    int width = g_screenSurface->w;
    int height = g_screenSurface->h;
    int pitch = g_screenSurface->pitch / format->BytesPerPixel; // Pitch in pixels
    Uint32 col;
    switch (cType){
        case red:
           col =SDL_MapRGB(g_screenSurface->format, 255, 0, 0);
           break;
        case blue:
           col =SDL_MapRGB(g_screenSurface->format, 0, 0, 255);
           break;
        case green:
           col =SDL_MapRGB(g_screenSurface->format, 0, 255, 0);
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

int main(){
    InitializeProgram();

    LockScreenSurface();

    EditScreenSurfacePixels(red);
    SDL_UpdateWindowSurface(g_window);
    SDL_Delay(700);

    EditScreenSurfacePixels(blue);
    SDL_UpdateWindowSurface(g_window);
    SDL_Delay(700);

    EditScreenSurfacePixels(green);
    SDL_UpdateWindowSurface(g_window);
    SDL_Delay(700);

    UnlockScreenSurface();
    SDL_Delay(3000);
    SDL_DestroyWindow(g_window);
    SDL_Quit();

    return 0;
}

