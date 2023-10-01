/*
 * Simple-color-conversions
 *
 * Copyright 2016 Jake Besworth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/*  
 * color.h
 *
 * Portable, simple, compact, color conversion library between color spaces, most notibly RGB -> CYM(K) and HSV
 * Formulas derived from "Color Space Conversions" by Adrian Ford and Alan Roberts.
 *
 * Note: RGB values are 0-255 (unsigned char)
 *       CMY(K) values are 0.00-1.00 (0 to 100%) (float)
 *       HSV values are Hue: 0-360 (0 to 360 degrees) (float), Saturation: 0.00-1.00 (0 to 100%) (float), and Value: 0.00-1.00 (0 to 100%) (float)
 *
 */

#ifndef COLOR_ 
#define COLOR_

#ifdef __cplusplus
extern "C"
{
#endif

void convertHSVtoRGB(const int h, const int s, const int v, unsigned char * r, unsigned char * g, unsigned char * b)
{
    unsigned char region, remainder, p, q, t;
    
    if (s == 0)
    {
        *r = v;
        *g = v;
        *b = v;
    }
    
    region = h / 43;
    remainder = (h - (region * 43)) * 6; 
    
    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (unsigned char)(v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;
    
    switch (region)
    {
        case 0:
            *r = v; *g = t; *b = p;
            break;
        case 1:
            *r = q; *g = v; *b = p;
            break;
        case 2:
            *r = p; *g = v; *b = t;
            break;
        case 3:
            *r = p; *g = q; *b = v;
            break;
        case 4:
            *r = t; *g = p; *b = v;
            break;
        default:
            *r = v; *g = p; *b = q;
            break;
    }
}
#ifdef __cplusplus
}
#endif

#endif
