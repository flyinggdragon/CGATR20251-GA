#include <GL/glew.h>      
#include <GLFW/glfw3.h> 
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>

#include "Group.h"
#include "Mesh.h"
#include "Obj3D.h"
#include "Shot.h"

using namespace std;

Obj3D::Obj3D() { }

Obj3D::~Obj3D() {
	delete mesh;

	for (Group* g : mesh->groups) {
		glDeleteVertexArrays(1, &g->VAO);
		glDeleteBuffers(1, &g->VBO);
	}
}

// Normaliza o objeto. Escala para caber dentro do frustum.
void Obj3D::ScaleObj() {
    float d = mesh->GetDiameter();
    transform = glm::scale(glm::mat4(1), glm::vec3(1/d, 1/d, 1/d));
}