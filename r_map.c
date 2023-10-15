
#include "r_map.h"

#include "glad/gl.h"
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

    r_map_t *M = malloc(sizeof(r_map_t) + sizeof(r_surface_t) * map_data_get_texture_count());
    if (M == NULL) return NULL;
    M->surface_count = map_data_get_texture_count();

    for (int i = 0; i < map_data_get_texture_count(); i++) {
        r_surface_t *S = &M->surfaces[i];
        texture_data *texture = map_data_get_texture(i);

        char *full_path = malloc(sizeof(prefix_res_textures) + sizeof(suffix_png) + strlen(texture->name) + 1);
        if (full_path == NULL) continue;

        *full_path = 0;
        strcat(full_path, prefix_res_textures);
        strcat(full_path, texture->name);
        strcat(full_path, suffix_png);

        r_texture_t *rc_texture = r_res_texture_from_name(full_path);
        S->texture = rc_texture;

        if (S->texture) {
            texture->width = rc_texture->w;
            texture->height = rc_texture->h;
        }
    }

    geo_generator_run();

    for (int i = 0; i < map_data_get_texture_count(); i++) {
        r_surface_t *S = &M->surfaces[i];
        texture_data *texture = map_data_get_texture(i);

        surface_gatherer_reset_params();
        surface_gatherer_set_split_type(SST_BRUSH);
        surface_gatherer_set_texture_filter(texture->name);
        surface_gatherer_set_brush_filter_texture("dev/clip");
        surface_gatherer_set_face_filter_texture("dev/skip");
        surface_gatherer_set_worldspawn_layer_filter(1);

        surface_gatherer_run();
        const surfaces *surfaces = surface_gatherer_fetch();
        _debug(surfaces->surface_count);

        S->meshes = malloc(sizeof(r_mesh_t) * surfaces->surface_count);
        S->mesh_count = surfaces->surface_count;
        if (S->meshes == NULL) continue;

        for (int s = 0; s < surfaces->surface_count; s++) {
            surface *current_surface = &surfaces->surfaces[s];
            r_mesh_t *mesh = &S->meshes[s];

            glGenVertexArrays(1, &mesh->vao);
            glGenBuffers(1, &mesh->vbo);
            glGenBuffers(1, &mesh->ebo);

            _debug(mesh->vao);

            mesh->vertices = (uint32_t) current_surface->index_count;
            for (int v = 0; v < current_surface->vertex_count; v++) {
                // SHHHH SSSSSSHUT THE [ expletive ] UP DON'T TELL ANYONE I DID
                // THIS
                face_vertex *current_vertex = &current_surface->vertices[v];
                glm_vec3_scale(current_vertex->vertex, 1.f / 16.f, current_vertex->vertex);

                float vertex_z = current_vertex->vertex[2];
                float normal_z = current_vertex->normal[2];
                current_vertex->vertex[2] = current_vertex->vertex[1];
                current_vertex->vertex[1] = vertex_z;
                current_vertex->normal[2] = current_vertex->normal[1];
                current_vertex->normal[1] = normal_z;
            }

            glBindVertexArray(mesh->vao);

            glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(face_vertex) * current_surface->vertex_count, current_surface->vertices, GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * current_surface->index_count, current_surface->indices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(face_vertex), NULL);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(face_vertex), (void *) offsetof(face_vertex, normal));
            glVertexAttribPointer(2, 2, GL_DOUBLE, GL_FALSE, sizeof(face_vertex), (void *) offsetof(face_vertex, uv));
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);

            glBindVertexArray(0);
        }
    }
    return M;
}

void r_map_draw(r_map_t *M) {
    for (int i = 0; i < M->surface_count; i++) {
        r_surface_t *S = &M->surfaces[i];
        if (S->texture != NULL)
           r_res_texture_bind(S->texture);

        for (int j = 0; j < S->mesh_count; j++) {
            r_mesh_t *mesh = &S->meshes[j];
            glBindVertexArray(mesh->vao);
            glDrawElements(GL_TRIANGLES, mesh->vertices, GL_UNSIGNED_INT, NULL);
        }
    }
}

void r_map_free(r_map_t *M) {
    
}
