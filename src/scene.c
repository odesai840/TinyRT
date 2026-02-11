#include "scene.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TinyRTVec3 TinyRT_ParseVector3(cJSON* array) {
    TinyRTVec3 v = {0};
    if (array && cJSON_GetArraySize(array) == 3) {
        v.x = cJSON_GetArrayItem(array, 0)->valuedouble;
        v.y = cJSON_GetArrayItem(array, 1)->valuedouble;
        v.z = cJSON_GetArrayItem(array, 2)->valuedouble;
    }
    return v;
}

TinyRTVec3* TinyRT_ParseVector3Array(cJSON* array, int* count) {
    if (!array) {
        *count = 0;
        return NULL;
    }

    *count = cJSON_GetArraySize(array);
    
    TinyRTVec3* vectors = (TinyRTVec3*)malloc(*count * sizeof(TinyRTVec3));
    if (!vectors) {
        *count = 0;
        return NULL;
    }

    cJSON* item = NULL;
    int i = 0;
    cJSON_ArrayForEach(item, array) {
        vectors[i++] = TinyRT_ParseVector3(item);
    }

    return vectors;
}

Object* TinyRT_ParseObjectArray(cJSON* array, int* count) {
    if (!array) {
        *count = 0;
        return NULL;
    }

    *count = cJSON_GetArraySize(array);
    if (*count == 0) {
        return NULL;
    }
    
    Object* objects = (Object*)malloc(*count * sizeof(Object));
    if (!objects) {
        *count = 0;
        return NULL;
    }

    cJSON* item = NULL;
    int i = 0;
    cJSON_ArrayForEach(item, array) {
        cJSON* vertices = cJSON_GetObjectItem(item, "vertices");
        cJSON* normals = cJSON_GetObjectItem(item, "normals");
        cJSON* material = cJSON_GetObjectItem(item, "material");
        cJSON* emission = cJSON_GetObjectItem(item, "emission");

        int temp_vert_count = 0;
        TinyRTVec3* temp_verts = TinyRT_ParseVector3Array(vertices, &temp_vert_count);
        objects[i].vertices = TinyRT_ConvertQuadsToTriangles(temp_verts, temp_vert_count, &objects[i].num_vertices);
        free(temp_verts);
        
        int temp_normal_count = 0;
        TinyRTVec3* temp_normals = TinyRT_ParseVector3Array(normals, &temp_normal_count);
        int num_triangles = objects[i].num_vertices / 3;
        objects[i].normals = TinyRT_ExpandNormals(temp_normals, temp_normal_count, num_triangles);
        objects[i].num_normals = num_triangles;
        free(temp_normals);

        if (material) {
            cJSON* diffuse = cJSON_GetObjectItem(material, "diffuse");
            cJSON* specular = cJSON_GetObjectItem(material, "specular");
            objects[i].diffuse = TinyRT_ParseVector3(diffuse);
            objects[i].specular = TinyRT_ParseVector3(specular);
        } else {
            objects[i].diffuse = (TinyRTVec3){0};
            objects[i].specular = (TinyRTVec3){0};
        }

        if (emission) {
            objects[i].emission = emission->valuedouble;
        } else {
            objects[i].emission = 0;
        }

        i++;
    }

    return objects;
}

Scene* TinyRT_LoadScene(const char* filepath) {
    printf("Loading scene from file: %s\n", filepath);
    
    FILE* file = fopen(filepath, "r");
    if (!file) {
        printf("Failed to open scene file.\n");
        return NULL;
    }

    int result = fseek(file, 0, SEEK_END);
    if (result != 0) {
        printf("Failed to read scene file.\n");
        return NULL;
    }
    long size = ftell(file);
    
    result = fseek(file, 0, SEEK_SET);
    if (result != 0) {
        printf("Failed to read scene file.\n");
        return NULL;
    }

    char* buffer = (char*)malloc(size + 1);
    if (buffer == NULL) {
        printf("Failed to allocate memory for scene data buffer.\n");
        return NULL;
    }
    
    size_t length = fread(buffer, 1, size, file);
    buffer[length] = '\0';
    
    result = fclose(file);
    if (result != 0) {
        printf("Failed to close scene file.\n");
        return NULL;
    }

    cJSON* json = cJSON_Parse(buffer);
    free(buffer);
    if (!json) {
        printf("Failed to parse scene file: %s\n", cJSON_GetErrorPtr());
        return NULL;
    }

    Scene* scene = (Scene*)malloc(sizeof(Scene));
    if (!scene) {
        cJSON_Delete(json);
        printf("Failed to allocate memory for scene.\n");
        return NULL;
    }

    cJSON* camera = cJSON_GetObjectItem(json, "camera");
    if (camera) {
        cJSON* position = cJSON_GetObjectItem(camera, "position");
        scene->camera_pos = TinyRT_ParseVector3(position);
    } else {
        scene->camera_pos = (TinyRTVec3){0};
    }

    cJSON* objects = cJSON_GetObjectItem(json, "objects");
    if (objects) {
        scene->objects = TinyRT_ParseObjectArray(objects, &scene->num_objects);
    }

    cJSON_Delete(json);
    printf("Scene loaded successfully.\n");
    return scene;
}

void TinyRT_FreeScene(Scene* scene) {
    if (!scene) {
        return;
    }

    if (scene->objects) {
        for (int i = 0; i < scene->num_objects; i++) {
            free(scene->objects[i].vertices);
            free(scene->objects[i].normals);
        }
        free(scene->objects);
    }

    free(scene);
}

TinyRTVec3* TinyRT_ConvertQuadsToTriangles(TinyRTVec3* vertices, int vert_count, int* out_tris) {
    if (vert_count % 4 == 0) {
        int numQuads = vert_count / 4;
        *out_tris = numQuads * 6;

        TinyRTVec3* triangles = (TinyRTVec3*)malloc(*out_tris * sizeof(TinyRTVec3));
        if (!triangles) {
            *out_tris = 0;
            return NULL;
        }

        for (int i = 0; i < numQuads; i++) {
            int quadStart = i * 4;
            int triStart = i * 6;

            triangles[triStart + 0] = vertices[quadStart + 0];
            triangles[triStart + 1] = vertices[quadStart + 1];
            triangles[triStart + 2] = vertices[quadStart + 2];

            triangles[triStart + 3] = vertices[quadStart + 0];
            triangles[triStart + 4] = vertices[quadStart + 2];
            triangles[triStart + 5] = vertices[quadStart + 3];
        }

        return triangles;
    }
    
    if (vert_count % 3 == 0) {
        *out_tris = vert_count;
        TinyRTVec3* result = (TinyRTVec3*)malloc(vert_count * sizeof(TinyRTVec3));
        memcpy(result, vertices, vert_count * sizeof(TinyRTVec3));
        return result;
    }
    
    *out_tris = 0;
    return NULL;
}

TinyRTVec3* TinyRT_ExpandNormals(TinyRTVec3* normals, int normal_count, int triangle_count) {
    TinyRTVec3* expanded = (TinyRTVec3*)malloc(triangle_count * sizeof(TinyRTVec3));
    if (!expanded) return NULL;
    
    for (int i = 0; i < triangle_count && i < normal_count; i++) {
        expanded[i] = normals[i];
    }
    
    for (int i = normal_count; i < triangle_count; i++) {
        expanded[i] = normals[normal_count - 1];
    }

    return expanded;
}
