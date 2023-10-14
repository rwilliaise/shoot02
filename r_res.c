
#include "r_res.h"
#include "stb_image.h"

#include <stdlib.h>
#include <string.h>

r_texture_t textures[R_RES_TEXTURES_MAX];

r_texture_t *r_res_texture_lookup(const char *name) {
    for (int i = 0; i < R_RES_TEXTURES_MAX; i++) {
        r_texture_t *texture = &textures[i];
        if (texture->name != NULL && strcmp(name, texture->name) == 0)
            return r_res_texture_ref(texture);
    }
    return NULL;
}

r_texture_t *r_res_texture_from_name(const char *name) {
    r_texture_t *out = r_res_texture_lookup(name);
    if (out != NULL)
        return out;

    for (int i = 0; i < R_RES_TEXTURES_MAX; i++) {
        if (textures[i].rc == 0) {
            out = &textures[i];
            break;
        }
    }

    if (out == NULL) return NULL;

    int w, h, channels;
    unsigned char *data = stbi_load(name, &w, &h, &channels, 0);

    if (data == NULL) return NULL;

    _debug(channels);

    glGenTextures(1, &out->id);
    r_res_texture_bind(out);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    out->name = strdup(name);
    return r_res_texture_ref(out);
}

r_texture_t *r_res_texture_ref(r_texture_t *T) {
    T->rc++;
    return T;
}

void r_res_texture_bind(r_texture_t *T) {
    glBindTexture(GL_TEXTURE_2D, T->id);
}

void r_res_texture_unref(r_texture_t *T) {
    T->rc--;
    if (T->rc <= 0) {
        glDeleteTextures(1, &T->id);
        free(T->name);

        T->id = 0;
        T->rc = 0;
        T->name = NULL;
    }
}

