#include "application.h"

#include <stdio.h>

#include "window.h"
#include "renderer.h"
#include "scene.h"
#include <stdlib.h>

bool TinyRT_Run(void) {
    TinyRTWindowProps* props = (TinyRTWindowProps*)malloc(sizeof(TinyRTWindowProps));
    
    if(props == NULL) {
        return false;
    }

    props->title = "TinyRT";
    props->width = 800;
    props->height = 600;

    if(!TinyRT_InitWindow(props)) {
        return false;
    }
    glfwMakeContextCurrent(window);

    if(!TinyRT_InitRenderer(props)) {
        return false;
    }

    free(props);

    Scene* scene = TinyRT_LoadScene("scenes/cornellbox.scene");

    bool running = true;
    while(running) {
        glfwPollEvents();

        if(glfwWindowShouldClose(window)){
            running = false;
            continue;
        }
        
        TinyRT_RenderScene(scene);

        glfwSwapBuffers(window);
    }

    TinyRT_FreeScene(scene);

    glfwDestroyWindow(window);
    glfwTerminate();

    return true;
}