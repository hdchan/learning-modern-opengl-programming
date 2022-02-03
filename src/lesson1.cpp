#include <iostream>
using namespace std;

#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"


int main() {

    if (!glfwInit()) {
        cerr << "GLFW initialization failed" << endl;
        return -1;
    }



    return 0;
}