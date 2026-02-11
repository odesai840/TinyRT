#include "window.h"
#include <stdio.h>

bool TinyRT_InitWindow(TinyRTWindowProps* props) {
    if(!glfwInit()){
        printf("Failed to initialize GLFW.\n");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(props->width, props->height, props->title, NULL, NULL);
    if (!window) {
        printf("Failed to create GLFW window.\n");
        glfwTerminate();
        return false;
    }

    return true;
}
