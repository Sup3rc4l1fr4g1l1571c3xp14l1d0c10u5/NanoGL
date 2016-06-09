#ifndef _NanoGL_MATH_H
#define _NanoGL_MATH_H

#ifdef _WIN32
# define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <stdlib.h>

#ifndef min
#define min(x,y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef max
#define max(x,y) (((x) > (y)) ? (x) : (y))
#endif

#endif

