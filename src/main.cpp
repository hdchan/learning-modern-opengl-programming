#include <iostream>
#include <sstream>
using namespace std;

#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"

#include "ShaderProgram.h"
#include "Texture2D.h"
#include "Camera.h"

const char* APP_TITLE = "Introduction to Modern OpenGL - Hello Shader";
int gWindowWidth = 1024;
int gWindowHeight = 768;
GLFWwindow* gWindow = NULL;
bool gWireframe = false;
const std::string texture1Filename = "crate.jpg";

OrbitCamera orbitCamera;
float gYaw = 0.0f;
float gPitch = 0.0f;
float gRadius = 10.0f;
const float MOUSE_SENSITIVITY = 0.5f;

// function prototypes
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_OnFrameBufferSize(GLFWwindow* window, int width, int height);
void glfw_OnMouseMove(GLFWwindow* window, double posX, double posY);
void glfw_OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset);
void showFPS(GLFWwindow* window);
bool initOpenGL();

int main() {

    if (!initOpenGL()) {
        cerr << "GLFW initialization failed" << endl;
        return -1;
    }

    GLfloat verticies[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    // video buffer object and video array object
    GLuint vbo, vao; 

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

    ShaderProgram shaderProgram;
    shaderProgram.loadShaders("basic.vert", "basic.frag");

    Texture2D texture1;
    texture1.loadTexture(texture1Filename, true);

    // rotate cube, to look at it in an angle
    float cubeAngle = 0.0f;
    double lastTime = glfwGetTime();

    // main loop
    while (!glfwWindowShouldClose(gWindow)) {
        showFPS(gWindow);

        double currentTime = glfwGetTime();
        double deltaTime = currentTime = currentTime - lastTime;

        // polls for input events
        // callback funtions
        glfwPollEvents();

        // clear color and depth buffer every frame
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        texture1.bind(0);

        glm::vec3 cubePos = glm::vec3(0.0f, 0.0f, -5.0f);
        // create transformations
        glm::mat4 model, view, projection;
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

        model = glm::translate(model, cubePos);

        orbitCamera.setLookAt(cubePos);
        orbitCamera.rotate(gYaw, gPitch);
        orbitCamera.setRadius(gRadius);

        //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        view = orbitCamera.getViewMatrix();

        projection = glm::perspective(glm::radians(45.0f), (float)gWindowWidth / (float)gWindowHeight, 0.1f, 100.0f);

        shaderProgram.use();

        // retrieve the matrix uniform locations
        // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        shaderProgram.setUniform("model", model);
        shaderProgram.setUniform("view", view);
        shaderProgram.setUniform("projection", projection);


        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // two buffers 
        // 1) the one showing and the 
        // 2) back buffer where we are drawing
        // removes flicker/tearing
        glfwSwapBuffers(gWindow);

        lastTime = currentTime;
    }


    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

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
    glfwSetFramebufferSizeCallback(gWindow, glfw_OnFrameBufferSize);

    glfwSetKeyCallback(gWindow, glfw_onKey);
    glfwSetCursorPosCallback(gWindow, glfw_OnMouseMove);
    glfwSetScrollCallback(gWindow, glfw_OnMouseScroll);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "GLEW initialization failed" << endl;
        return false;
    }

    glClearColor(0.23f, 0.38f, 0.47f, 1.0f);
    glViewport(0, 0, gWindowWidth, gWindowHeight);
    // allows the shaders to draw the ones behind last
    glEnable(GL_DEPTH_TEST);


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

void glfw_OnFrameBufferSize(GLFWwindow* window, int width, int height) {
    gWindowWidth = width;
    gWindowHeight = height;
    glViewport(0, 0, gWindowWidth, gWindowHeight);
}

void glfw_OnMouseMove(GLFWwindow* window, double posX, double posY) {
    static glm::vec2 lastMousePos = glm::vec2(0, 0);

    if (glfwGetMouseButton(gWindow, GLFW_MOUSE_BUTTON_LEFT) == 1) {
        gYaw -= ((float)posX - lastMousePos.x) * MOUSE_SENSITIVITY;
        gPitch += ((float)posY - lastMousePos.y) * MOUSE_SENSITIVITY;
    }

    if (glfwGetMouseButton(gWindow, GLFW_MOUSE_BUTTON_RIGHT) == 1) {
        float dx = 0.01f * ((float)posX - lastMousePos.x);
        float dy = 0.01f * ((float)posY - lastMousePos.y);
        gRadius += dx - dy;
    }

    lastMousePos.x = (float)posX;
    lastMousePos.y = (float)posY;
}

void glfw_OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
    gRadius += yoffset;
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