#include <cmath>
void HSVtoRGB(float H, float S,float V,unsigned char *red, unsigned char *green, unsigned char *blue){
               
                *red = 0;
                *green = 0;
                *blue = 0;
				int section = (int)floor(H / 8192);
				int remainder = (int)floor(H) % 8192;
				switch (section)
				{
				case 0:
					// just increasing blue
					*blue = (int)round((remainder * 256) / 8192);
					break;
				case 1:
					// max blue increasing green
					*blue = 255;
					*green = (int)round((remainder * 256) / 8192);
					break;
				case 2:
					// max green decreasing blue
					*green = 255;
					*blue = 255 - (int)round((remainder * 256) / 8192);
					break;
				case 3:
					// max green increasing red
					*green = 255;
					*red = (int)round((remainder * 256) / 8192);
					break;
				case -4:
					// max red and green increasing blue
					*red = 255;
					*green = 255;
					*blue = (int)round((remainder * 256) / 8192);
					break;
				case 4:
					*green = *red = 255;
					break;
				case -3:
					// max red and blue decreasing green
					*red = 255;
					*blue = 255;
					*green = 255 - (int)round((remainder * 256) / 8192);
					break;
				case -2:
					// max red decreasing blue
					*red = 255;
					*blue = 255 - (int)round((remainder * 256) / 8192);
					break;
				case -1:
					// decreasing red
					*red = 255 - (int)round((remainder * 256) / 8192);
					break;
				default:
					std::cerr << "something went wrong calculating the section: " << H << " : " << section << std::endl;
					break;
				}
}