#include "geo_generator.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "face.h"
#include "brush.h"
#include "entity.h"
#include "libmap_math.h"

const vec3 UP_VECTOR = {0.0, 0.0, 1.0};
const vec3 RIGHT_VECTOR = {0.0, 1.0, 0.0};
const vec3 FORWARD_VECTOR = {1.0, 0.0, 0.0};

bool smooth_normals = false;

int wind_entity_idx = 0;
int wind_brush_idx = 0;
int wind_face_idx = 0;
vec3 wind_face_center;
vec3 wind_face_basis;
vec3 wind_face_normal;

int sort_vertices_by_winding(const void *lhs_in, const void *rhs_in)
{
    const vec3 *lhs = (const vec3 *)lhs_in;
    const vec3 *rhs = (const vec3 *)rhs_in;

    vec3 u;
    glm_vec3_normalize_to(wind_face_basis, u);
    
    vec3 v;
    glm_vec3_cross(u, wind_face_normal, v);
    glm_vec3_normalize(v);
            
    vec3 local_lhs;
    glm_vec3_sub((float *) *lhs, wind_face_center, local_lhs);

    double lhs_pu = glm_vec3_dot(local_lhs, u);
    double lhs_pv = glm_vec3_dot(local_lhs, v);

    vec3 local_rhs;
    glm_vec3_sub((float *) *rhs, wind_face_center, local_rhs);

    double rhs_pu = glm_vec3_dot(local_rhs, u);
    double rhs_pv = glm_vec3_dot(local_rhs, v);

    double lhs_angle = atan2(lhs_pv, lhs_pu);
    double rhs_angle = atan2(rhs_pv, rhs_pu);

    if (lhs_angle < rhs_angle)
    {
        return -1;
    }
    else if (lhs_angle > rhs_angle)
    {
        return 1;
    }

    return 0;
}

