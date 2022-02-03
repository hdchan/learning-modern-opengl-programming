#include <iostream>
#include <sstream>
using namespace std;

#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"

const char* APP_TITLE = "Introduction to Modern OpenGL - Hellow Window 1";
const int gWindowWidth = 800;
const int gWindowHeight = 600;
bool gFullscreen = true;

// function prototypes
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void showFPS(GLFWwindow* window);

int main() {
    !glewInit();
    if (!glfwInit()) {
        cerr << "GLFW initialization failed" << endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* pWindow = NULL;

    if (gFullscreen) {
        // enumerate all monitors
        // create in our primary monitor
        GLFWmonitor* pMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* pVmode = glfwGetVideoMode(pMonitor);
        if (pVmode != NULL) {
            pWindow = glfwCreateWindow(pVmode->width, pVmode->height, APP_TITLE, pMonitor, NULL);
        }   
    }
    else {
        pWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, APP_TITLE, NULL, NULL);
    }

    if (pWindow == NULL) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(pWindow);

    glfwSetKeyCallback(pWindow, glfw_onKey);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "GLEW initialization failed" << endl;
        return -1;
    }

    // main loop
    while (!glfwWindowShouldClose(pWindow)) {
        showFPS(pWindow);
        // polls for input events
        // callback funtions
        glfwPollEvents();


        glClearColor(0.23f, 0.38f, 0.47f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // two buffers 
        // 1) the one showing and the 
        // 2) back buffer where we are drawing
        // removes flicker/tearing
        glfwSwapBuffers(pWindow); 
    }

    glfwTerminate();
    return 0;
}

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode) {
    // will be triggered when a key is pressed
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void showFPS(GLFWwindow* window) {
    static double previousSeconds = 0.0;
    static int frameCount = 0;
    double elapsedSeconds;
    double currentSeconds = glfwGetTime(); // number of seconds since GLFW started as a double

    elapsedSeconds = currentSeconds - previousSeconds;

    // limit test update 4 times per seconds
    if (elapsedSeconds > 0.25) {
        previousSeconds = currentSeconds;
        double fps = (double)frameCount / elapsedSeconds;
        double msPerFrame = 1000.0 / fps;

        ostringstream outs;
        outs.precision(3);
        outs << fixed
            << APP_TITLE << "       "
            << "FPS: " << fps << "      "
            << "Frame Time: " << msPerFrame << " (ms)";
        glfwSetWindowTitle(window, outs.str().c_str());

        frameCount = 0;
    }

    frameCount++;
}