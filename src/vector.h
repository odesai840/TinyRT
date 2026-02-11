#pragma once

typedef struct TinyRTVec3 {
    float x;
    float y;
    float z;
} TinyRTVec3;

TinyRTVec3 AddVector3(TinyRTVec3 a, TinyRTVec3 b);
TinyRTVec3 SubVector3(TinyRTVec3 a, TinyRTVec3 b);
TinyRTVec3 MultVector3(TinyRTVec3 a, TinyRTVec3 b);
TinyRTVec3 MultVector3Scalar(TinyRTVec3 a, float b);
TinyRTVec3 DivVector3Scalar(TinyRTVec3 a, float b);
float DotVector3(TinyRTVec3 a, TinyRTVec3 b);
TinyRTVec3 CrossVector3(TinyRTVec3 a, TinyRTVec3 b);
TinyRTVec3 NormalizeVector3(TinyRTVec3 a);
float MagVector3(TinyRTVec3 a);
float MagSqVector3(TinyRTVec3 a);