void geo_generator_run()
{
    entity_geo = malloc(entity_count * sizeof(entity_geometry));

    for (int e = 0; e < entity_count; ++e)
    {
        entity *ent_inst = &entities[e];

        entity_geometry *entity_geo_inst = &entity_geo[e];
        *entity_geo_inst = (entity_geometry){0};

        entity_geo_inst->brushes = malloc(ent_inst->brush_count * sizeof(brush_geometry));

        for (int b = 0; b < ent_inst->brush_count; ++b)
        {
            brush *brush_inst = &ent_inst->brushes[b];

            brush_geometry *brush_geo_inst = &entity_geo_inst->brushes[b];
            *brush_geo_inst = (brush_geometry){0};

            brush_geo_inst->faces = malloc(brush_inst->face_count * sizeof(face_geometry));

            for (int f = 0; f < brush_inst->face_count; ++f)
            {
                face_geometry *face_geo_inst = &brush_geo_inst->faces[f];
                *face_geo_inst = (face_geometry){0};
            }
        }
    }

    for (int e = 0; e < entity_count; ++e)
    {
        entity *ent_inst = &entities[e];
        glm_vec3_zero(ent_inst->center);

        for (int b = 0; b < ent_inst->brush_count; ++b)
        {
            brush *brush_inst = &ent_inst->brushes[b];
            glm_vec3_zero(brush_inst->center);
            int vert_count = 0;

            generate_brush_vertices(e, b);

            brush_geometry *brush_geo_inst = &entity_geo[e].brushes[b];
            for (int f = 0; f < brush_inst->face_count; f++)
            {
                face_geometry *face_geo_inst = &brush_geo_inst->faces[f];

                for (int v = 0; v < face_geo_inst->vertex_count; ++v)
                {
                    glm_vec3_add(brush_inst->center, face_geo_inst->vertices[v].vertex, brush_inst->center);
                    vert_count++;
                }
            }

            if (vert_count > 0)
            {
                glm_vec3_divs(brush_inst->center, vert_count, brush_inst->center);
            }

            glm_vec3_add(ent_inst->center, brush_inst->center, ent_inst->center);
        }

        if (ent_inst->brush_count > 0)
        {
            glm_vec3_divs(ent_inst->center, ent_inst->brush_count, ent_inst->center);
        }
    }

    // Wind face vertices
    for (int e = 0; e < entity_count; ++e)
    {
        entity *entity_inst = &entities[e];
        entity_geometry *entity_geo_inst = &entity_geo[e];
        for (int b = 0; b < entity_inst->brush_count; ++b)
        {
            brush *brush_inst = &entity_inst->brushes[b];
            brush_geometry *brush_geo_inst = &entity_geo_inst->brushes[b];

            for (int f = 0; f < brush_inst->face_count; ++f)
            {
                face *face_inst = &brush_inst->faces[f];
                face_geometry *face_geo_inst = &brush_geo_inst->faces[f];

                if (face_geo_inst->vertex_count < 3)
                {
                    continue;
                }

                wind_entity_idx = e;
                wind_brush_idx = b;
                wind_face_idx = f;

                glm_vec3_sub(face_geo_inst->vertices[1].vertex, face_geo_inst->vertices[0].vertex, wind_face_basis);
                glm_vec3_zero(wind_face_center);
                glm_vec3_copy(face_inst->plane_normal, wind_face_normal);

                for (int v = 0; v < face_geo_inst->vertex_count; ++v)
                {
                    glm_vec3_add(wind_face_center, face_geo_inst->vertices[v].vertex, wind_face_center);
                }

                glm_vec3_divs(wind_face_center, face_geo_inst->vertex_count, wind_face_center);

                qsort(face_geo_inst->vertices, face_geo_inst->vertex_count, sizeof(face_vertex), sort_vertices_by_winding);

                wind_entity_idx = 0;
            }
        }
    }

    // Index face vertices
    for (int e = 0; e < entity_count; ++e)
    {
        entity *entity_inst = &entities[e];
        entity_geometry *entity_geo_inst = &entity_geo[e];
        for (int b = 0; b < entity_inst->brush_count; ++b)
        {
            brush *brush_inst = &entity_inst->brushes[b];
            brush_geometry *brush_geo_inst = &entity_geo_inst->brushes[b];

            for (int f = 0; f < brush_inst->face_count; ++f)
            {
                face_geometry *face_geo_inst = &brush_geo_inst->faces[f];

                if (face_geo_inst->vertex_count < 3)
                {
                    continue;
                }

                face_geo_inst->indices = malloc((face_geo_inst->vertex_count - 2) * 3 * sizeof(int));
                for (int i = 0; i < face_geo_inst->vertex_count - 2; i++)
                {
                    face_geo_inst->indices[face_geo_inst->index_count++] = 0;
                    face_geo_inst->indices[face_geo_inst->index_count++] = i + 1;
                    face_geo_inst->indices[face_geo_inst->index_count++] = i + 2;
                }
            }
        }
    }
}

