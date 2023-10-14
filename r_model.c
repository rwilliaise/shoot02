
#include "r_model.h"

#include "cglm/cglm.h"
#include "glad/gl.h"

#include <stdlib.h>
#include <stdio.h>

#include <assimp/material.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct r_vertex {
    vec3 position;
    vec3 normal;
    vec2 uv;
};

static void r_load_mesh(r_model_t *M, const struct aiScene *scene, const struct aiMesh *mesh, int index) {
    r_mesh_t *out = &M->meshes[index];
    glGenVertexArrays(1, &out->vao);
    glGenBuffers(1, &out->vbo);
    if (mesh->mFaces != NULL)
        glGenBuffers(1, &out->ebo);

    _debug(out->vao);
    _debug(out->vbo);
    _debug(out->ebo);

    size_t vertex_data_size = sizeof(struct r_vertex) * mesh->mNumVertices;
    struct r_vertex *vertex_data = malloc(vertex_data_size);
    if (vertex_data == NULL) {
        glDeleteVertexArrays(1, &out->vao);
        glDeleteBuffers(1, &out->vbo);
        if (mesh->mFaces != NULL)
            glDeleteBuffers(1, &out->ebo);
        return;
    }
    for (int i = 0; i < mesh->mNumVertices; i++) {
        struct r_vertex *vertex = &vertex_data[i];
        memcpy(vertex->position, &mesh->mVertices[i], sizeof(float) * 3);
        if (mesh->mNormals != NULL)
            memcpy(vertex->normal, &mesh->mNormals[i], sizeof(float) * 3);
        if (mesh->mTextureCoords[0] != NULL)
            memcpy(vertex->uv, &mesh->mTextureCoords[0][i], sizeof(float) * 2);
    }

    uint32_t *index_data = NULL;
    if (mesh->mFaces != NULL) {
        out->vertices = mesh->mNumFaces; // at least one index per face
        size_t index_data_curr = 0;
        index_data = malloc(sizeof(uint32_t) * out->vertices);
        if (index_data == NULL) {
            free(vertex_data);
            glDeleteVertexArrays(1, &out->vao);
            glDeleteBuffers(1, &out->vbo);
            if (mesh->mFaces != NULL)
                glDeleteBuffers(1, &out->ebo);
            return;
        }
        for (int i = 0; i < mesh->mNumFaces; i++) {
            struct aiFace face = mesh->mFaces[i];
            if (out->vertices - index_data_curr < face.mNumIndices) {
                out->vertices = index_data_curr + face.mNumIndices;
                index_data = realloc(index_data, sizeof(uint32_t) * out->vertices);
                if (index_data == NULL) {
                    free(vertex_data);
                    glDeleteVertexArrays(1, &out->vao);
                    glDeleteBuffers(1, &out->vbo);
                    if (mesh->mFaces != NULL)
                        glDeleteBuffers(1, &out->ebo);
                    return;
                }
            }
            for (int j = 0; j < face.mNumIndices; j++) {
                index_data[index_data_curr++] = face.mIndices[j];
            }
        }
    } else {
        out->vertices = mesh->mNumVertices;
    }

    _debug(out->vertices);
    glBindVertexArray(out->vao);
    glBindBuffer(GL_ARRAY_BUFFER, out->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_data_size, vertex_data, GL_STATIC_DRAW);
    if (mesh->mFaces != NULL) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out->ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * out->vertices, index_data, GL_STATIC_DRAW);
    }

    free(vertex_data);
    free(index_data);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct r_vertex), NULL);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct r_vertex), (void *) offsetof(struct r_vertex, normal));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct r_vertex), (void *) offsetof(struct r_vertex, uv));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

static void r_traverse_node(r_model_t **pM, const struct aiScene *scene, const struct aiNode *node) {
    if (*pM == NULL) return;

    r_model_t *M = *pM;
    int start = M->mesh_count;
    M->mesh_count += node->mNumMeshes;
    _debug(M->mesh_count);
    *pM = realloc(M, sizeof(r_model_t) + sizeof(r_mesh_t) * M->mesh_count);

    if (*pM == NULL) return;
    M = *pM;

    for (int i = 0; i < node->mNumMeshes; i++) {
        const struct aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        _debug(start + i);
        r_load_mesh(M, scene, mesh, start + i);
    }

    for (int i = 0; i < node->mNumChildren; i++) {
        struct aiNode *child = node->mChildren[i];
        r_traverse_node(pM, scene, child);
    }
}

r_model_t *r_model_load(const char *path) {
    const struct aiScene *scene = aiImportFile(path,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType
    );

    if (scene == NULL || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == NULL) {
        aiReleaseImport(scene);
        fprintf(stderr, "Assimp ERROR: %s\n", aiGetErrorString());
        return NULL;
    }

    r_model_t *M = malloc(sizeof(r_model_t));
    M->mesh_count = 0;
    r_traverse_node(&M, scene, scene->mRootNode);

    if (M == NULL) {
        aiReleaseImport(scene);
        return NULL;
    }

    aiReleaseImport(scene);
    return M;
}

void r_model_draw(r_model_t *M) {
    for (int i = 0; i < M->mesh_count; i++) {
        r_mesh_t *mesh = &M->meshes[i];
        glBindVertexArray(mesh->vao);
        if (mesh->ebo != 0)
            glDrawElements(GL_TRIANGLES, mesh->vertices, GL_UNSIGNED_INT, NULL);
        else
            glDrawArrays(GL_TRIANGLES, 0, mesh->vertices);
    }
}

void r_model_free(r_model_t *M) {
    for (int i = 0; i < M->mesh_count; i++) {
        r_mesh_t *mesh = &M->meshes[i];
        glDeleteVertexArrays(1, &mesh->vao);
        glDeleteBuffers(1, &mesh->vbo);
        if (mesh->ebo != 0)
            glDeleteBuffers(1, &mesh->ebo);
    }
    free(M);
}

