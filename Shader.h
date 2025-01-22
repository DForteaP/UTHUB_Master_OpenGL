#pragma once
#include <iostream>
#include "Dependencies/include/glad/glad.h"

class Shader {
public:
    unsigned int ID;

    Shader(const char* source, GLenum shaderType) {
        ID = glCreateShader(shaderType);
        glShaderSource(ID, 1, &source, nullptr);
        glCompileShader(ID);

        int success;
        glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(ID, 512, nullptr, infoLog);
            std::cerr << "Error: Shader compilation failed\n" << infoLog << std::endl;
        }
    }

    ~Shader() {
        glDeleteShader(ID);
    }
};