void generate_brush_vertices(int entity_idx, int brush_idx)
{
    entity *ent_inst = &entities[entity_idx];
    brush *brush_inst = &ent_inst->brushes[brush_idx];

    for (int f0 = 0; f0 < brush_inst->face_count; ++f0)
    {
        for (int f1 = 0; f1 < brush_inst->face_count; ++f1)
        {
            for (int f2 = 0; f2 < brush_inst->face_count; ++f2)
            {
                vec3 vertex = GLM_VEC3_ZERO_INIT;
                if (intersect_faces(brush_inst->faces[f0], brush_inst->faces[f1], brush_inst->faces[f2], &vertex))
                {
                    if (vertex_in_hull(brush_inst->faces, brush_inst->face_count, vertex))
                    {
                        face *face_inst = &entities[entity_idx].brushes[brush_idx].faces[f0];
                        face_geometry *face_geo_inst = &entity_geo[entity_idx].brushes[brush_idx].faces[f0];

                        vec3 normal;

                        const char *phong_property = map_data_get_entity_property(entity_idx, "_phong");
                        bool phong = phong_property != NULL && strcmp(phong_property, "1") == 0;
                        if (phong)
                        {
                            const char *phong_angle_property = map_data_get_entity_property(entity_idx, "_phong_angle");
                            if (phong_angle_property != NULL)
                            {
                                double threshold = cos((atof(phong_angle_property) + 0.01) * 0.0174533);
                                glm_vec3_copy(brush_inst->faces[f0].plane_normal, normal);
                                if (glm_vec3_dot(brush_inst->faces[f0].plane_normal, brush_inst->faces[f1].plane_normal) > threshold)
                                {
                                    glm_vec3_add(normal, brush_inst->faces[f1].plane_normal, normal);
                                }
                                if (glm_vec3_dot(brush_inst->faces[f0].plane_normal, brush_inst->faces[f2].plane_normal) > threshold)
                                {
                                    glm_vec3_add(normal, brush_inst->faces[f2].plane_normal, normal);
                                }
                                glm_vec3_normalize(normal);
                            }
                            else
                            {
                                glm_vec3_add(
                                    brush_inst->faces[f1].plane_normal,
                                    brush_inst->faces[f2].plane_normal,
                                    normal
                                );
                                glm_vec3_add(
                                    brush_inst->faces[f0].plane_normal,
                                    normal,
                                    normal
                                );
                                glm_vec3_normalize(normal);
                            }
                        }
                        else
                        {
                            glm_vec3_copy(face_inst->plane_normal, normal);
                        }

                        texture_data *texture = map_data_get_texture(face_inst->texture_idx);

                        vertex_uv uv;
                        if (face_inst->is_valve_uv)
                        {
                            uv = get_valve_uv(vertex, face_inst, texture->width, texture->height);
                        }
                        else
                        {
                            uv = get_standard_uv(vertex, face_inst, texture->width, texture->height);
                        }

                        vertex_tangent tangent;
                        if (face_inst->is_valve_uv)
                        {
                            tangent = get_valve_tangent(face_inst);
                        }
                        else
                        {
                            tangent = get_standard_tangent(face_inst);
                        }

                        bool unique_vertex = true;
                        int duplicate_index = -1;

                        for (int v = 0; v < face_geo_inst->vertex_count; ++v)
                        {
                            vec3 comp_vertex;
                            glm_vec3_copy(face_geo_inst->vertices[v].vertex, comp_vertex);
                            glm_vec3_sub(vertex, comp_vertex, comp_vertex);
                            if (glm_vec3_norm(comp_vertex) < CMP_EPSILON)
                            {
                                unique_vertex = false;
                                duplicate_index = v;
                                break;
                            }
                        }

                        if (unique_vertex)
                        {
                            face_geo_inst->vertex_count++;
                            face_geo_inst->vertices = realloc(face_geo_inst->vertices, face_geo_inst->vertex_count * sizeof(face_vertex));

                            face_vertex *out_vertex = &face_geo_inst->vertices[face_geo_inst->vertex_count - 1];
                            glm_vec3_copy(vertex, out_vertex->vertex);
                            glm_vec3_copy(normal, out_vertex->normal);
                            out_vertex->uv = uv;
                            out_vertex->tangent = tangent;
                        }
                        else if(phong)
                        {
                            glm_vec3_add(face_geo_inst->vertices[duplicate_index].normal, normal, face_geo_inst->vertices[duplicate_index].normal);
                        }
                        
                    }
                }
            }
        }
    }

    for (int f = 0; f < brush_inst->face_count; ++f)
    {
        face_geometry *face_geo_inst = &entity_geo[entity_idx].brushes[brush_idx].faces[f];

        for (int v = 0; v < face_geo_inst->vertex_count; ++v)
        {
            glm_vec3_normalize(face_geo_inst->vertices[v].normal);
        }
    }
}

