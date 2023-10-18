
#ifndef SHOOT02_H_
#define SHOOT02_H_

#include "glad/gl.h"
#include <GLFW/glfw3.h>

#ifndef NDEBUG
    #define _debug(x) printf(#x ": %d\n", x)
    #define _debugf(x) printf(#x ": %f\n", x)
    #define _debugs(x) printf(#x ": %s\n", x)
    #define _debugp(x) printf(#x ": %p\n", x)
#else
    #define _debug(x)
    #define _debugf(x)
    #define _debugs(x)
    #define _debugp(x)
#endif

extern GLFWwindow *r_window;

#endif // SHOOT02_H_
