
#include "mathlib.h"

#include <stdio.h>
#include <math.h>

void mat4_print(mat4_t M) {
    printf("{{%f, %f, %f, %f}, {%f, %f, %f, %f}, {%f, %f, %f, %f}, {%f, %f, %f, %f}}\n",
        M[0][0],
        M[0][1],
        M[0][2],
        M[0][3],

        M[1][0],
        M[1][1],
        M[1][2],
        M[1][3],

        M[2][0],
        M[2][1],
        M[2][2],
        M[2][3],

        M[3][0],
        M[3][1],
        M[3][2],
        M[3][3]
    );
}

void mat4_perspective(mat4_t M, float fov, float far, float near) {
    float scale = 1 / tan(fov * 0.5 * M_PI / 180);

    M[0][0] = scale;
    M[1][1] = scale;
    M[2][2] = -far / (far - near);
    M[3][2] = -far * near / (far - near);
    M[2][3] = -1;
    M[3][3] = 0;
}


