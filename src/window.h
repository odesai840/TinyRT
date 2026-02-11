#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdbool.h>

GLFWwindow* window;

typedef struct TinyRTWindowProps {
    const char* title;
    int width;
    int height;
} TinyRTWindowProps;

bool TinyRT_InitWindow(TinyRTWindowProps* props);
