#include <iostream>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <STB/stb_image.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"

const unsigned int width = 800;
const unsigned int height = 600;

const char* vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 CameraMatrix;
uniform mat4 ModelMatrix;

void main() {
    gl_Position = CameraMatrix * ModelMatrix * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)glsl";

const char* fragmentShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D tex0;

void main() {
    FragColor = texture(tex0, TexCoord);
}
)glsl";


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

class ShaderProgram {
public:
    unsigned int ID;

    ShaderProgram() {
        ID = glCreateProgram();
    }

    void attachShader(const Shader& shader) {
        glAttachShader(ID, shader.ID);
    }

    void link() {
        glLinkProgram(ID);

        int success;
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(ID, 512, nullptr, infoLog);
            std::cerr << "Error: Program linking failed\n" << infoLog << std::endl;
        }
    }

    void use() const {
        glUseProgram(ID);
    }

    ~ShaderProgram() {
        glDeleteProgram(ID);
    }
};

class Texture {
public:
    unsigned int ID;

    Texture(const std::string& path, GLenum format = GL_RGB, GLenum wrapS = GL_REPEAT, GLenum wrapT = GL_REPEAT,
        GLenum minFilter = GL_LINEAR, GLenum magFilter = GL_NEAREST) {
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cerr << "Error: No se pudo cargar la textura en " << path << std::endl;
        }
        stbi_image_free(data);
    }

    void bind() const {
        glBindTexture(GL_TEXTURE_2D, ID);
    }

    ~Texture() {
        glDeleteTextures(1, &ID);
    }
};

class Mesh {
public:
    unsigned int VAO, VBO, EBO;
    size_t indexCount;

    Mesh(const float* vertices, size_t vertexSize, const unsigned int* indices, size_t indexSize) {
        indexCount = indexSize / sizeof(unsigned int);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertexSize, vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void bind() const {
        glBindVertexArray(VAO);
    }

    ~Mesh() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
};

class Object {
public:
    Mesh mesh;
    Texture texture;
    glm::mat4 modelMatrix;

    Object(const float* vertices, size_t vertexSize, const unsigned int* indices, size_t indexSize, const std::string& texturePath)
        : mesh(vertices, vertexSize, indices, indexSize), texture(texturePath) {
        modelMatrix = glm::mat4(1.0f);
    }

    void setModelMatrix(const glm::mat4& matrix) {
        modelMatrix = matrix;
    }

    void draw(ShaderProgram& shaderProgram) {
        int modelLoc = glGetUniformLocation(shaderProgram.ID, "ModelMatrix");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        texture.bind();
        mesh.bind();
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
    }
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


// COORDENADAS E INDICES DE OBJETOS

float verticespiramide[] = {
    -0.5f, 0.0f,  0.5f,   0.0f, 0.0f,
     0.5f, 0.0f,  0.5f,   1.0f, 0.0f,
     0.5f, 0.0f, -0.5f,   1.0f, 1.0f,
    -0.5f, 0.0f, -0.5f,   0.0f, 1.0f,
     0.0f, 0.8f,  0.0f,   0.5f, 0.5f
};

unsigned int indicespiramide[] = {
    0, 1, 2,
    0, 2, 3,
    0, 1, 4,
    1, 2, 4,
    2, 3, 4,
    3, 0, 4
};

float verticescubo[] = {

    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f,  
    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 

    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,  
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f,  
    -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 

    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,   1.0f, 0.0f,  
    -0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 
    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 

     0.5f, -0.5f, -0.5f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f,  
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 
     0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 

     -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 
      0.5f, -0.5f, -0.5f,   1.0f, 0.0f,  
      0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 
     -0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 

     -0.5f,  0.5f, -0.5f,   0.0f, 0.0f,  
      0.5f,  0.5f, -0.5f,   1.0f, 0.0f,  
      0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 
     -0.5f,  0.5f,  0.5f,   0.0f, 1.0f  
};

unsigned int indicescubo[] = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4,
    8, 9, 10, 10, 11, 8,
    12, 13, 14, 14, 15, 12,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 23, 20
};



int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "Computacion grafica", nullptr, nullptr);
    if (!window) {
        std::cerr << "Error creating GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader vertexShader(vertexShaderSource, GL_VERTEX_SHADER);
    Shader fragmentShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    ShaderProgram shaderProgram;
    shaderProgram.attachShader(vertexShader);
    shaderProgram.attachShader(fragmentShader);
    shaderProgram.link();

    glEnable(GL_DEPTH_TEST);

    // Crear varios objetos
    Object object1(verticespiramide, sizeof(verticespiramide), indicespiramide, sizeof(indicespiramide), "chill.jpg");
    Object object2(verticescubo, sizeof(verticescubo), indicescubo, sizeof(indicescubo), "chill.jpg");

    //Desplazamiento objetos
    object2.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 0.0f)));

    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glDisable(GL_CULL_FACE);

        shaderProgram.use();
        camera.CameraMatrix(45.0f, 0.1f, 100.f, shaderProgram.ID, "CameraMatrix");
        camera.CameraInputs(window);

        // Dibujar el primer objeto
        object1.draw(shaderProgram);

        // Dibujar el segundo objeto
        object2.draw(shaderProgram);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}