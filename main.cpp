#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>

#include"Obj3D.h"
#include"ObjReader.h"
#include"Face.h"
#include"Group.h"
#include"Mesh.h"

using namespace std;

const char* vShader =
"#version 460\n"
"layout(location=0) in vec3 vp;"
"layout(location=1) in vec3 vc;"
"uniform mat4 view;"
"uniform mat4 projection;"
"uniform mat4 transform;"
"out vec3 color;"
"void main () {"
"   color = vc;"
"	gl_Position = projection * view * transform * vec4(vp, 1.0);"
"}";

const char* fShader =
"#version 410\n"
"in vec3 color;"
"out vec4 frag_color;"
"void main () {"
"   frag_color = vec4(1.0, 1.0, 1.0, 1.0);"
"}";

int main() {
	if (!glfwInit()) {
		cerr << "Falha ao inicializar GLFW" << endl;
		return -1;
	}

	// Configura��es do OpenGL
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	const int WIDTH = 800;
	const int HEIGHT = 600;

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Camera", nullptr, nullptr);

	if (!window) {
		cerr << "Falha ao criar janela GLFW" << endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		cerr << "Falha ao inicializar GLEW" << endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);

	vector<Obj3D*> objects;

	ObjReader objReader = ObjReader();
	Obj3D* obj = new Obj3D;

	//string data = objReader.readObj("C:\\Users\\Acer\\Documents\\GitHub\\CGATR20251\\Objs\\mesa\\mesa01.obj");
	string data = objReader.readObj("C:\\Users\\Acer\\Documents\\GitHub\\CGATR20251\\Objs\\teapot\\teapot1.obj");
	//string data = objReader.readObj("C:\\Users\\Acer\\Documents\\GitHub\\CGATR20251\\Objs\\piramide\\pyramid.obj");

	Mesh* mesh = objReader.readMesh(data);
	obj->mesh = mesh;

	objects.push_back(obj);

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vShader, nullptr);
	glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fShader, nullptr);
	glCompileShader(fragmentShader);

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgram);

	glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);

	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPosition);

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));

	glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
	GLfloat cameraSpeed = 0.005f;

	glm::mat4 view;
	int viewLocation = glGetUniformLocation(shaderProgram, "view");

	glm::mat4 proj = glm::mat4(1.0f);
	proj = glm::perspective(glm::radians(90.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

	int projLocation = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(proj));

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		glm::vec3 direction = glm::normalize(cameraTarget - cameraPosition);
		glm::vec3 right = glm::normalize(glm::cross(up, direction));
		cameraUp = glm::cross(direction, right);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			cameraPosition += cameraSpeed * direction;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			cameraPosition -= cameraSpeed * direction;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			cameraPosition -= right * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			cameraPosition += right * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}

		cameraTarget = cameraPosition + direction;

		view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::scale(trans, glm::vec3(0.1f, 0.1f, 0.1f));

		int matrixLocation = glGetUniformLocation(shaderProgram, "transform");
		glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, glm::value_ptr(trans));

		mesh = obj->mesh;
		for (Group* g : mesh->groups) {
			glBindVertexArray(g->VAO);
			glDrawArrays(GL_TRIANGLES, 0, g->numVertices);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteProgram(shaderProgram);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void drawScene() {

}