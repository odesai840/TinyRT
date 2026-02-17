#include "renderer.h"
#include "math_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <time.h>

bool TinyRT_InitRenderer(TinyRTRenderer* renderer, TinyRTWindowProps* props)
{
    renderer->render_width = props->width;
    renderer->render_height = props->height;

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD.\n");
        return false;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, renderer->render_width, renderer->render_height);

    TinyRT_CreateRenderTexture(renderer);
    renderer->shader = TinyRT_CreateShader("shaders/shader.vert", "shaders/shader.frag");
    renderer->compute_shader = TinyRT_CreateComputeShader("shaders/shader.comp");
    TinyRT_CreateFullscreenQuad(renderer);

    glGenTextures(1, &renderer->accumulation_texture);
    glBindTexture(GL_TEXTURE_2D, renderer->accumulation_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, renderer->render_width, renderer->render_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    renderer->current_sample_count = 0;

    return true;
}

void TinyRT_ResetRender(TinyRTRenderer* renderer)
{
    rendering_complete = false;
    should_abort_render = false;
}

void InitRNG(void)
{
    rng_state = (uint32_t)time(NULL);
}

void TinyRT_RenderScene(TinyRTRenderer* renderer, Scene* scene) {
    if (!rendering_complete) {
        if (renderer->current_sample_count == 0) {
            printf("Rendering with %d samples per pixel...\n", SAMPLES_PER_PIXEL);
            TinyRT_UploadSceneToGPU(renderer, scene);

            float zeros[4] = {0, 0, 0, 0};
            glClearTexImage(renderer->accumulation_texture, 0, GL_RGBA, GL_FLOAT, zeros);
        }

        TinyRT_DispatchComputeRender(renderer, scene, SAMPLES_PER_PASS);

        if (renderer->current_sample_count >= SAMPLES_PER_PIXEL) {
            rendering_complete = true;
            printf("Rendering complete!\n");
            //TinyRT_SaveImage(renderer, "output.ppm");
        }
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(renderer->shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderer->render_texture);
    glBindVertexArray(renderer->quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

TinyRTVec3 TinyRT_TracePath(TinyRTRay ray, Scene* scene) {
    TinyRTVec3 color = {0.0f, 0.0f, 0.0f};
    TinyRTVec3 throughput = {1.0f, 1.0f, 1.0f};

    for (int bounce = 0; bounce < MAX_DEPTH; bounce++) {
        TinyRTHitResult out_hit = {FLT_MAX, {0, 0, 0}, {0, 0, 0, 255}};
        Object* hit_obj = NULL;

        for (int i = 0; i < scene->num_objects; i++) {
            Object* obj = &scene->objects[i];
            for (int j = 0; j < obj->num_vertices; j += 3) {
                int tri_idx = j / 3;
                TinyRTVec3 normal = obj->normals[tri_idx];

                if (TinyRT_RayTriangleIntersect(ray,
                                                obj->vertices[j],
                                                obj->vertices[j + 1],
                                                obj->vertices[j + 2],
                                                normal,
                                                &out_hit)) {
                    hit_obj = obj;
                }
            }
        }

        if (!hit_obj) {
            break;
        }

        TinyRTVec3 hitPoint = AddVector3(ray.origin,
                                         MultVector3Scalar(ray.direction, out_hit.t));
        hitPoint = AddVector3(hitPoint, MultVector3Scalar(out_hit.hit_normal, 0.001f));

        if (hit_obj->emission > 0.0f) {
            color.x += throughput.x * hit_obj->diffuse.x * hit_obj->emission;
            color.y += throughput.y * hit_obj->diffuse.y * hit_obj->emission;
            color.z += throughput.z * hit_obj->diffuse.z * hit_obj->emission;
            break;
        }

        if (bounce > 3) {
            float max_component = fmaxf(throughput.x, fmaxf(throughput.y, throughput.z));
            if (TinyRT_RandomFloat() > max_component) {
                break;
            }
            throughput.x /= max_component;
            throughput.y /= max_component;
            throughput.z /= max_component;
        }

        throughput.x *= hit_obj->diffuse.x;
        throughput.y *= hit_obj->diffuse.y;
        throughput.z *= hit_obj->diffuse.z;

        TinyRTVec3 new_direction = TinyRT_RandomHemisphere(out_hit.hit_normal);

        ray.origin = hitPoint;
        ray.direction = new_direction;
    }

    return color;
}

bool TinyRT_CheckInShadow(TinyRTVec3 point, TinyRTVec3 light_pos, Scene* scene, float max_dist)
{
    TinyRTRay shadow_ray;
    shadow_ray.origin = point;
    shadow_ray.direction = NormalizeVector3(SubVector3(light_pos, point));

    TinyRTHitResult shadow_hit = {FLT_MAX, {0, 0, 0}, {0, 0, 0, 255}};

    for (int i = 0; i < scene->num_objects; i++) {
        Object* obj = &scene->objects[i];
        for (int j = 0; j < obj->num_vertices; j += 3) {
            int triangle_index = j / 3;
            TinyRTVec3 triangle_normal = obj->normals[triangle_index];

            if (TinyRT_RayTriangleIntersect(shadow_ray, obj->vertices[j], obj->vertices[j+1], obj->vertices[j+2], triangle_normal, &shadow_hit))
            {
                if (shadow_hit.t > EPSILON && shadow_hit.t < max_dist) {
                    return false;
                }
            }
        }
    }

    return true;
}

int TinyRT_GetTotalTriangleCount(Scene* scene) {
    int total = 0;
    for (int i = 0; i < scene->num_objects; i++) {
        total += scene->objects[i].num_vertices / 3;
    }
    return total;
}

TinyRTVec3 TinyRT_RandomInUnitSphere(void) {
    while (1) {
        TinyRTVec3 p = {
            TinyRT_RandomFloatRange(-1.0f, 1.0f),
            TinyRT_RandomFloatRange(-1.0f, 1.0f),
            TinyRT_RandomFloatRange(-1.0f, 1.0f)
        };
        if (MagSqVector3(p) < 1.0f) {
            return p;
        }
    }
}

TinyRTVec3 TinyRT_RandomHemisphere(TinyRTVec3 normal) {
    TinyRTVec3 in_unit_sphere = TinyRT_RandomInUnitSphere();
    TinyRTVec3 direction = NormalizeVector3(in_unit_sphere);

    if (DotVector3(direction, normal) > 0.0f) {
        return direction;
    } else {
        return MultVector3Scalar(direction, -1.0f);
    }
}

GLuint TinyRT_CreateShader(const char* vertex_path, const char* fragment_path) {
    char* vertex_source = TinyRT_LoadShaderFile(vertex_path);
    if (!vertex_source) {
        printf("Failed to load vertex shader: %s\n", vertex_path);
        return 0;
    }
    GLuint vertex_shader = TinyRT_CompileShader(GL_VERTEX_SHADER, vertex_source);
    free(vertex_source);

    char* fragment_source = TinyRT_LoadShaderFile(fragment_path);
    if (!fragment_source) {
        printf("Failed to load fragment shader: %s\n", fragment_path);
        glDeleteShader(vertex_shader);
        return 0;
    }
    GLuint fragment_shader = TinyRT_CompileShader(GL_FRAGMENT_SHADER, fragment_source);
    free(fragment_source);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(program, 512, NULL, info_log);
        printf("Graphics shader linking failed: %s\n", info_log);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}

GLuint TinyRT_CreateComputeShader(const char* compute_path) {
    char* compute_source = TinyRT_LoadShaderFile(compute_path);
    if (!compute_source) {
        printf("Failed to load compute shader: %s\n", compute_path);
        return 0;
    }
    GLuint cs = TinyRT_CompileShader(GL_COMPUTE_SHADER, compute_source);
    free(compute_source);

    GLuint program = glCreateProgram();
    glAttachShader(program, cs);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(program, 512, NULL, info_log);
        printf("Compute shader linking failed: %s\n", info_log);
    }

    glDeleteShader(cs);

    return program;
}

char* TinyRT_LoadShaderFile(const char* filepath) {
    FILE* file = fopen(filepath, "rb");
    if (!file) {
        printf("Failed to open shader file: %s\n", filepath);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(length + 1);
    fread(buffer, 1, length, file);
    buffer[length] = '\0';

    fclose(file);
    return buffer;
}

GLuint TinyRT_CompileShader(GLenum type, const char* source) {
    GLuint program = glCreateShader(type);
    glShaderSource(program, 1, &source, NULL);
    glCompileShader(program);

    int success;
    glGetShaderiv(program, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(program, 512, NULL, info_log);
        printf("Shader compilation failed: %s\n", info_log);
    }

    return program;
}

void TinyRT_UploadSceneToGPU(TinyRTRenderer* renderer, Scene* scene) {
    int total_triangles = 0;
    for (int i = 0; i < scene->num_objects; i++) {
        total_triangles += scene->objects[i].num_vertices / 3;
    }

    TinyRTGPUTriangle* gpu_triangles = (TinyRTGPUTriangle*)malloc(sizeof(TinyRTGPUTriangle) * total_triangles);
    int tri_idx = 0;

    for (int i = 0; i < scene->num_objects; i++) {
        Object* obj = &scene->objects[i];
        for (int j = 0; j < obj->num_vertices; j += 3) {
            gpu_triangles[tri_idx].v0 = obj->vertices[j];
            gpu_triangles[tri_idx].v1 = obj->vertices[j + 1];
            gpu_triangles[tri_idx].v2 = obj->vertices[j + 2];
            gpu_triangles[tri_idx].normal = obj->normals[j / 3];
            gpu_triangles[tri_idx].diffuse = obj->diffuse;
            gpu_triangles[tri_idx].emission = obj->emission;
            tri_idx++;
        }
    }

    glGenBuffers(1, &renderer->triangle_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderer->triangle_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(TinyRTGPUTriangle) * total_triangles,
                 gpu_triangles, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, renderer->triangle_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    free(gpu_triangles);
}

void TinyRT_DispatchComputeRender(TinyRTRenderer* renderer, Scene* scene, int samples_per_pass)
{
    glUseProgram(renderer->compute_shader);

    glUniform1i(glGetUniformLocation(renderer->compute_shader, "num_triangles"),
                TinyRT_GetTotalTriangleCount(scene));
    glUniform3f(glGetUniformLocation(renderer->compute_shader, "camera_pos"),
                scene->camera_pos.x, scene->camera_pos.y, scene->camera_pos.z);
    glUniform1i(glGetUniformLocation(renderer->compute_shader, "width"), renderer->render_width);
    glUniform1i(glGetUniformLocation(renderer->compute_shader, "height"), renderer->render_height);
    glUniform1i(glGetUniformLocation(renderer->compute_shader, "samples_this_pass"), samples_per_pass);
    glUniform1i(glGetUniformLocation(renderer->compute_shader, "current_sample"), renderer->current_sample_count);
    glUniform1i(glGetUniformLocation(renderer->compute_shader, "max_depth"), MAX_DEPTH);

    glBindImageTexture(0, renderer->accumulation_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindImageTexture(1, renderer->render_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

    int num_groups_x = (renderer->render_width + 15) / 16;
    int num_groups_y = (renderer->render_height + 15) / 16;
    glDispatchCompute(num_groups_x, num_groups_y, 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    renderer->current_sample_count += samples_per_pass;
}

void TinyRT_CreateFullscreenQuad(TinyRTRenderer* renderer) {
    float quad_vertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &renderer->quad_vao);
    glGenBuffers(1, &renderer->quad_vbo);

    glBindVertexArray(renderer->quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void TinyRT_CreateRenderTexture(TinyRTRenderer* renderer) {
    glGenTextures(1, &renderer->render_texture);
    glBindTexture(GL_TEXTURE_2D, renderer->render_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderer->render_width, renderer->render_height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void TinyRT_SaveImage(TinyRTRenderer* renderer, const char* filename) {
    unsigned char* pixels = (unsigned char*)malloc(renderer->render_width * renderer->render_height * 4);

    glBindTexture(GL_TEXTURE_2D, renderer->render_texture);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Failed to open file: %s\n", filename);
        free(pixels);
        return;
    }

    fprintf(file, "P6\n%d %d\n255\n", renderer->render_width, renderer->render_height);

    for (int y = renderer->render_height - 1; y >= 0; y--) {
        for (int x = 0; x < renderer->render_width; x++) {
            int idx = (y * renderer->render_width + x) * 4;
            fwrite(&pixels[idx], 1, 3, file);
        }
    }

    fclose(file);
    free(pixels);
    printf("Image saved to %s\n", filename);
}
