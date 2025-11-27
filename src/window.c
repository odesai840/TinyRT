#include "window.h"

bool InitWindow(WindowProps* props) {
    if(!glfwInit()){
        return false;
    }

    window = glfwCreateWindow(props->width, props->height, props->title, NULL, NULL);

    return true;
}
