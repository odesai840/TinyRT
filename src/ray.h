#pragma once

#include "vector.h"
#include "color.h"
#include <stdbool.h>

typedef struct TinyRTHitResult
{
    float t;
    TinyRTVec3 hit_normal;
    TinyRTColor hit_color;
} TinyRTHitResult;

typedef struct TinyRTRay
{
    TinyRTVec3 origin;
    TinyRTVec3 direction;
} TinyRTRay;

TinyRTRay TinyRT_GenerateRay(float pixelX, float pixelY, int width, int height, TinyRTVec3 camera_pos);
bool TinyRT_RayTriangleIntersect(TinyRTRay ray, TinyRTVec3 vert0, TinyRTVec3 vert1, TinyRTVec3 vert2, TinyRTVec3 normal, TinyRTHitResult* hit_result);
