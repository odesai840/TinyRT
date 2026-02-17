#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdbool.h>

typedef struct TinyRTWindowProps {
    const char* title;
    int width;
    int height;
} TinyRTWindowProps;

GLFWwindow* TinyRT_InitWindow(TinyRTWindowProps* props);
