#include <iostream>
#include <sstream>
using namespace std;

#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"

const char* APP_TITLE = "Introduction to Modern OpenGL - Hello Triangle";
const int gWindowWidth = 800;
const int gWindowHeight = 600;
GLFWwindow* gWindow = NULL;


const GLchar* vertextShaderSrc =
"#version 330 core\n"
"layout (location = 0) in vec3 pos;"
"layout (location = 1) in vec3 color;"
"out vec3 vert_color;"
"void main()"
"{"
"   vert_color = color;"
"   gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);"
"}";

const GLchar* fragmentShaderSrc =
"#version 330 core\n"
"in vec3 vert_color;"
"out vec4 frag_color;"
"void main()"
"{"
"   frag_color = vec4(vert_color, 1.0f);"
"}";

// function prototypes
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void showFPS(GLFWwindow* window);
bool initOpenGL();

int main() {
    if (!initOpenGL()) {
        cerr << "GLFW initialization failed" << endl;
        return -1;
    }

    GLfloat vert_pos[] = {
        // position (x, y, z)   
         0.0f,  0.5f,  0.0f, // top
         0.5f, -0.5f,  0.0f, // right
        -0.5f, -0.5f,  0.0f // left
    };

    GLfloat vert_color[] = {
        // color (r, g, b) 
        1.0f, 0.0f, 0.0f, // top
        0.0f, 1.0f, 0.0f, // right
        0.0f, 0.0f, 1.0f // left
    };

    // video buffer object and video array object
    GLuint vbo, vbo2, vao; 

    // get an address on the video card buffer and assign to vbo
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // GL_STATIC_DRAW -> our veriticies won't change much
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert_pos), vert_pos, GL_STATIC_DRAW);

    glGenBuffers(1, &vbo2);
    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert_color), vert_color, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // position attributes
    // Tells the layout of our verticies array
    // first parameter where vertextShaderSrc (location = 0)
    glBindBuffer(GL_ARRAY_BUFFER, vbo); // we switch to our first buffer because we can only work with on buffer at a time
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // color attributes, need to offset by 12 bytes
    // first parameter "1" coincides with (location = 1)
    glBindBuffer(GL_ARRAY_BUFFER, vbo2); // now activate the second buffer
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);


    // vertex shader
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertextShaderSrc, NULL);
    glCompileShader(vs);

    GLint result;
    GLchar infoLog[512];
    glGetShaderiv(vs, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(vs, sizeof(infoLog), NULL, infoLog);
        cout << "Error! Vertex shader failed to compile." << infoLog << endl;
    }

    // fragment shader
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSrc, NULL);
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(fs, sizeof(infoLog), NULL, infoLog);
        cout << "Error! Fragment shader failed to compile." << infoLog << endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
        cout << "Error! Shader Program linker failure " << infoLog << endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    // main loop
    while (!glfwWindowShouldClose(gWindow)) {
        showFPS(gWindow);
        // polls for input events
        // callback funtions
        glfwPollEvents();


        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        // two buffers 
        // 1) the one showing and the 
        // 2) back buffer where we are drawing
        // removes flicker/tearing
        glfwSwapBuffers(gWindow);
    }

    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    glfwTerminate();
    return 0;
}

bool initOpenGL() {
    !glewInit();
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