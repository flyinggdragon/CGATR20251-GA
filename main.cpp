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
#include"Shot.h"

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

Camera* camera = nullptr;
Shot* shot = nullptr;

bool CollisionCheck(Obj3D* obj, Shot* shot);

void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
	camera->ProcessMouseMovement(xpos, ypos);
}

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
	string sceneData = sceneReader.ReadScene("C:\\Users\\Acer\\Documents\\GitHub\\CGATR20251\\GrauA\\cena.scene");
	vector<Obj3D*> objects = sceneReader.GetObjects(sceneData);

	for (Obj3D* obj : objects) {
		obj->mesh->GenMinMax();
		obj->ScaleObj();
		obj->mesh->GenMinMax();

		obj->center = obj->mesh->DetermineCenter();
		obj->radius = obj->mesh->GetDiameter() / 2;
	}

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

	camera = new Camera(WIDTH, HEIGHT, shaderProgram, 0.005f, 0.2f);
	glfwSetCursorPosCallback(window, MouseCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	Obj3D* shotObj{};

	float currentFrame = 0.0f, deltaTime = 0.0f, lastFrame = 0.0f;
	float shotLifetimeInSeconds = 2.5f, shotDurationInSeconds = 2.5f;

	while (!glfwWindowShouldClose(window)) {
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		for (int i = 0; i < objects.size(); ++i) {
			Obj3D* obj = objects[i];

			if (obj->name == "cube") shotObj = obj;

			int matrixLocation = glGetUniformLocation(shaderProgram, "transform");
			glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, glm::value_ptr(obj->transform));

			for (Group* g : obj->mesh->groups) {
				glBindVertexArray(g->VAO);
				glDrawArrays(GL_TRIANGLES, 0, g->numVertices);
			}
		}

		camera->ProcessInput(window);

		if (!shot && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			shot = new Shot(camera->position, camera->front);
		}

		if (shot) {
			shot->Update(deltaTime);

			int shotMatrixLocation = glGetUniformLocation(shaderProgram, "transform");
			glUniformMatrix4fv(shotMatrixLocation, 1, GL_FALSE, glm::value_ptr(shot->transform));

			for (Group* g : shot->shotObj->mesh->groups) {
				glBindVertexArray(g->VAO);
				glDrawElements(GL_TRIANGLES, g->numVertices, GL_UNSIGNED_INT, 0);
			}

			shotLifetimeInSeconds -= deltaTime;

			for (int i = 0; i < objects.size();) {
				Obj3D* obj = objects[i];

				if (CollisionCheck(obj, shot)) {
					if (obj->deletable) {
						delete obj;
						objects.erase(objects.begin() + i);
						continue;
					}
					else {
						shot->Reflect(obj);
					}
				}

				++i;
			}

			if (shotLifetimeInSeconds < 0.0f) {
				delete shot;
				shot = nullptr;

				shotLifetimeInSeconds = shotDurationInSeconds;
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

bool CollisionCheck(Obj3D* obj, Shot* shot) {
	glm::vec3 objMinWorld = glm::vec3(obj->transform * glm::vec4(obj->mesh->min, 1.0f));
	glm::vec3 objMaxWorld = glm::vec3(obj->transform * glm::vec4(obj->mesh->max, 1.0f));

	glm::vec3 shotMinWorld = glm::vec3(shot->transform * glm::vec4(shot->shotObj->mesh->min, 1.0f));
	glm::vec3 shotMaxWorld = glm::vec3(shot->transform * glm::vec4(shot->shotObj->mesh->max, 1.0f));

	bool collisionX = objMinWorld.x <= shotMaxWorld.x && objMaxWorld.x >= shotMinWorld.x;
	bool collisionY = objMinWorld.y <= shotMaxWorld.y && objMaxWorld.y >= shotMinWorld.y;
	bool collisionZ = objMinWorld.z <= shotMaxWorld.z && objMaxWorld.z >= shotMinWorld.z;

	return collisionX && collisionY && collisionZ;
}

/*
GLuint CreateBoundingBoxVAO(const glm::vec3& min, const glm::vec3& max, GLuint& VBO) {
	glm::vec3 corners[8] = {
		{min.x, min.y, min.z},
		{max.x, min.y, min.z},
		{max.x, max.y, min.z},
		{min.x, max.y, min.z},
		{min.x, min.y, max.z},
		{max.x, min.y, max.z},
		{max.x, max.y, max.z},
		{min.x, max.y, max.z},
	};

	GLuint indices[] = {
		0,1, 1,2, 2,3, 3,0, // bottom
		4,5, 5,6, 6,7, 7,4, // top
		0,4, 1,5, 2,6, 3,7  // sides
	};

	std::vector<glm::vec3> lines;
	for (int i = 0; i < 24; i++) {
		lines.push_back(corners[indices[i]]);
	}

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(glm::vec3), lines.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	glBindVertexArray(0);
	return VAO;
}

vector<GLuint> bboxVAOs(objects.size());
vector<GLuint> bboxVBOs(objects.size());

GLuint shotBBoxVAO, shotBBoxVBO;
shotBBoxVAO = createBoundingBoxVAO(
	shot->shotObj->mesh->min,
	shot->shotObj->mesh->max,
	shotBBoxVBO
);

bboxVAOs[i] = CreateBoundingBoxVAO(obj->mesh->min, obj->mesh->max, bboxVBOs[i]);

glBindVertexArray(bboxVAOs[i]);
glDrawArrays(GL_LINES, 0, 24);

glUniformMatrix4fv(shotMatrixLocation, 1, GL_FALSE, glm::value_ptr(shot->transform));

glBindVertexArray(shotBBoxVAO);
glDrawArrays(GL_LINES, 0, 24);

glDeleteVertexArrays(1, &shotBBoxVAO);
glDeleteBuffers(1, &shotBBoxVBO);
*/