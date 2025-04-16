#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class Camera {
public:
	Camera(float WIDTH, float HEIGHT, GLuint shaderProgram);
	~Camera();

	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 direction;
	glm::vec3 up;
	glm::vec3 right;
	GLfloat speed = 0.005f;
	glm::mat4 view;
	glm::mat4 proj;
	int viewLocation;
	int projLocation;
};