#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdbool.h>

GLFWwindow* window;

typedef struct WindowProps {
    const char* title;
    int width;
    int height;
} WindowProps;

bool InitWindow(WindowProps* props);
