#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class Camera {
public:
	Camera(float WIDTH, float HEIGHT, GLuint shaderProgram, float cameraSpeed, float camSensitivity);
	~Camera();
	void ProcessMouseMovement(double xpos, double ypos);
	void ProcessInput(GLFWwindow* window);

	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	float speed;
	glm::mat4 view;
	glm::mat4 proj;
	float yaw = -90.0f;
	float pitch = 0.0f;
	float sensitivity;
	float firstMouse = true;
	double lastX;
	double lastY;
	int viewLocation;
	int projLocation;

private:
	void UpdateDirection();
};