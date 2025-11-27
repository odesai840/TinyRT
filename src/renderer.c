#include "renderer.h"
#include <glad/glad.h>

bool InitRenderer(WindowProps* props) {
    renderWidth = props->width;
    renderHeight = props->height;

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return false;
    }
    
    glEnable(GL_DEPTH_TEST);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, renderWidth, renderHeight);
    
    return true;
}
