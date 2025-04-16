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
#include"SceneReader.h"
#include"Camera.h"

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

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "SceneDisplayer", nullptr, nullptr);

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

	SceneReader sceneReader = SceneReader();
	string sceneData = sceneReader.readScene("C:\\Users\\Acer\\Documents\\GitHub\\CGATR20251\\GrauA\\cena.scene");
	vector<Obj3D*> objects = sceneReader.getObjects(sceneData);

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vShader, nullptr);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fShader, nullptr);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgram);

	Camera camera = Camera(WIDTH, HEIGHT, shaderProgram);
	
	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.position += camera.speed * camera.direction;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.position -= camera.speed * camera.direction;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.position -= camera.right * camera.speed;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.position += camera.right * camera.speed;
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		
		camera.target = camera.position + camera.direction;

		camera.view = glm::lookAt(camera.position, camera.target, camera.up);
		glUniformMatrix4fv(camera.viewLocation, 1, GL_FALSE, glm::value_ptr(camera.view));

		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (Obj3D* obj : objects) {
			int matrixLocation = glGetUniformLocation(shaderProgram, "transform");
			glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, glm::value_ptr(obj->transform));

			for (Group* g : obj->mesh->groups) {
				glBindVertexArray(g->VAO);
				glDrawArrays(GL_TRIANGLES, 0, g->numVertices);
			}
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteProgram(shaderProgram);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}