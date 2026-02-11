#pragma once

#include "window.h"
#include "scene.h"
#include "ray.h"
#include <glad/glad.h>
#include <stdbool.h>

#define SAMPLES_PER_PIXEL 10000
#define SAMPLES_PER_PASS 10
#define ROWS_PER_BATCH 1
#define MAX_DEPTH 50

int render_width;
int render_height;

GLuint render_texture;
GLuint accumulation_texture;
GLuint shader;
GLuint compute_shader;
GLuint quad_vao, quad_vbo;
GLuint triangle_ssbo;

int current_sample_count;

static bool rendering_complete = false;
static bool should_abort_render = false;

typedef struct TinyRTGPUTriangle {
    TinyRTVec3 v0;
    float pad0;
    TinyRTVec3 v1;
    float pad1;
    TinyRTVec3 v2;
    float pad2;
    TinyRTVec3 normal;
    float pad3;
    TinyRTVec3 diffuse;
    float emission;
} TinyRTGPUTriangle;

bool TinyRT_InitRenderer(TinyRTWindowProps* props);
void TinyRT_ResetRender(void);

void TinyRT_InitRNG(void);

void TinyRT_RenderScene(Scene* scene);
TinyRTVec3 TinyRT_TracePath(TinyRTRay ray, Scene* scene);
bool TinyRT_CheckInShadow(TinyRTVec3 point, TinyRTVec3 light_pos, Scene* scene, float max_dist);
int TinyRT_GetTotalTriangleCount(Scene* scene);

TinyRTVec3 TinyRT_RandomInUnitSphere(void);
TinyRTVec3 TinyRT_RandomHemisphere(TinyRTVec3 normal);

GLuint TinyRT_CreateShader(const char* vertex_path, const char* fragment_path);
GLuint TinyRT_CreateComputeShader(const char* compute_path);
char* TinyRT_LoadShaderFile(const char* filepath);
GLuint TinyRT_CompileShader(GLenum type, const char* source);
void TinyRT_UploadSceneToGPU(Scene* scene);
void TinyRT_DispatchComputeRender(Scene* scene, int samples_per_pass);

void TinyRT_CreateFullscreenQuad(void);
void TinyRT_CreateRenderTexture(void);
void TinyRT_SaveImage(const char* filename);
