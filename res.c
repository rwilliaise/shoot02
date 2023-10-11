
#include "res.h"

#include <stdio.h>
#include <stdlib.h>

char *res_from_path(const char *path) {
    FILE *file = fopen(path, "r");
    if (file == NULL)
        return NULL;
    
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *out = malloc(size);
    if (out == NULL) {
        fclose(file);
        return NULL;
    }
    fread(out, 1, size, file);
    fclose(file);
    return out;
}

