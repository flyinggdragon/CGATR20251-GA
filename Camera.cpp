#include "Camera.h"

Camera::Camera(float WIDTH, float HEIGHT, GLuint shaderProgram) {
	position = glm::vec3(0.0f, 0.0f, 3.0f);

	target = glm::vec3(0.0f, 0.0f, 0.0f);
	direction = glm::normalize(target - position);

	up = glm::vec3(0.0f, 1.0f, 0.0f);
	right = glm::normalize(glm::cross(up, direction));

	up = glm::cross(direction, right);
	speed = 0.005f;

	viewLocation = glGetUniformLocation(shaderProgram, "view");

	glm::mat4 proj = glm::mat4(1.0f);
	projLocation = glGetUniformLocation(shaderProgram, "projection");

	proj = glm::perspective(glm::radians(90.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(proj));
}

Camera::~Camera() {

}