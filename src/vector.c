#include "vector.h"
#include <math.h>

TinyRTVec3 AddVector3(TinyRTVec3 a, TinyRTVec3 b) {
    TinyRTVec3 c;
    
    c.x = a.x + b.x;
    c.y = a.y + b.y;
    c.z = a.z + b.z;
    
    return c;
}

TinyRTVec3 SubVector3(TinyRTVec3 a, TinyRTVec3 b) {
    TinyRTVec3 c;
    
    c.x = a.x - b.x;
    c.y = a.y - b.y;
    c.z = a.z - b.z;
    
    return c;
}

TinyRTVec3 MultVector3(TinyRTVec3 a, TinyRTVec3 b) {
    TinyRTVec3 c;
    
    c.x = a.x * b.x;
    c.y = a.y * b.y;
    c.z = a.z * b.z;
    
    return c;
}

TinyRTVec3 MultVector3Scalar(TinyRTVec3 a, float b) {
    TinyRTVec3 c;
    
    c.x = a.x * b;
    c.y = a.y * b;
    c.z = a.z * b;
    
    return c;
}

TinyRTVec3 DivVector3Scalar(TinyRTVec3 a, float b) {
    TinyRTVec3 c;
    
    c.x = a.x / b;
    c.y = a.y / b;
    c.z = a.z / b;
    
    return c;
}

float DotVector3(TinyRTVec3 a, TinyRTVec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

TinyRTVec3 CrossVector3(TinyRTVec3 a, TinyRTVec3 b) {
    TinyRTVec3 c;
    
    c.x = a.y * b.z - a.z * b.y;
    c.y = a.z * b.x - a.x * b.z;
    c.z = a.x * b.y - a.y * b.x;
    
    return c;
}

TinyRTVec3 NormalizeVector3(TinyRTVec3 a) {
    TinyRTVec3 c;
    float mag = MagVector3(a);
    
    if (mag == 0.0f) {
        c.x = 0.0f;
        c.y = 0.0f;
        c.z = 0.0f;
        return c;
    }
    
    c.x = a.x / mag;
    c.y = a.y / mag;
    c.z = a.z / mag;
    return c;
}

float MagVector3(TinyRTVec3 a) {
    return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

float MagSqVector3(TinyRTVec3 a) {
    return a.x * a.x + a.y * a.y + a.z * a.z;
}
