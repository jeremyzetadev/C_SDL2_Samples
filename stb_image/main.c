#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

// build by : gcc -std=c17 -Wall -pedantic main.c -o mainrun -lm
int main(void){
    int width, height, channels;
    unsigned char *img = stbi_load("sky.jpg", &width, &height, &channels, 0);
    if(img==NULL){
        printf("Error in loading the image\n");
        exit(1);
    }
    printf("sky.jpg Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, channels);
    stbi_write_png("skyTEST.png", width, height, channels, img, width*channels);



    unsigned char *img2 = stbi_load("shapes.png", &width, &height, &channels, 0);
    if(img==NULL){
        printf("Error in loading the image\n");
        exit(1);
    }
    printf("shapes.png loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, channels);
    stbi_write_jpg("shapesTEST.jpg", width, height, channels, img2, 100);
    //
    //
    //
    stbi_image_free(img);
    stbi_image_free(img2);
}