bool intersect_faces(face f0, face f1, face f2, vec3 *o_vertex)
{

    vec3 f0_f1;
    glm_vec3_cross(f0.plane_normal, f1.plane_normal, f0_f1);
    double denom = glm_vec3_dot(f0_f1, f2.plane_normal);

    if (denom < CMP_EPSILON)
    {
        return false;
    }

    if (o_vertex)
    {
        vec3 f1_f2;
        vec3 f2_f0;
        glm_vec3_cross(f1.plane_normal, f2.plane_normal, f1_f2);
        glm_vec3_cross(f2.plane_normal, f0.plane_normal, f2_f0);

        glm_vec3_scale(
            f1_f2,
            f0.plane_dist,
            f1_f2
        );
        glm_vec3_scale(
            f2_f0,
            f1.plane_dist,
            f2_f0
        );
        glm_vec3_scale(
            f0_f1,
            f2.plane_dist,
            f0_f1
        );

        glm_vec3_add(
            f1_f2,
            f2_f0,
            f1_f2
        );
        glm_vec3_add(
            f1_f2,
            f0_f1,
            *o_vertex
        );
        glm_vec3_divs(
            *o_vertex,
            denom,
            *o_vertex
        );
    }

    return true;
}

bool vertex_in_hull(face *faces, int face_count, vec3 vertex)
{
    for (int f = 0; f < face_count; f++)
    {
        face face_inst = faces[f];

        double proj = glm_vec3_dot(face_inst.plane_normal, vertex);

        if (proj > face_inst.plane_dist && fabs(face_inst.plane_dist - proj) > CMP_EPSILON)
        {
            return false;
        }
    }

    return true;
}

vertex_uv get_standard_uv(vec3 vertex, const face *face, int texture_width, int texture_height)
{
    vertex_uv uv_out;

    double du = fabs(glm_vec3_dot((float *) face->plane_normal, (float *) UP_VECTOR));
    double dr = fabs(glm_vec3_dot((float *) face->plane_normal, (float *) RIGHT_VECTOR));
    double df = fabs(glm_vec3_dot((float *) face->plane_normal, (float *) FORWARD_VECTOR));

    if (du >= dr && du >= df)
    {
        uv_out = (vertex_uv){vertex[0], -vertex[1]};
    }
    else if (dr >= du && dr >= df)
    {
        uv_out = (vertex_uv){vertex[0], -vertex[2]};
    }
    else if (df >= du && df >= dr)
    {
        uv_out = (vertex_uv){vertex[1], -vertex[2]};
    }

    vertex_uv rotated;
    double angle = DEG_TO_RAD(face->uv_extra.rot);
    rotated.u = uv_out.u * cos(angle) - uv_out.v * sin(angle);
    rotated.v = uv_out.u * sin(angle) + uv_out.v * cos(angle);
    uv_out = rotated;

    uv_out.u /= texture_width;
    uv_out.v /= texture_height;

    uv_out.u /= face->uv_extra.scale_x;
    uv_out.v /= face->uv_extra.scale_y;

    uv_out.u += face->uv_standard.u / texture_width;
    uv_out.v += face->uv_standard.v / texture_height;

    return uv_out;
}

vertex_uv get_valve_uv(vec3 vertex, const face *face, int texture_width, int texture_height)
{
    vertex_uv uv_out;

    double u_shift = face->uv_valve.u.offset;
    double v_shift = face->uv_valve.v.offset;

    uv_out.u = glm_vec3_dot((float *) face->uv_valve.u.axis, vertex);
    uv_out.v = glm_vec3_dot((float *) face->uv_valve.v.axis, vertex);

    uv_out.u /= texture_width;
    uv_out.v /= texture_height;

    uv_out.u /= face->uv_extra.scale_x;
    uv_out.v /= face->uv_extra.scale_y;

    uv_out.u += u_shift / texture_width;
    uv_out.v += v_shift / texture_height;

    return uv_out;
}

double sign(double v)
{
    if (v > 0)
    {
        return 1.0;
    }
    else if (v < 0)
    {
        return -1.0;
    }

    return 0.0;
}

