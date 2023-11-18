#ifndef COLOR_H
#define COLOR_H


#include <cmath>
int map(int num, int in_min, int in_max, int out_min, int out_max){
    return (int)(num - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Takes in a value from 0 -> 65536(2^16) and gives back the rgb to draw
void HSVtoRGB(int H,unsigned char *red, unsigned char *green, unsigned char *blue){
    
				// Prevent bad values
				H = H % 65537;
				H = map(H,0,65536,-32768,32768);

				int section = (int)std::floor(H / 8192);
				// Pretty Jank but provents the only 4 section 'clips'
				if((H >= -24607 && H <= -24607+31) ||
				   (H >= -16415 && H <= -16415+31) ||
				   (H >= -8223 && H <= -8223+31)   ||
				   (H >= -31 && H <= -31+31)){
					return;
				}

                *red = 0;
                *green = 0;
                *blue = 0; 
            
				int remainder = (int)std::floor(H) % 8192;
				switch (section)
				{
				case 0:
					// just increasing blue
					*blue = (int)std::round((remainder * 256) / 8192);
					break;
				case 1:
					// max blue increasing green
					*blue = 255;
					*green = (int)std::round((remainder * 256) / 8192);
					break;
				case 2:
					// max green decreasing blue
					*green = 255;
					*blue = 255 - (int)std::round((remainder * 256) / 8192);
					break;
				case 3:
					// max green increasing red
					*green = 255;
					*red = (int)std::round((remainder * 256) / 8192);
					break;
				case -4:
					// max red and green increasing blue
					*red = 255;
					*green = 255;
					*blue = (int)std::round((remainder * 256) / 8192);
					break;
				case 4:
					*green = *red = 255;
					break;
				case -3:
					// max red and blue decreasing green
					*red = 255;
					*blue = 255;
					*green = 255 - (int)std::round((remainder * 256) / 8192);
					break;
				case -2:
					// max red decreasing blue
					*red = 255;
					*blue = 255 - (int)std::round((remainder * 256) / 8192);
					break;
				case -1:
					// decreasing red
					*red = 255 - (int)std::round((remainder * 256) / 8192);
					break;
				default:
                    // Something went wrong ...
					break;
				}
}


#endif