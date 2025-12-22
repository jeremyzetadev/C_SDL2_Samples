#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

int main() {
    int w, h, channels;
    unsigned char *img = stbi_load("shapesREDtext.png", &w, &h, &channels, 0);
    if (!img) { /* Error handling */ return 1; }

    // Target color (e.g., pure red) to replace
    unsigned char targetR = 255, targetG = 0, targetB = 0;
    // New color (e.g., pure blue)
    unsigned char newR = 0, newG = 0, newB = 255;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int i = (y * w + x) * channels;
            if (img[i] == targetR && img[i+1] == targetG && img[i+2] == targetB) {
                img[i] = newR;       // Set Red to 0
                img[i+1] = newG;     // Set Green to 0
                img[i+2] = newB;     // Set Blue to 255
            }
        }
    }

    stbi_write_png("shapesREDTEXT_toblue.png", w, h, channels, img, w * channels);
    stbi_image_free(img);
    return 0;
}
