
#include "r_map.h"

#include "libmap/geo_generator.h"
#include "libmap/map_data.h"
#include "libmap/map_parser.h"
#include "libmap/surface_gatherer.h"
#include "r_res.h"

#include <string.h>

static const char prefix_res_textures[] = "res/textures/";
static const char suffix_png[] = ".png";

r_map_t *r_map_load(const char *path) {
    map_parser_load(path);
    geo_generator_run();

    r_map_t *M = malloc(sizeof(r_map_t) + sizeof(r_surface_t) * map_data_get_texture_count());
    M->surface_count = map_data_get_texture_count();

    for (int i = 0; i < map_data_get_texture_count(); i++) {
        r_surface_t *S = &M->surfaces[i];
        const texture_data *texture = map_data_get_texture(i);
        printf("%s\n", texture->name);

        char *full_path = malloc(sizeof(prefix_res_textures) + sizeof(suffix_png) + strlen(texture->name) + 1);
        if (full_path == NULL) continue;

        *full_path = 0;
        strcat(full_path, prefix_res_textures);
        strcat(full_path, texture->name);
        strcat(full_path, suffix_png);

        r_texture_t *rc_texture = r_res_texture_from_name(full_path);
        S->texture = rc_texture;

        surface_gatherer_reset_params();
        surface_gatherer_set_split_type(SST_BRUSH);
        surface_gatherer_set_texture_filter(texture->name);
        surface_gatherer_set_brush_filter_texture("dev/clip");
        surface_gatherer_set_face_filter_texture("dev/skip");
        surface_gatherer_set_worldspawn_layer_filter(1);

        surface_gatherer_run();
        const surfaces *surfaces = surface_gatherer_fetch();
        _debug(surfaces->surface_count);
        _debug(surfaces->surfaces[0].vertex_count);

        S->meshes = malloc(sizeof(r_mesh_t) * surfaces->surface_count);
        if (S->meshes == NULL) continue;

        for (int s = 0; s < surfaces->surface_count; s++) {
            surface *current_surface = &surfaces->surfaces[s];
            r_mesh_t *mesh = &S->meshes[s];
            glGenVertexArrays(1, &mesh->vao);

        }
    }
    return NULL;
}
