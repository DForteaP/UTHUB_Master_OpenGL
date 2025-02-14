#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Object.h"

#include "Shaders/fragment.glsl"
#include "Shaders/vertex.glsl"

#include "Objects/Cube.h"
#include "Objects/CubeSmall.h"
#include "Objects/Plane.h"
#include "Objects/Piramid.h"

const unsigned int width = 800, height = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(width, height, "Computacion Grafica", nullptr, nullptr);
    if (!window) {
        std::cerr << "Error al crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Error al inicializar GLAD" << std::endl;
        return -1;
    }
    
    Shader vertexShader(vertexShaderSource, GL_VERTEX_SHADER);
    Shader fragmentShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    ShaderProgram shaderProgram;
    shaderProgram.attachShader(vertexShader);
    shaderProgram.attachShader(fragmentShader);
    shaderProgram.link();
    glEnable(GL_DEPTH_TEST);

    // Crear objetos
    Object object0(verticesSuelo, sizeof(verticesSuelo), indicesSuelo, sizeof(indicesSuelo), "green.jpg");
    Object object1(verticespiramide, sizeof(verticespiramide), indicespiramide, sizeof(indicespiramide), "chill.jpg");
    Object object2(verticescubo, sizeof(verticescubo), indicescubo, sizeof(indicescubo), "red.jpg");
    Object object3(verticeslight, sizeof(verticeslight), indiceslight, sizeof(indiceslight), "yellow.jpg");
    Object object4(verticescubo, sizeof(verticescubo), indicescubo, sizeof(indicescubo), "purple.jpg");
    Object object5(verticescubo, sizeof(verticescubo), indicescubo, sizeof(indicescubo), "blue.jpg");
    Object object6(verticescubo, sizeof(verticescubo), indicescubo, sizeof(indicescubo), "purple.jpg");
    
    // Ajustar posición de los objetos
    object0.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))); //Suelo
    object1.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 0.0f, 0.0f))); //Piramide
    object2.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f))); //Cubo rojo
    object3.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f))); //Fuente de iluminación
    object4.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f))); //Cubo purpura
    object5.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f))); //Cubo azul
    object6.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f))); //Cubo purpura 2

    // Crear cámara
    Camera camera(width, height, glm::vec3(0.0f, 1.0f, 4.0f));

    // Bucle principal
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
        camera.CameraInputs(window);
        
        shaderProgram.setVec3("viewPos", camera.Position);
        shaderProgram.setVec3("lightPos", object3.getPosition());
        shaderProgram.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        shaderProgram.setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
        shaderProgram.use();
        
        camera.CameraMatrix(45.0f, 0.1f, 100.f, shaderProgram.ID, "CameraMatrix");
        
        // Dibujar los objetos
        object0.draw(shaderProgram);
        object1.draw(shaderProgram);
        object2.draw(shaderProgram);
        object3.draw(shaderProgram);
        object4.draw(shaderProgram);
        object5.draw(shaderProgram);
        object6.draw(shaderProgram);

        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}