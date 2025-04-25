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

// Protótipos de função.
bool CollisionCheck(Obj3D* obj, Shot* shot);

void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
	camera->ProcessMouseMovement(xpos, ypos);
}

int main() {
	// Configurações do OpenGL
	if (!glfwInit()) {
		cerr << "Falha ao inicializar GLFW" << endl;
		return -1;
	}

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

	// Instancia o leitor de cena e importa as informações do arquivo de cena.
	SceneReader sceneReader = SceneReader();
	string sceneData = sceneReader.ReadScene("C:\\Users\\Acer\\Documents\\GitHub\\CGATR20251\\GrauA\\cena.scene");
	
	// Após isso, carrega este vetor com os objetos que serão colocados em cena.
	vector<Obj3D*> objects = sceneReader.GetObjects(sceneData);

	// Configuração dos shaders.
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

	Obj3D* shotObj;

	// Cria a câmera.
	camera = new Camera(WIDTH, HEIGHT, shaderProgram, 0.005f, 0.2f);
	glfwSetCursorPosCallback(window, MouseCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Variáveis relacionadas ao tiro.
	float currentFrame = 0.0f, deltaTime = 0.0f, lastFrame = 0.0f;
	float shotLifetimeInSeconds = 2.5f, shotDurationInSeconds = 2.5f;

	while (!glfwWindowShouldClose(window)) {
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		// Varre por todos os objetos que devem ser carregados para a cena.
		for (int i = 0; i < objects.size(); ++i) {
			Obj3D* obj = objects[i];

			// Envia a matriz de transformação do objeto para o vertex shader.
			int matrixLocation = glGetUniformLocation(shaderProgram, "transform");
			glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, glm::value_ptr(obj->transform));

			// Por fim, desenha o objeto em tela, grupo por grupo.
			for (Group* g : obj->mesh->groups) {
				glBindVertexArray(g->VAO);
				glDrawArrays(GL_TRIANGLES, 0, g->numVertices);
			}
		}

		// Movimentação da câmera
		camera->ProcessInput(window);

		// Instancia o tiro ao apertar SPACE, somente se não houver outro tiro em tela.
		if (!shot && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			shot = new Shot(camera->position, camera->front);
		}

		// Se o tiro estiver em tela...
		if (shot) {
			// Atualiza sua posição.
			shot->Update(deltaTime);

			// Envia a matriz de transformação ao shader.
			int shotMatrixLocation = glGetUniformLocation(shaderProgram, "transform");
			glUniformMatrix4fv(shotMatrixLocation, 1, GL_FALSE, glm::value_ptr(shot->transform));

			// Desenha o tiro em tela.
			for (Group* g : shot->shotObj->mesh->groups) {
				glBindVertexArray(g->VAO);
				glDrawElements(GL_TRIANGLES, g->numVertices, GL_UNSIGNED_INT, 0);
			}

			// Decrementa o timer de lifetime.
			shotLifetimeInSeconds -= deltaTime;

			for (int i = 0; i < objects.size();) {
				Obj3D* obj = objects[i];

				// Verifica a colisão para cada objeto em cena e exclui/reflete de acordo.
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

			// Se o lifetime acabar, exclui o tiro e reseta o tempo.
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

// Verifica a colisão.
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