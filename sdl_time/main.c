#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main(){
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("Test Time", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture *texture = IMG_LoadTexture(renderer, "C-logo.png");
    SDL_Rect clip;
    clip.x = 0;
    clip.y = 0;
    clip.w = 1;
    clip.h = 1;
    printf("By screen of 800x600=480_000\n");

    int msec, i;
    clock_t start, diff;

    i = 100000;
    start = clock();
    while(i-- > 0)
    {
        SDL_RenderDrawPoint(renderer, 0, 0);
    }
    diff = clock() - start;
    msec = diff * 1000 / CLOCKS_PER_SEC;
    fprintf(stdout, "SDL_RenderDrawPoints: Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);


    i = 100000;
    start = clock();
        while(i-- > 0)
        {
            SDL_RenderCopy(renderer, texture, &clip, &clip);
        }
    diff = clock() - start;
    msec = diff * 1000 / CLOCKS_PER_SEC;
    fprintf(stdout, "SDL_RenderCopys: Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);


    Uint32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    i = 100000;
    start = clock();
        while(i-- > 0)
        {
            SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(Uint32)); 
        }
    diff = clock() - start;
    msec = diff * 1000 / CLOCKS_PER_SEC;
    fprintf(stdout, "SDL_UpdateTexture: Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);
}
