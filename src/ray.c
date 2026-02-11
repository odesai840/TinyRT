#include "ray.h"
#include "math_util.h"
#include <math.h>

TinyRTRay TinyRT_GenerateRay(float pixelX, float pixelY, int width, int height, TinyRTVec3 camera_pos)
{
    TinyRTRay ray;
    ray.origin = camera_pos;
    
    float screenX = (2.0f * pixelX / width) - 1.0f;
    float screenY = 1.0f - (2.0f * pixelY / height);
    
    float aspect = (float)(width / height);
    float fov = 90.0f;
    float scale = tanf(fov * 0.5f * MATH_PI / 180.0f);
    
    ray.direction.x = screenX * aspect * scale;
    ray.direction.y = screenY * scale;
    ray.direction.z = 1.0f;
    ray.direction = NormalizeVector3(ray.direction);
    
    return ray;
}

bool TinyRT_RayTriangleIntersect(TinyRTRay ray, TinyRTVec3 vert0, TinyRTVec3 vert1, TinyRTVec3 vert2, TinyRTVec3 normal, TinyRTHitResult* hit_result)
{
    TinyRTVec3 edge1 = SubVector3(vert1, vert0);
    TinyRTVec3 edge2 = SubVector3(vert2, vert0);
    TinyRTVec3 h = CrossVector3(ray.direction, edge2);
    float a = DotVector3(edge1, h);
    
    if (a > -EPSILON && a < EPSILON)
    {
        return false;
    }
    
    float f = 1.0f / a;
    TinyRTVec3 s = SubVector3(ray.origin, vert0);
    float u = f * DotVector3(s, h);
    
    if (u < 0.0f || u > 1.0f)
    {
        return false;
    }
    
    TinyRTVec3 q = CrossVector3(s, edge1);
    float v = f * DotVector3(ray.direction, q);
    
    if (v < 0.0f || u + v > 1.0f)
    {
        return false;
    }
    
    float t = f * DotVector3(edge2, q);
    
    if (t > EPSILON && t < hit_result->t)
    {
        hit_result->t = t;
        hit_result->hit_normal = normal;
        return true;
    }
    
    return false;
}
