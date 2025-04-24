#include <GL/glew.h>      
#include <GLFW/glfw3.h> 
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>

#include "Obj3D.h"
#include "Group.h"

using namespace std;

Obj3D::Obj3D() { }

Obj3D::~Obj3D() {
	delete mesh;

	for (Group* g : mesh->groups) {
		glDeleteVertexArrays(1, &g->VAO);
		glDeleteBuffers(1, &g->VBO);
	}
}

void Obj3D::ScaleObj() {
    float d = mesh->GetDiameter();
    transform = glm::scale(glm::mat4(1), glm::vec3(1/d, 1/d, 1/d));
}