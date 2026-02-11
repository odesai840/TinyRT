#pragma once

#include "vector.h"
#include <cjson/cJSON.h>

typedef struct Object {
    TinyRTVec3* vertices;
    int num_vertices;
    TinyRTVec3* normals;
    int num_normals;
    TinyRTVec3 diffuse;
    TinyRTVec3 specular;
    float emission;
} Object;

typedef struct Scene {
    TinyRTVec3 camera_pos;
    int num_objects;
    Object* objects;
} Scene;

TinyRTVec3 TinyRT_ParseVector3(cJSON* array);
TinyRTVec3* TinyRT_ParseVector3Array(cJSON* array, int* count);
Object* TinyRT_ParseObjectArray(cJSON* array, int* count);
Scene* TinyRT_LoadScene(const char* filepath);
void TinyRT_FreeScene(Scene* scene);

TinyRTVec3* TinyRT_ConvertQuadsToTriangles(TinyRTVec3* vertices, int vert_count, int* out_tris);
TinyRTVec3* TinyRT_ExpandNormals(TinyRTVec3* normals, int normal_count, int triangle_count);
