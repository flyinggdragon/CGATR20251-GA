#pragma once
#include<glm.hpp>
#include<vector>

#include"Mesh.h"

class Obj3D {
public:
	Mesh* mesh;
	glm::mat4 transform;
	int status;
	bool deletable;
	glm::vec3 direction;
};