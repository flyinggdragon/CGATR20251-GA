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
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);

void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
	camera->ProcessMouseMovement(xpos, ypos);
}

// ---- Vertex Shader (completo para iluminação) ----
const char* vertShader = R"(
#version 410 core
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;

uniform mat4 model, view, projection;

out vec3 FragPos;
out vec3 Normal;

void main() {
    FragPos = vec3(model * vec4(position,1.0));
    Normal = mat3(transpose(inverse(model))) * normal;
    gl_Position = projection * view * vec4(FragPos,1.0);
}
)";

// ---- Fragment Shader (ambiente + difusa + especular + atenuação + fog) ----
const char* fragShader = R"(
#version 410 core
in vec3 FragPos;
in vec3 Normal;
out vec4 FragColor;

uniform vec3 ka; // Ambiente
uniform vec3 kd; // Difusa
uniform vec3 ks; // Especular
uniform float shininess;

uniform vec3 lightPos, lightColor;
uniform float attenuation;

uniform vec3 viewPos;

uniform vec3 fogColor;
uniform float fogStart, fogEnd;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    // Ambiente
    vec3 ambient = ka;

    // Difusa
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = kd * diff;

    // Especular
    float spec = pow(max(dot(reflectDir, viewDir),0.0), shininess);
    vec3 specular = ks * spec * lightColor;

    // Atenuação
    float d = length(lightPos - FragPos);
    float att = 1.0 / (1.0 + attenuation * d * d);

    vec3 color = (ambient + diffuse + specular) * att;

    // Fog (linear)
    float dist = length(viewPos - FragPos);
    float fogFactor = clamp((fogEnd - dist)/(fogEnd - fogStart), 0.0, 1.0);
    vec3 finalColor = mix(fogColor, color, fogFactor);

    FragColor = vec4(finalColor, 1.0);
}
)";

// ---- Dados de cubo ----
float cubeVertices[] = {
	// pos           // normal
	-1,-1,-1,   0,0,-1,  1,-1,-1,   0,0,-1,  1, 1,-1,   0,0,-1,  -1, 1,-1,   0,0,-1,
	-1,-1, 1,   0,0,1,   1,-1, 1,   0,0,1,   1, 1, 1,   0,0,1,   -1, 1, 1,   0,0,1,
	-1,-1,-1,  -1,0,0,   -1, 1,-1, -1,0,0,   -1, 1, 1, -1,0,0,   -1,-1, 1,  -1,0,0,
	 1,-1,-1,   1,0,0,    1, 1,-1,  1,0,0,    1, 1, 1,  1,0,0,    1,-1, 1,   1,0,0,
	-1,-1,-1,   0,-1,0,   1,-1,-1,  0,-1,0,   1,-1, 1,  0,-1,0,   -1,-1, 1,  0,-1,0,
	-1, 1,-1,   0,1,0,    1, 1,-1,  0,1,0,    1, 1, 1,  0,1,0,    -1, 1, 1,  0,1,0,
};
unsigned int cubeIndices[] = {
	0,1,2, 2,3,0,  4,5,6, 6,7,4,
	8,9,10, 10,11,8, 12,13,14, 14,15,12,
	16,17,18, 18,19,16, 20,21,22, 22,23,20
};

GLuint vao, vbo, ebo, shaderProg;

GLuint compile(GLenum type, const char* src) {
	GLuint id = glCreateShader(type);
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);
	return id;
}


int main() {
	// Iluminação
	glfwInit();
	GLFWwindow* win = glfwCreateWindow(900, 700, "Iluminacao Completa OpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(win);
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glfwSetCursorPosCallback(win, CursorPosCallback);

	// Instancia o leitor de cena e importa as informações do arquivo de cena.
	SceneReader sceneReader = SceneReader();
	string sceneData = sceneReader.ReadScene("C:\\Users\\Acer\\Documents\\GitHub\\CGATR20251\\GrauA\\cena.scene");

	// Após isso, carrega este vetor com os objetos que serão colocados em cena.
	vector<Obj3D*> objects = sceneReader.GetObjects(sceneData);

	GLuint vs = compile(GL_VERTEX_SHADER, vertShader);
	GLuint fs = compile(GL_FRAGMENT_SHADER, fragShader);
	shaderProg = glCreateProgram();
	glAttachShader(shaderProg, vs);
	glAttachShader(shaderProg, fs);
	glLinkProgram(shaderProg);
	glDeleteShader(vs); glDeleteShader(fs);

	// Cria a câmera.
	camera = new Camera(900, 700, shaderProg, 0.05f, 0.2f);
	glfwSetCursorPosCallback(win, MouseCallback);
	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	while (!glfwWindowShouldClose(win)) {
		glClearColor(0.75f, 0.85f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Processa input da câmera (atualiza view matrix e posição)
		camera->ProcessInput(win);

		// Usa view e posição da câmera
		glm::mat4 view = camera->view;
		glm::vec3 camPos = camera->position;

		glUseProgram(shaderProg);

		glUniformMatrix4fv(glGetUniformLocation(shaderProg, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shaderProg, "projection"), 1, GL_FALSE, glm::value_ptr(camera->proj));

		glUniform3fv(glGetUniformLocation(shaderProg, "viewPos"), 1, glm::value_ptr(camPos));

		// Iluminação
		glUniform3f(glGetUniformLocation(shaderProg, "ka"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(shaderProg, "kd"), 0.7f, 0.3f, 0.3f);
		glUniform3f(glGetUniformLocation(shaderProg, "ks"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shaderProg, "shininess"), 40.0f);
		glUniform3f(glGetUniformLocation(shaderProg, "lightPos"), 3.0f, 4.0f, 3.0f);
		glUniform3f(glGetUniformLocation(shaderProg, "lightColor"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shaderProg, "attenuation"), 0.12f);

		// Fog
		glUniform3f(glGetUniformLocation(shaderProg, "fogColor"), 0.7f, 0.7f, 0.9f);
		glUniform1f(glGetUniformLocation(shaderProg, "fogStart"), 6.0f);
		glUniform1f(glGetUniformLocation(shaderProg, "fogEnd"), 20.0f);

		// Renderiza objetos
		for (Obj3D* obj : objects) {
			glUniformMatrix4fv(glGetUniformLocation(shaderProg, "model"), 1, GL_FALSE, glm::value_ptr(obj->transform));

			for (Group* g : obj->mesh->groups) {
				glBindVertexArray(g->VAO);
				glDrawArrays(GL_TRIANGLES, 0, g->numVertices);
			}
		}

		glfwSwapBuffers(win);
		glfwPollEvents();
	}
	glfwDestroyWindow(win);
	glfwTerminate();
	return 0;
}

// Callback para movimentos do mouse.
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	camera->ProcessMouseMovement(xpos, ypos);
	/*
	// Se está no modo de arrasto.
	if (isDragging && draggedPointIndex >= 0 && draggedPointIndex < controlPoints.size()) {
		float x = (float)xpos;
		float y = (float)(HEIGHT - ypos);

		// Atualiza a posição do ponto de controle sendo arrastado.
		controlPoints[draggedPointIndex] = glm::vec3(x, y, 0.0f);

		GenerateSpline();
	}*/
}