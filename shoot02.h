
#ifndef SHOOT02_H_
#define SHOOT02_H_

#include "glad/gl.h"
#include <GLFW/glfw3.h>

#ifndef NDEBUG
    #define _debug(x) printf(#x ": %d\n", x)
#else
    #define _debug(x)
#endif

extern GLFWwindow *r_window;

#endif // SHOOT02_H_