vertex_tangent get_standard_tangent(const face *face)
{
    vertex_tangent tangent_out;

    double du = glm_vec3_dot((float *) face->plane_normal, (float *) UP_VECTOR);
    double dr = glm_vec3_dot((float *) face->plane_normal, (float *) RIGHT_VECTOR);
    double df = glm_vec3_dot((float *) face->plane_normal, (float *) FORWARD_VECTOR);

    double dua = fabs(du);
    double dra = fabs(dr);
    double dfa = fabs(df);

    vec3 u_axis;
    double v_sign = 0;

    if (dua >= dra && dua >= dfa)
    {
        glm_vec3_copy((float *) FORWARD_VECTOR, u_axis);
        v_sign = sign(du);
    }
    else if (dra >= dua && dra >= dfa)
    {
        glm_vec3_copy((float *) FORWARD_VECTOR, u_axis);
        v_sign = -sign(dr);
    }
    else if (dfa >= dua && dfa >= dra)
    {
        glm_vec3_copy((float *) RIGHT_VECTOR, u_axis);
        v_sign = sign(df);
    }

    v_sign *= sign(face->uv_extra.scale_y);
    glm_vec3_rotate(u_axis, -face->uv_extra.rot * v_sign, (float *) face->plane_normal);

    tangent_out.x = u_axis[0];
    tangent_out.y = u_axis[1];
    tangent_out.z = u_axis[2];
    tangent_out.w = v_sign;

    return tangent_out;
}

vertex_tangent get_valve_tangent(const face *face)
{
    vertex_tangent tangent_out;

    vec3 u_axis;
    vec3 v_axis;
    vec3 n_u;
    glm_vec3_normalize_to((float *) face->uv_valve.u.axis, u_axis);
    glm_vec3_normalize_to((float *) face->uv_valve.v.axis, v_axis);
    glm_vec3_cross((float *) face->plane_normal, u_axis, n_u);
    
    double v_sign = -sign(glm_vec3_dot(n_u, v_axis));

    tangent_out.x = u_axis[0];
    tangent_out.y = u_axis[1];
    tangent_out.z = u_axis[2];
    tangent_out.w = v_sign;

    return tangent_out;
}

void geo_generator_print_entities()
{
    for (int e = 0; e < entity_count; ++e)
    {
        entity *entity_inst = &entities[e];
        entity_geometry *entity_geo_inst = &entity_geo[e];
        printf("Entity %d\n", e);
        for (int b = 0; b < entity_inst->brush_count; ++b)
        {
            brush *brush_inst = &entity_inst->brushes[b];
            brush_geometry *brush_geo_inst = &entity_geo_inst->brushes[b];
            printf("Brush %d\n", b);

            for (int f = 0; f < brush_inst->face_count; ++f)
            {
                face_geometry *face_geo_inst = &brush_geo_inst->faces[f];
                printf("Face %d\n", f);
                for (int i = 0; i < face_geo_inst->vertex_count; ++i)
                {
                    face_vertex vertex = face_geo_inst->vertices[i];
                    printf("vertex: (%f %f %f), normal: (%f %f %f)\n",
                           vertex.vertex[0], vertex.vertex[1], vertex.vertex[2],
                           vertex.normal[0], vertex.normal[1], vertex.normal[2]);
                }

                puts("Indices:");
                for (int i = 0; i < (face_geo_inst->vertex_count - 2) * 3; ++i)
                {
                    printf("index: %d\n", face_geo_inst->indices[i]);
                }
            }

            putchar('\n');
            putchar('\n');
        }
    }
}

const entity_geometry *geo_generator_get_entities()
{
    return entity_geo;
}

int geo_generator_get_brush_vertex_count(int entity_idx, int brush_idx)
{
    int vertex_count = 0;

    brush *brush_inst = &entities[entity_idx].brushes[brush_idx];
    brush_geometry *brush_geo_inst = &entity_geo[entity_idx].brushes[brush_idx];

    for (int i = 0; i < brush_inst->face_count; ++i)
    {
        face_geometry *face_geo_inst = &brush_geo_inst->faces[i];
        vertex_count = vertex_count + face_geo_inst->vertex_count;
    }

    return vertex_count;
}

int geo_generator_get_brush_index_count(int entity_idx, int brush_idx)
{
    int index_count = 0;

    brush *brush_inst = &entities[entity_idx].brushes[brush_idx];
    brush_geometry *brush_geo_inst = &entity_geo[entity_idx].brushes[brush_idx];

    for (int i = 0; i < brush_inst->face_count; ++i)
    {
        face_geometry *face_geo_inst = &brush_geo_inst->faces[i];
        index_count = index_count + face_geo_inst->index_count;
    }

    return index_count;
}
