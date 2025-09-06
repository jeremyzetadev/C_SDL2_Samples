#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

int main(){
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("Test Time", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_Texture *texture = IMG_LoadTexture(renderer, "earth.png"); //C-logo.png
    SDL_Rect sprite_rect = {0, 0, 0, 0};
    SDL_QueryTexture(texture, NULL, NULL, &sprite_rect.w, &sprite_rect.h);

    SDL_Rect clip;
    clip.x = 0;
    clip.y = 0;
    clip.w = 1;
    clip.h = 1;
    printf("By screen of 1920x1080=2_073_600\n");

    int msec, i, cnt_DrawPoint;
    clock_t start, diff;

    i = 100000;
    start = clock();
    while(i-- > 0)
    {
        SDL_RenderDrawPoint(renderer, 0, 0);
        cnt_DrawPoint++;
    }
    diff = clock() - start;
    msec = diff * 1000 / CLOCKS_PER_SEC;
    fprintf(stdout, "SDL_RenderDrawPoints: Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);


    i = 100000;
    start = clock();
        while(i-- > 0)
        {
            // SDL_RenderCopy(renderer, texture, &clip, &clip);
            SDL_RenderCopy(renderer, texture, NULL, &sprite_rect);
        }
    diff = clock() - start;
    msec = diff * 1000 / CLOCKS_PER_SEC;
    fprintf(stdout, "SDL_RenderCopys 1920x1080png: Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);

    //SDL_LockSurface is fastest by directly accessing pixels;
    //* Get a 640x480, 24-bit software screen surface */
    // scr = SDL_SetVideoMode(640, 480, 24, SDL_SWSURFACE);
    // assert(scr);
    //
    // /* Ensures we have exclusive access to the pixels */
    // SDL_LockSurface(scr);
    //
    // for(y = 0; y < scr->h; y++)
    //     for(x = 0; x < scr->w; x++)
    //     {
    //             /* This is what generates the pattern based on the xy co-ord */
    //             t = ((x*x + y*y) & 511) - 256;
    //             if (t < 0)
    //                     t = -(t + 1);
    //
    //             /* Now we write to the surface */
    //             pel(scr, x, y, 0) = 255 - t; //red
    //             pel(scr, x, y, 1) = t; //green
    //             pel(scr, x, y, 2) = t; //blue
    //     }
    // SDL_UnlockSurface(scr);
    // SDL_UpdateRect(scr, 0, 0, 0, 0);

    // Uint32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    // i = 100000;
    // start = clock();
    //     while(i-- > 0)
    //     {
    //         SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(Uint32)); 
    //     }
    // diff = clock() - start;
    // msec = diff * 1000 / CLOCKS_PER_SEC;
    // fprintf(stdout, "SDL_UpdateTexture: Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);
}
