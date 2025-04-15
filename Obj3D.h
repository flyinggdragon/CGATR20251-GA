#pragma once
#include<glm.hpp>
#include<vector>

#include"Mesh.h"

class Obj3D {
public:
	string name;
	Mesh* mesh;
	glm::mat4 transform = glm::mat4(1.0f);
	int status;
	bool deletable;
	glm::vec3 direction;
};