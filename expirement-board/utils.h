#ifndef __UTILS_H__
#define __UTILS_H__

#include "tigr.h"

Tigr* img_copy(Tigr*);
Tigr* img_flip_horizontal(Tigr*);
Tigr* img_flip_vertical(Tigr*);
Tigr* img_rotate_180(Tigr*);
int abs_mod(int,int);

#endif  /* __UTILS_H__ */ 