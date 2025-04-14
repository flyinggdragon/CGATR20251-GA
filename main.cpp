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

	glViewport(0, 0, 800, 600);

	vector<Obj3D*> objects;

	ObjReader objReader = ObjReader();
	Obj3D* teapot = new Obj3D;
	Obj3D* table = new Obj3D;
	Obj3D* pyramid = new Obj3D;
	Obj3D* dragon = new Obj3D;
	Obj3D* torreDiPisa = new Obj3D;
	Obj3D* trout = new Obj3D;
	Obj3D* cube = new Obj3D;

	string tableData = objReader.readObj("C:\\Users\\Acer\\Documents\\GitHub\\CGATR20251\\Grau A\\Objs\\mesa\\mesa01.obj");
	string teapotData = objReader.readObj("C:\\Users\\Acer\\Documents\\GitHub\\CGATR20251\\Grau A\\Objs\\teapot\\teapot1.obj");
	string pyramidData = objReader.readObj("C:\\Users\\Acer\\Documents\\GitHub\\CGATR20251\\Grau A\\Objs\\piramide\\pyramid.obj");
	string dragonData = objReader.readObj("C:\\Users\\Acer\\Documents\\GitHub\\CGATR20251\\Grau A\\Objs\\dragon\\dragon.obj");
	string torreDiPisaData = objReader.readObj("C:\\Users\\Acer\\Documents\\GitHub\\CGATR20251\\Grau A\\Objs\\torredipisa\\torredipisa.obj");
	string troutData = objReader.readObj("C:\\Users\\Acer\\Documents\\GitHub\\CGATR20251\\Grau A\\Objs\\trout\\trout.obj");
	string cubeData = objReader.readObj("C:\\Users\\Acer\\Documents\\GitHub\\CGATR20251\\Grau A\\Objs\\cubo\\cubo1.obj");

	// Temporário até a criação do arquivo de cena.
	Mesh* mesh;
	
	mesh = objReader.readMesh(teapotData);
	teapot->mesh = mesh;
	teapot->transform = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
	teapot->transform = glm::translate(teapot->transform, glm::vec3(0.0f, 10.0f, 0.0f));

	mesh = objReader.readMesh(tableData);
	table->mesh = mesh;
	table->transform = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));

	mesh = objReader.readMesh(pyramidData);
	pyramid->mesh = mesh;
	pyramid->transform = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	pyramid->transform = glm::translate(pyramid->transform, glm::vec3(0.0f, 0.0f, -12.0f));

	mesh = objReader.readMesh(dragonData);
	dragon->mesh = mesh;
	dragon->transform = glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0f, 1.0f));
	dragon->transform = glm::translate(dragon->transform, glm::vec3(0.0f, 4.0f, -10.0f));

	mesh = objReader.readMesh(torreDiPisaData);
	torreDiPisa->mesh = mesh;
	torreDiPisa->transform = glm::scale(glm::mat4(1.0f), glm::vec3(0.06f, 0.06f, 0.06f));
	torreDiPisa->transform = glm::translate(torreDiPisa->transform, glm::vec3(45.1f, -0.1f, -78.0f));
	
	mesh = objReader.readMesh(troutData);
	trout->mesh = mesh;
	trout->transform = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
	
	mesh = objReader.readMesh(cubeData);
	cube->mesh = mesh;
	cube->transform = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));

	objects.push_back(teapot);
	objects.push_back(table);
	objects.push_back(pyramid);
	objects.push_back(dragon);
	objects.push_back(torreDiPisa);
	objects.push_back(trout);
	objects.push_back(cube);

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
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			cameraPosition -= right * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			cameraPosition += right * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		

		cameraTarget = cameraPosition + direction;

		view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (Obj3D* obj : objects) {
			int matrixLocation = glGetUniformLocation(shaderProgram, "transform");
			glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, glm::value_ptr(obj->transform));

			mesh = obj->mesh;
			for (Group* g : mesh->groups) {
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

void drawScene() {

}