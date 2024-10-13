#include "tigr.h"

Tigr* img_flip_horizontal(Tigr* image) {
    Tigr* flipped = tigrBitmap(image->w, image->h);
    for (int y = 0; y < image->h; y++) {
        for (int x = 0; x < (image->w / 2) + 1; x++) {
            TPixel temp = tigrGet(image, x, y);
            tigrPlot(flipped, x, y, tigrGet(image, image->w - 1 - x, y));
            tigrPlot(flipped, image->w - 1 - x, y, temp);
        }
    }
    return flipped;
}

Tigr*  img_flip_vertical(Tigr* image) {
    Tigr* flipped = tigrBitmap(image->w, image->h);
    for (int y = 0; y < (image->h / 2) + 1; y++) {
        for (int x = 0; x < image->w; x++) {
            TPixel temp = tigrGet(image, x, y);
            tigrPlot(flipped, x, y, tigrGet(image, x, image->h - 1 - y));
            tigrPlot(flipped , x, image->h - 1 - y, temp);
        }
    }
    return flipped;
}

Tigr* img_rotate_180(Tigr* image) {
    
    Tigr* rotated = tigrBitmap(image->w, image->h);

    for (int y = 0; y < image->h / 2; y++) {
        for (int x = 0; x < image->w; x++) {
            /* Swap pixel (x, y) with (image->w - 1 - x, image->h - 1 - y) */
            TPixel temp = tigrGet(image, x, y);
            tigrPlot(rotated, x, y, tigrGet(image, image->w - 1 - x, image->h - 1 - y));
            tigrPlot(rotated, image->w - 1 - x, image->h - 1 - y, temp);
        }
    }
    
    /* If the height is odd, handle the middle row separately */
    if (image->h % 2 != 0) {
        int middleY = image->h / 2;
        for (int x = 0; x < image->w / 2; x++) {
            /* Swap the middle row pixels */
            TPixel temp = tigrGet(image, x, middleY);
            tigrPlot(rotated, x, middleY, tigrGet(image, image->w - 1 - x, middleY));
            tigrPlot(rotated, image->w - 1 - x, middleY, temp);
        }
    }

    return rotated;
}

int abs_mod(int a,int b){
    return (a+b) % b;
}