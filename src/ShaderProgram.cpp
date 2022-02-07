#include "ShaderProgram.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "glm/gtc/type_ptr.hpp"

using namespace std;

#include <direct.h> // _getcwd
#include <stdlib.h> // free, perror
#include <stdio.h>  // printf
#include <string.h> // strlen

ShaderProgram::ShaderProgram() : mHandler(0) {

}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(mHandler);
}

bool ShaderProgram::loadShaders(const char* vsFilename, const char* fsFilename) {
    string vsString = fileToString(vsFilename);
    string fsString = fileToString(fsFilename);

    const GLchar* vsSourcePtr = vsString.c_str();
    const GLchar* fsSourcePtr = fsString.c_str();
    
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);


    glShaderSource(vs, 1, &vsSourcePtr, NULL);
    glShaderSource(fs, 1, &fsSourcePtr, NULL);


    glCompileShader(vs);
    checkCompileErrors(vs, VERTEX);


    glCompileShader(fs);
    checkCompileErrors(vs, FRAGMENT);


    mHandler = glCreateProgram();
    glAttachShader(mHandler, vs);
    glAttachShader(mHandler, fs);
    glLinkProgram(mHandler);

    checkCompileErrors(mHandler, PROGRAM);

    glDeleteShader(vs);
    glDeleteShader(fs);

    mUniformLocations.clear();
    
    return true;
}

void ShaderProgram::use() {
    if (mHandler > 0) {
        glUseProgram(mHandler);
    }
}

string ShaderProgram::fileToString(const string& filename) {
    stringstream ss;
    ifstream file;
    /*
    char* buffer;

    std::ofstream outfile("test.txt");

    outfile << "my text here!" << std::endl;

    outfile.close();

    // Get the current working directory:
    if ((buffer = _getcwd(NULL, 0)) == NULL)
        perror("_getcwd error");
    else
    {
        printf("%s \nLength: %zu\n", buffer, strlen(buffer));
        free(buffer);
    }
    */

    try {
        file.open(filename, ios::in);
        
        if (!file.fail()) {
            ss << file.rdbuf();
        }


        file.close();
    }
    catch (exception ex) {
        cerr << "Error reading shader filename!" << endl;
    }

    return ss.str();
}

void ShaderProgram::checkCompileErrors(GLuint shader, ShaderType type) {
    int status = 0;

    if (type == PROGRAM) {
        glGetProgramiv(mHandler, GL_LINK_STATUS, &status);
        if (status == GL_FALSE) {
            GLint length = 0;
            glGetProgramiv(mHandler, GL_INFO_LOG_LENGTH, &length);

            string errorLog(length, ' ');
            glGetProgramInfoLog(mHandler, length, &length, &errorLog[0]);
            cerr << "Error! Program failed to link. " << errorLog << endl;
        }
    }
    else {
        // VERTEX or FRAGMENT
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            GLint length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

            string errorLog(length, ' ');
            glGetShaderInfoLog(shader, length, &length, &errorLog[0]);
            cerr << "Error! Shader failed to link." << errorLog << endl;
        }
    }
}

GLuint ShaderProgram::getProgram()const {
    return mHandler;
}

GLint ShaderProgram::getUniformLocation(const GLchar* name) {
    map<string, GLint>::iterator it = mUniformLocations.find(name);

    if (it == mUniformLocations.end()) {
        mUniformLocations[name] = glGetUniformLocation(mHandler, name);
    }

    return mUniformLocations[name];
}

void ShaderProgram::setUniform(const GLchar* name, const glm::vec2& v) {
    GLint loc = getUniformLocation(name);
    glUniform2f(loc, v.x, v.y);
}

void ShaderProgram::setUniform(const GLchar* name, const glm::vec3& v) {
    GLint loc = getUniformLocation(name);
    glUniform3f(loc, v.x, v.y, v.z);
}

void ShaderProgram::setUniform(const GLchar* name, const glm::vec4& v) {
    GLint loc = getUniformLocation(name);
    glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void ShaderProgram::setUniform(const GLchar* name, const glm::mat4& m) {
    GLint loc = getUniformLocation(name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}