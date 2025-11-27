#include "application.h"
#include "window.h"
#include "renderer.h"
#include <stdlib.h>

bool Run() {
    WindowProps* props = (WindowProps*)malloc(sizeof(WindowProps));
    
    if(props == NULL) {
        return false;
    }

    props->title = "TinyRT";
    props->width = 800;
    props->height = 600;

    if(!InitWindow(props)) {
        return false;
    }
    glfwMakeContextCurrent(window);

    if(!InitRenderer(props)) {
        return false;
    }

    free(props);

    bool running = true;
    while(running) {
        glfwPollEvents();

        if(glfwWindowShouldClose(window)){
            running = false;
            continue;
        }

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return true;
}