#include <iostream>
#include <sstream>
using namespace std;

#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "ShaderProgram.h"
#include "Texture2D.h"

const char* APP_TITLE = "Introduction to Modern OpenGL - Hello Shader";
const int gWindowWidth = 800;
const int gWindowHeight = 600;
GLFWwindow* gWindow = NULL;
bool gWireframe = false;
const std::string texture1Filename = "airplane.jpg";
const std::string texture2Filename = "crate.jpg";

// function prototypes
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void showFPS(GLFWwindow* window);
bool initOpenGL();

int main() {

    if (!initOpenGL()) {
        cerr << "GLFW initialization failed" << endl;
        return -1;
    }

    GLfloat verticies[] = {
        // position (x, y, z)   // tex coords
        -0.5f,  0.5f,  0.0f,    0.0f, 1.0f, // top left
         0.5f,  0.5f,  0.0f,    1.0f, 1.0f, // top right
         0.5f, -0.5f,  0.0f,    1.0f, 0.0f, // bottom right
        -0.5f, -0.5f,  0.0f,    0.0f, 0.0f // bottom left
    };

    GLuint indicies[] = {
        0, 1, 2, // triangle 2
        0, 2, 3 // triangle 1
    };

    // video buffer object and video array object
    GLuint vbo, ibo, vao; 

    // get an address on the video card buffer and assign to vbo
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // GL_STATIC_DRAW -> our veriticies won't change much
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // position attributes
    // Tells the layout of our verticies array
    // first parameter where vertextShaderSrc (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);
    glEnableVertexAttribArray(0); // enables our poitner

    // text coord
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // will allow us to call position by indicies, and saving space used for indicies
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

    ShaderProgram shaderProgram;
    shaderProgram.loadShaders("basic.vert", "basic.frag");

    Texture2D texture1;
    texture1.loadTexture(texture1Filename, true);

    Texture2D texture2;
    texture2.loadTexture(texture2Filename, true);

    // main loop
    while (!glfwWindowShouldClose(gWindow)) {
        showFPS(gWindow);
        // polls for input events
        // callback funtions
        glfwPollEvents();


        glClear(GL_COLOR_BUFFER_BIT);

        texture1.bind(0);
        texture2.bind(1);

        shaderProgram.use();

        glUniform1i(glGetUniformLocation(shaderProgram.getProgram(), "myTexture1"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram.getProgram(), "myTexture2"), 1);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // allows us to draw by using indicies
        glBindVertexArray(0);

        // two buffers 
        // 1) the one showing and the 
        // 2) back buffer where we are drawing
        // removes flicker/tearing
        glfwSwapBuffers(gWindow);
    }


    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);

    glfwTerminate();
    return 0;
}

bool initOpenGL() {

    if (!glfwInit()) {
        cerr << "GLFW initialization failed" << endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    gWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, APP_TITLE, NULL, NULL);
    if (gWindow == NULL) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(gWindow);

    glfwSetKeyCallback(gWindow, glfw_onKey);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "GLEW initialization failed" << endl;
        return false;
    }

    glClearColor(0.23f, 0.38f, 0.47f, 1.0f);

    return true;
}

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode) {
    // will be triggered when a key is pressed
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        gWireframe = !gWireframe;
        if (gWireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
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