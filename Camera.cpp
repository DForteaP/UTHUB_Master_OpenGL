#include "Camera.h"

Camera::Camera(int width, int height, glm::vec3 position)
{
	Camera::Width = width;
	Camera::Height = height;
	Position = position;
}

glm::mat4 Camera::CameraMatrix(float FOV, float nearPlane, float farPlane, unsigned int ShaderProgram, const char* uniform)
{
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	view = glm::lookAt(Position, Position + Orientation, Up);
	projection = glm::perspective(FOV, (float)Width / (float)Height, nearPlane,farPlane);

	GLuint CameraUniform = glGetUniformLocation(ShaderProgram, uniform);
	glUniformMatrix4fv(CameraUniform, 1, GL_FALSE, glm::value_ptr(projection * view));
	return projection;
}

void Camera::CameraInputs(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_W)) {
		Position += speed * Orientation;
	}

	if (glfwGetKey(window, GLFW_KEY_A)) {
		Position += speed * -glm::normalize(glm::cross(Orientation, Up));
	}

	if (glfwGetKey(window, GLFW_KEY_S)) {
		Position += speed * -Orientation;
	}

	if (glfwGetKey(window, GLFW_KEY_D)) {
		Position += speed * glm::normalize(glm::cross(Orientation, Up));
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)){
		speed = 0.1f;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
		speed = 1.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_Q)) {
		Position += speed * Up;
	}

	if (glfwGetKey(window, GLFW_KEY_E)) {
		Position += speed * -Up;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		if (!rightMouseButtonHeld) {
			rightMouseButtonHeld = true;

			glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
		}
		else {
			double currentMouseX, currentMouseY;
			glfwGetCursorPos(window, &currentMouseX, &currentMouseY);

			double deltaX = currentMouseX - lastMouseX;
			double deltaY = currentMouseY - lastMouseY;

			Orientation = glm::rotate(Orientation, -glm::radians((float)deltaX * mouseSensitivity), Up);
			glm::vec3 right = glm::normalize(glm::cross(Orientation, Up));
			Orientation = glm::rotate(Orientation, -glm::radians((float)deltaY * mouseSensitivity), right);

			lastMouseX = currentMouseX;
			lastMouseY = currentMouseY;
		}
	}
	else {
		rightMouseButtonHeld = false;
	}
}


