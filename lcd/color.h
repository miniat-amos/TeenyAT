#ifndef __COLOR_H__
#define __COLOR_H__

#define COLORS_IN_PALLETE 4096 // 2^12
#define COLORS_PER_SECTION 512 // 2^12 / 8 since there are 8 sections
#define RGB_RESOLUTION 256		 // 8 bit colors
#define RGB_MAX_VALUE (RGB_RESOLUTION - 1)

// Takes in a value from 0 -> 65536(2^16) and gives back the rgb to draw
void HSVtoRGB(int H, unsigned char *red, unsigned char *green, unsigned char *blue);

#endif  /* __COLOR_H__ */