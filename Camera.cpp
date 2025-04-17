#include "Camera.h"

Camera::Camera(float WIDTH, float HEIGHT, GLuint shaderProgram, float cameraSpeed, float camSensitivity) {
	sensitivity = camSensitivity;
	
	lastX = WIDTH / 2.0f;
	lastY = HEIGHT / 2.0f;
	
	view = glm::mat4(1.0f);
	proj = glm::mat4(1.0f);

	position = glm::vec3(0.0f, 0.0f, 3.0f);

	target = glm::vec3(0.0f, 0.0f, 0.0f);
	direction = glm::normalize(target - position);

	direction.x = cos(glm::radians(yaw));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw));

	up = glm::vec3(0.0f, 1.0f, 0.0f);
	right = glm::normalize(glm::cross(up, direction));

	up = glm::cross(direction, right);
	speed = cameraSpeed;

	viewLocation = glGetUniformLocation(shaderProgram, "view");

	glm::mat4 proj = glm::mat4(1.0f);
	projLocation = glGetUniformLocation(shaderProgram, "projection");

	proj = glm::perspective(glm::radians(90.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(proj));
}

Camera::~Camera() {

}

void Camera::UpdateDirection() {
	glm::vec3 newDirection;
	newDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	newDirection.y = sin(glm::radians(pitch));
	newDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	direction = glm::normalize(newDirection);
	right = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
	up = glm::normalize(glm::cross(right, direction));
}

void Camera::ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		position += speed * direction;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		position -= speed * direction;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		position -= right * speed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		position += right * speed;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	target = position + direction;

	view = glm::lookAt(position, target, up);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
}

void Camera::ProcessMouseMovement(double xpos, double  ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	UpdateDirection();
}