#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

class Camera
{
public:

	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

	bool rightMouseButtonHeld = false;
	double lastMouseX, lastMouseY;
	float mouseSensitivity = 0.5f;

	int Width;
	int Height;

	float speed = 0.3f;

	Camera(int width, int height, glm::vec3 position);

	glm::mat4 CameraMatrix(float FOV, float nearPlane, float farPlane, unsigned int ShaderProgram, const char* uniform);

	void CameraInputs(GLFWwindow* window);
};

